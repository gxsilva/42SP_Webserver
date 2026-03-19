#include "server.hpp"
#include "../CGI/CgiOrchestrator.hpp"

Server::Server() : _epollManager(NULL), _connectionManager(NULL), _cgiOrchestrator(NULL), _isValid(false) {}

Server::Server(const std::vector< ServerBlock >& serverConfigs)
	: _serverConfigs(serverConfigs), _epollManager(NULL), _connectionManager(NULL), _cgiOrchestrator(NULL),
	  _isValid(false)
{
	if (_serverConfigs.empty())
		return;

	PollCapacity maxEvents(1024);
	_epollManager	   = new EpollManager(maxEvents);
	_connectionManager = new ConnectionManager(*_epollManager, maxEvents);
	_connectionManager->configureMethodOrchestrator(_serverConfigs[0]);
	_cgiOrchestrator   = new CgiOrchestrator(*_epollManager);
	_connectionManager->setCgiOrchestrator(_cgiOrchestrator);

	for (size_t i = 0; i < _serverConfigs.size(); ++i)
	{
		std::string host = _serverConfigs[i].host;
		if (host.empty())
			host = "0.0.0.0";

		Port port(_serverConfigs[i].port);
		IpAddr ipAddr(host);

		ServerSocket* serverSocket = new ServerSocket(port, ipAddr);
		if (!serverSocket->isValid())
		{
			delete serverSocket;
			handleError("Failed to initialize socket");
			return;
		}
		if (!serverSocket->setBind())
		{
			delete serverSocket;
			handleError("Failed to bind socket");
			return;
		}
		if (!serverSocket->setListen(SOMAXCONN))
		{
			delete serverSocket;
			handleError("Failed to listen on socket");
			return;
		}

		_epollManager->addFd(serverSocket->getPollFd(), POLLIN);
		_serverSockets.push_back(serverSocket);
	}

	_isValid = true;
}

Server::~Server()
{
	for (size_t i = 0; i < _serverSockets.size(); ++i)
		delete _serverSockets[i];
	_serverSockets.clear();

	delete _cgiOrchestrator;
	delete _connectionManager;
	delete _epollManager;
}

bool Server::isValid() const { return _isValid; }

void Server::run()
{
	while (true)
	{
		int count = _epollManager->waitForEvents(1000);

		if (count < 0)
			break;
		processEvents(count);
	}
}

void Server::processEvents(int count)
{
	for (int i = 0; i < count; i++)
		handleEventByIndex(i);
	_connectionManager->dispatchCgiResponses();
}

void Server::handleEventByIndex(int index)
{
	int			 fd	   = _epollManager->getEventFd(index);
	unsigned int flags = _epollManager->getEventFlags(index);

	ServerSocket* serverSocket = findServerSocketByFd(fd);
	if (serverSocket != NULL)
	{
		_connectionManager->acceptNewClient(*serverSocket);
		return;
	}

	if (_connectionManager->isCgiFd(fd))
	{
		_connectionManager->handleCgiEvent(fd, flags);
		return;
	}

	if (flags & EPOLLOUT)
	{
		_connectionManager->handleClientWrite(fd);
		return;
	}

	if (flags & EPOLLIN)
		_connectionManager->handleClientRead(fd);
}

ServerSocket* Server::findServerSocketByFd(int fd) const
{
	for (size_t i = 0; i < _serverSockets.size(); ++i)
	{
		if (_serverSockets[i] != NULL && _serverSockets[i]->getPollFd() == fd)
			return _serverSockets[i];
	}
	return NULL;
}
