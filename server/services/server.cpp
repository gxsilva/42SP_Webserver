#include "includes/server.hpp"

Server::Server()
: _epollManager(NULL), 
_connectionManager(NULL), 
_isValid(false) 
{}

Server::Server(const Port& port, const IpAddr& ipAddr)
: _serverSocket(port, ipAddr), _epollManager(NULL), _connectionManager(NULL), _isValid(false)
{
    PollCapacity maxEvents(1024);
    _epollManager = new EpollManager(maxEvents);
    
    if (!_epollManager)
    {
        handleError("Failed to create event manager");
        return;
    }
    
    _connectionManager = new ConnectionManager(*_epollManager, maxEvents);
    if (!_connectionManager)
    {
        handleError("Failed to create connection manager");
        return;
    }
    
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
    delete _connectionManager;
    delete _epollManager;
}

bool	Server::isValid() const
{
    return _isValid;
}

void Server::run()
{
    while (true)
    {
        int count = _epollManager->waitForEvents();
        
        if (count < 0)
            break;
        processEvents(count);
    }
}

void Server::processEvents(int count)
{
    for (int i = 0; i < count; i++)
        handleEventByIndex(i);
}

void Server::handleEventByIndex(int index)
{
    int fd = _epollManager->getEventFd(index);
    unsigned int flags = _epollManager->getEventFlags(index);
    
    if (isServerSocket(fd))
    {
        _connectionManager->acceptNewClient(_serverSocket);
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

bool Server::isServerSocket(int fd) const
{
    return (fd == _serverSocket.getPollFd());
}
