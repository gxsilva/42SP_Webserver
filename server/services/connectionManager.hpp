#ifndef	CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../infra/serverSocket.hpp"
#include "../infra/clientSocket.hpp"
#include "../domain/epollEvents.hpp"
#include "epollManager.hpp"

#include <vector>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include "../infra/testHttpResponse.hpp"

class	ConnectionManager
{
	private:	
		EpollManager&				_epollManager;
		std::vector<ClientSocket*>	_clients;

		void	disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents);
		~ConnectionManager();

		void	acceptNewClient(ServerSocket& serverSocket);
		void	handleClientRead(int fd);
		void	handleClientWrite(int fd);
};

#endif