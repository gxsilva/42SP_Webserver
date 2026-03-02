#include "includes/connectionManager.hpp"

ConnectionManager::ConnectionManager(EpollManager& epollManager, const MaxEvents& maxEvents)
: _epollManager(epollManager),
_clients(maxEvents.getAmount(), (ClientSocket*)NULL)
{}

ConnectionManager::~ConnectionManager()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		delete _clients[i];
	}
	_clients.clear();
}

void	ConnectionManager::acceptNewClient(ServerSocket& serverSocket)
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

void	ConnectionManager::disconnectClient(int fd)
{
	_epollManager.removeFd(fd);
	delete _clients[fd];
	_clients[fd] = NULL;
}

void ConnectionManager::handleClientData(int fd)
{
    ClientSocket* client = _clients[fd];
    
    if (client == NULL)
        return;

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = client->receiveData(buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0)
    {
        disconnectClient(fd);
        return;
    }

    std::cout << "Data from fd " << fd << ": " << buffer << std::endl;
    
    sendTestHttpResponse(*client);
    shutdown(fd, SHUT_WR);
    
    std::cout << "Client disconnected: fd " << fd << std::endl;
    
    disconnectClient(fd);
}