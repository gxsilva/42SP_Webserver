#ifndef	CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../../infra/includes/serverSocket.hpp"
#include "../../infra/includes/clientSocket.hpp"
#include "../../domain/includes/epollEvents.hpp"
#include "epollManager.hpp"

#include <vector>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include "../../infra/includes/testHttpResponse.hpp"

class	ConnectionManager
{
	private:	
		EpollManager&				_epollManager;
		std::vector<ClientSocket*>	_clients;

		void	disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const MaxEvents& maxEvents);
		~ConnectionManager();

		void	acceptNewClient(ServerSocket& serverSocket);
		void	handleClientData(int fd);
};

#endif