#include "server.hpp"
#include "../CGI/CgiOrchestrator.hpp"

Server::Server()
	: _epollManager(NULL), _connectionManager(NULL), _cgiOrchestrator(NULL), _isValid(false)
{
}

Server::Server(const Port& port, const IpAddr& ipAddr, const ServerBlock& serverConfig)
	: _serverSocket(port, ipAddr), _epollManager(NULL), _connectionManager(NULL),
	  _cgiOrchestrator(NULL), _isValid(false)
{
	PollCapacity maxEvents(1024);
	_epollManager	   = new EpollManager(maxEvents);
	_connectionManager = new ConnectionManager(*_epollManager, maxEvents);
	_connectionManager->configureMethodOrchestrator(serverConfig);
	_cgiOrchestrator   = new CgiOrchestrator(*_epollManager);
	_connectionManager->setCgiOrchestrator(_cgiOrchestrator);

	if (!_serverSocket.isValid())
	{
		handleError("Failed to initialize socket");
		return;
	}
	if (!_serverSocket.setBind())
	{
		handleError("Failed to bind socket");
		return;
	}
	if (!_serverSocket.setListen(SOMAXCONN))
	{
		handleError("Failed to listen on socket");
		return;
	}

	_epollManager->addFd(_serverSocket.getPollFd(), POLLIN);
	_isValid = true;
}

Server::~Server()
{
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

	if (isServerSocket(fd))
	{
		_connectionManager->acceptNewClient(_serverSocket);
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

bool Server::isServerSocket(int fd) const { return (fd == _serverSocket.getPollFd()); }
