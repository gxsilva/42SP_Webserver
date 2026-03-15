#include "connectionManager.hpp"

static bool isCgiRequest(const std::string& uri)
{
	if (uri.size() >= 3 && uri.substr(uri.size() - 3) == ".py")
		return true;
	if (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".php")
		return true;
	return false;
}

static std::string getInterpreter(const std::string& uri)
{
	if (uri.size() >= 3 && uri.substr(uri.size() - 3) == ".py")
		return "/usr/bin/python3";
	return "/usr/bin/php-cgi";
}

static CgiRouteConfig buildCgiConfig(const HttpRequest& request)
{
	CgiRouteConfig config;
	config.scriptPath = "./www" + request.getUri();
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

void ConnectionManager::handleClientRead(int fd)
{
	ClientSocket* client = _clients[fd];

	if (client == NULL)
		return;

	char	buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytesRead = client->receiveData(buffer, sizeof(buffer) - 1);

	if (bytesRead <= 0)
	{
		disconnectClient(fd);
		return;
	}

	Result<HttpRequest> result = _parseUseCase.execute(std::string(buffer, bytesRead));

	if (result.isErr())
	{
		std::cerr << "[Validation Error] FD: " << fd << " | Error: " << result.getError()
				  << std::endl;

		HttpResponse response;
		response.setStatusCode(400);
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Connection", "close");
		response.setBody("Invalid Request: " + result.getError());
		bufferTestHttpResponse(*client, response);
		_epollManager.modifyFd(fd, EPOLLOUT);
		return;
	}

	HttpRequest request = result.getValue();

	std::cout << "[Request] FD: " << fd << " | Method: " << request.getMethod()
			  << " | URI: " << request.getUri() << std::endl;

	if (isCgiRequest(request.getUri()) && _cgiOrchestrator != NULL)
	{
		CgiRouteConfig config = buildCgiConfig(request);
		if (_cgiOrchestrator->startCgi(fd, request, config))
			return;

		HttpResponse response;
		response.setStatusCode(502);
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Connection", "close");
		response.setBody("<html><body><h1>502 Bad Gateway</h1></body></html>");
		bufferTestHttpResponse(*client, response);
		_epollManager.modifyFd(fd, EPOLLOUT);
		return;
	}

	HttpResponse response;
	response.setStatusCode(200);
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Connection", "close");
	response.setBody("Static file logic goes here!");
	bufferTestHttpResponse(*client, response);
	_epollManager.modifyFd(fd, EPOLLOUT);
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
