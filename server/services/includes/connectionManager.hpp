#ifndef	CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../../domain/includes/httpSession.hpp"
#include "../../domain/includes/epollEvents.hpp"
#include "../../infra/includes/serverSocket.hpp"
#include "../../infra/includes/clientSocket.hpp"
#include "../../infra/includes/testHttpResponse.hpp"
#include "epollManager.hpp"

#include <vector>
#include <poll.h>
#include <unistd.h>
#include <iostream>

class	ConnectionManager
{
	private:	
		EpollManager&				_epollManager;
		std::vector<HttpSession*>	_sessions;

		void	disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const MaxEvents& maxEvents);
		~ConnectionManager();

		void	acceptNewClient(ServerSocket& serverSocket);
		void	handleClientData(int fd);
};

#endif