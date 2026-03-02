#include "includes/connectionManager.hpp"

ConnectionManager::ConnectionManager(EpollManager& epollManager, const MaxEvents& maxEvents)
: _epollManager(epollManager),
_sessions(maxEvents.getAmount(), (HttpSession*)NULL)
{}

ConnectionManager::~ConnectionManager()
{
	for (size_t i = 0; i < _sessions.size(); i++)
		delete _sessions[i];
	_sessions.clear();
}

void	ConnectionManager::acceptNewClient(ServerSocket& serverSocket)
{
	int newClient = serverSocket.setAccept();

	if (newClient < 0)
		return;

	ClientSocket* socket = new ClientSocket(newClient);
	if (!socket->isValid())
	{
		delete socket;
		close(newClient);
		return;
	}

	HttpSession* session = new HttpSession(socket);

	_epollManager.addFd(newClient, POLLIN);
	if (static_cast<size_t>(newClient) >= _sessions.size())
		_sessions.resize(newClient + 128, (HttpSession*)NULL);

	_sessions[newClient] = session;

	std::cout << "New client connected: fd " << newClient << std::endl;
}

void	ConnectionManager::disconnectClient(int fd)
{
	_epollManager.removeFd(fd);
	delete _sessions[fd];
	_sessions[fd] = NULL;
}

void ConnectionManager::handleClientData(int fd)
{
	HttpSession* session = _sessions[fd];

	if (session == NULL)
		return;

	if (session->getState() == HttpSession::READING)
	{
		char buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = session->getSocket()->receiveData(buffer, sizeof(buffer) - 1);

		if (bytesRead <= 0)
		{
			disconnectClient(fd);
			return;
		}

		session->appendToBuffer(buffer, bytesRead);
		std::cout << "Data from fd " << fd << ": " << buffer << std::endl;

		if (!session->isReadComplete())
			return;

		session->setState(HttpSession::WRITING);
	}

	if (session->getState() == HttpSession::WRITING)
	{
		sendTestHttpResponse(*session->getSocket());
		shutdown(fd, SHUT_WR);
		session->setState(HttpSession::CLOSED);
	}

	if (session->getState() == HttpSession::CLOSED)
	{
		std::cout << "Client disconnected: fd " << fd << std::endl;
		disconnectClient(fd);
	}
}