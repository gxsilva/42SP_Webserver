#include "connectionManager.hpp"

static bool isCgiRequest(const std::string& uri)
{
	std::string path = uri;
	std::string::size_type queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);

	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return true;
	if (path.size() >= 4 && path.substr(path.size() - 4) == ".php")
		return true;
	return false;
}

static std::string getInterpreter(const std::string& uri)
{
	std::string path = uri;
	std::string::size_type queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);

	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return "/usr/bin/python3";
	return "/usr/bin/php-cgi";
}

static CgiRouteConfig buildCgiConfig(const HttpRequest& request)
{
	CgiRouteConfig config;
	std::string uriPath = request.getUri();
	std::string::size_type queryPos = uriPath.find('?');
	if (queryPos != std::string::npos)
		uriPath = uriPath.substr(0, queryPos);

	config.scriptPath = "./www" + uriPath;
	config.interpreterPath = getInterpreter(request.getUri());
	config.serverName = "localhost";
	config.serverPort = 8080;
	return config;
}

ConnectionManager::ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents)
	: _epollManager(epollManager), _clients(maxEvents.getAmount(), (ClientSocket*)NULL),
	  _cgiOrchestrator(NULL)
{
}

ConnectionManager::~ConnectionManager()
{
	for (size_t i = 0; i < _clients.size(); i++)
		delete _clients[i];
	_clients.clear();
}

void ConnectionManager::setCgiOrchestrator(CgiOrchestrator* orch)
{
	_cgiOrchestrator = orch;
}

void ConnectionManager::configureMethodOrchestrator(const ServerBlock& serverConfig)
{
	_methodOrchestrator.configure(serverConfig);
}

void ConnectionManager::acceptNewClient(ServerSocket& serverSocket)
{
	int newClient = serverSocket.setAccept();

	if (newClient < 0)
		return;

	ClientSocket* client = new ClientSocket(newClient);
	if (!client->isValid())
	{
		delete client;
		close(newClient);
		return;
	}

	_epollManager.addFd(newClient, POLLIN);
	if (static_cast<size_t>(newClient) >= _clients.size())
		_clients.resize(newClient + 128, (ClientSocket*)NULL);

	_clients[newClient] = client;

	std::cout << "New client connected: fd " << newClient << std::endl;
}

void ConnectionManager::disconnectClient(int fd)
{
	if (_cgiOrchestrator != NULL)
		_cgiOrchestrator->cancelForClient(fd);
	_epollManager.removeFd(fd);
	delete _clients[fd];
	_clients[fd] = NULL;
}

void ConnectionManager::queueResponse(int fd, ClientSocket& client, const HttpResponse& response)
{
	bufferTestHttpResponse(client, response);
	_epollManager.modifyFd(fd, EPOLLOUT);
}

bool ConnectionManager::readRawRequestOrDisconnect(int fd,
	ClientSocket& client,
	std::string& rawRequest)
{
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytesRead = client.receiveData(buffer, sizeof(buffer) - 1);

	if (bytesRead <= 0)
	{
		disconnectClient(fd);
		return false;
	}

	rawRequest.assign(buffer, bytesRead);
	return true;
}

bool ConnectionManager::parseRequestOrRespondBadRequest(int fd,
	ClientSocket& client,
	const std::string& rawRequest,
	HttpRequest& request)
{
	Result<HttpRequest> result = _parseUseCase.execute(rawRequest);

	if (result.isErr())
	{
		std::cerr << "[Validation Error] FD: " << fd << " | Error: " << result.getError()
				  << std::endl;

		HttpResponse response;
		response.setStatusCode(400);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("Invalid Request: " + result.getError());
		queueResponse(fd, client, response);
		return false;
	}

	request = result.getValue();
	return true;
}

bool ConnectionManager::handleCgiOrRespondBadGateway(int fd,
	ClientSocket& client,
	const HttpRequest& request)
{
	if (!isCgiRequest(request.getUri()) || _cgiOrchestrator == NULL)
		return false;

	CgiRouteConfig config = buildCgiConfig(request);
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

	std::cout << "[Request] FD: " << fd << " | Method: " << request.getMethod()
			  << " | URI: " << request.getUri() << std::endl;

	if (handleCgiOrRespondBadGateway(fd, *client, request))
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

	std::vector<std::pair<int, HttpResponse> > results = _cgiOrchestrator->collectFinished();

	for (size_t i = 0; i < results.size(); ++i)
	{
		int clientFd = results[i].first;
		if (clientFd < 0 || static_cast<size_t>(clientFd) >= _clients.size())
			continue;

		ClientSocket* client = _clients[clientFd];
		if (client == NULL)
			continue;

		bufferTestHttpResponse(*client, results[i].second);
		_epollManager.modifyFd(clientFd, EPOLLOUT);
	}
}