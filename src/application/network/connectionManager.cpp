#include "connectionManager.hpp"
#include "../../infrastructure/io/request/HttpRequestFramer.hpp"
#include "../CGI/CgiRouteResolver.hpp"

ConnectionManager::ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents)
	: _epollManager(epollManager), _clients(maxEvents.getAmount(), (ClientSocket*)NULL), _cgiOrchestrator(NULL),
	  _hasDefaultServerConfig(false)
{
	_defaultServerConfig.port			   = 80;
	_defaultServerConfig.clientMaxBodySize = 0;
}

ConnectionManager::~ConnectionManager()
{
	for (size_t i = 0; i < _clients.size(); i++)
		delete _clients[i];
	_clients.clear();
	_clientServerConfigs.clear();
	_listenerServerConfigs.clear();
	_requestReadBuffers.clear();
}

void ConnectionManager::setCgiOrchestrator(CgiOrchestrator* orch) { _cgiOrchestrator = orch; }

void ConnectionManager::configureMethodOrchestrator(const ServerBlock& serverConfig)
{
	_defaultServerConfig	= serverConfig;
	_hasDefaultServerConfig = true;
}

void ConnectionManager::registerListenerConfig(int listenerFd, const ServerBlock& serverConfig)
{
	if (listenerFd < 0)
		return;
	_listenerServerConfigs[listenerFd] = serverConfig;
}

void ConnectionManager::acceptNewClient(ServerSocket& serverSocket)
{
	while (true)
	{
		int newClient = serverSocket.setAccept();

		if (newClient < 0)
			break;

		ClientSocket* client = new ClientSocket(newClient);
		if (!client->isValid())
		{
			delete client;
			close(newClient);
			continue;
		}

		_epollManager.addFd(newClient, EPOLLIN);
		if (static_cast< size_t >(newClient) >= _clients.size())
			_clients.resize(newClient + 128, (ClientSocket*)NULL);

		_clients[newClient]				  = client;
		const ServerBlock* listenerConfig = findListenerServerConfig(serverSocket.getPollFd());
		if (listenerConfig != NULL)
			_clientServerConfigs[newClient] = *listenerConfig;
		else if (_hasDefaultServerConfig)
			_clientServerConfigs[newClient] = _defaultServerConfig;

		std::cout << "New client connected: fd " << newClient << std::endl;
	}
}

void ConnectionManager::disconnectClient(int fd)
{
	if (_cgiOrchestrator != NULL)
		_cgiOrchestrator->cancelForClient(fd);
	_epollManager.removeFd(fd);
	_clientServerConfigs.erase(fd);
	_requestReadBuffers.erase(fd);
	delete _clients[fd];
	_clients[fd] = NULL;
}

const ServerBlock* ConnectionManager::findClientServerConfig(int clientFd) const
{
	std::map< int, ServerBlock >::const_iterator it = _clientServerConfigs.find(clientFd);
	if (it == _clientServerConfigs.end())
		return NULL;
	return &it->second;
}

const ServerBlock* ConnectionManager::findListenerServerConfig(int listenerFd) const
{
	std::map< int, ServerBlock >::const_iterator it = _listenerServerConfigs.find(listenerFd);
	if (it == _listenerServerConfigs.end())
		return NULL;
	return &it->second;
}

const ServerBlock& ConnectionManager::resolveServerConfigForClient(int clientFd) const
{
	const ServerBlock* clientConfig = findClientServerConfig(clientFd);
	if (clientConfig != NULL)
		return *clientConfig;
	return _defaultServerConfig;
}

size_t ConnectionManager::resolveMaxBodySizeForClient(int clientFd) const
{
	const ServerBlock* clientConfig = findClientServerConfig(clientFd);
	if (clientConfig != NULL)
		return clientConfig->clientMaxBodySize;
	if (_hasDefaultServerConfig)
		return _defaultServerConfig.clientMaxBodySize;
	return 0;
}

void ConnectionManager::queueResponse(int fd, ClientSocket& client, const HttpResponse& response)
{
	bufferTestHttpResponse(client, response);
	_epollManager.modifyFd(fd, EPOLLOUT);
}

bool ConnectionManager::readRawRequestOrDisconnect(int fd, ClientSocket& client, std::string& rawRequest)
{
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytesRead = client.receiveData(buffer, sizeof(buffer) - 1);

	if (bytesRead == 0)
	{
		disconnectClient(fd);
		return false;
	}

	if (bytesRead < 0)
		return false;

	_requestReadBuffers[fd].append(buffer, static_cast< size_t >(bytesRead));
	return popCompleteRequestFromBuffer(fd, rawRequest);
}

bool ConnectionManager::popCompleteRequestFromBuffer(int clientFd, std::string& rawRequest)
{
	std::map< int, std::string >::iterator it = _requestReadBuffers.find(clientFd);
	if (it == _requestReadBuffers.end())
		return false;

	return HttpRequestFramer::popCompleteRequestFromBuffer(it->second, rawRequest);
}

bool ConnectionManager::parseRequestOrRespondBadRequest(int fd, ClientSocket& client, const std::string& rawRequest,
														HttpRequest& request)
{
	size_t				  maxBodySize = resolveMaxBodySizeForClient(fd);
	Result< HttpRequest > result	  = _parseUseCase.execute(rawRequest, maxBodySize);

	if (result.isErr())
	{
		std::cerr << "[Validation Error] FD: " << fd << " | Error: " << result.getError() << std::endl;
		const HttpRequestValidationIssue& issue = _parseUseCase.getLastIssue();

		HttpResponse response;
		if (issue.hasError())
			response.setStatusCode(static_cast< int >(issue.getStatusCode()));
		else
			response.setStatusCode(400);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("Invalid Request: " + result.getError() + "\n");
		queueResponse(fd, client, response);
		return false;
	}

	request = result.getValue();
	return true;
}

bool ConnectionManager::handleCgiOrRespondBadGateway(int fd, ClientSocket& client, const HttpRequest& request,
													 const ServerBlock& serverConfig)
{
	if (!CgiRouteResolver::isCgiRequest(request.getUri()) || _cgiOrchestrator == NULL)
		return false;

	CgiRouteConfig config = CgiRouteResolver::buildConfig(request, serverConfig);
	if (_cgiOrchestrator->startCgi(fd, request, config))
		return true;

	HttpResponse response;
	response.setStatusCode(502);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Connection", "close");
	response.setBody("<html><body><h1>502 Bad Gateway</h1></body></html>");
	queueResponse(fd, client, response);
	return true;
}

void ConnectionManager::handleClientRead(int fd)
{
	ClientSocket* client = _clients[fd];

	if (client == NULL)
		return;

	std::string rawRequest;
	if (!readRawRequestOrDisconnect(fd, *client, rawRequest))
		return;

	HttpRequest request;
	if (!parseRequestOrRespondBadRequest(fd, *client, rawRequest, request))
		return;

	const ServerBlock& serverConfig = resolveServerConfigForClient(fd);
	_methodOrchestrator.configure(serverConfig);

	std::cout << "[Request] FD: " << fd << " | Method: " << request.getMethod() << " | URI: " << request.getUri()
			  << std::endl;

	if (handleCgiOrRespondBadGateway(fd, *client, request, serverConfig))
		return;

	HttpResponse response = _methodOrchestrator.handle(request);
	queueResponse(fd, *client, response);
}

void ConnectionManager::handleClientWrite(int fd)
{
	ClientSocket* client = _clients[fd];

	if (client == NULL)
		return;

	if (!client->flushWriteBuffer())
	{
		disconnectClient(fd);
		return;
	}

	if (!client->hasDataToSend())
	{
		std::cout << "Response sent to fd " << fd << std::endl;
		disconnectClient(fd);
	}
}

bool ConnectionManager::isCgiFd(int fd) const
{
	if (_cgiOrchestrator == NULL)
		return false;
	return _cgiOrchestrator->isCgiFd(fd);
}

void ConnectionManager::handleCgiEvent(int fd, unsigned int flags)
{
	if (_cgiOrchestrator == NULL)
		return;
	_cgiOrchestrator->handleEvent(fd, flags);
}

void ConnectionManager::dispatchCgiResponses()
{
	if (_cgiOrchestrator == NULL)
		return;

	std::vector< std::pair< int, HttpResponse > > results = _cgiOrchestrator->collectFinished();

	for (size_t i = 0; i < results.size(); ++i)
	{
		int clientFd = results[i].first;
		if (clientFd < 0 || static_cast< size_t >(clientFd) >= _clients.size())
			continue;

		ClientSocket* client = _clients[clientFd];
		if (client == NULL)
			continue;

		bufferTestHttpResponse(*client, results[i].second);
		_epollManager.modifyFd(clientFd, EPOLLOUT);
	}
}
