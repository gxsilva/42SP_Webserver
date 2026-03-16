#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../../domain/events/epollEvents.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
#include "../../infrastructure/network/serverSocket.hpp"
#include "epollManager.hpp"

#include "../../infrastructure/network/testHttpResponse.hpp"
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <vector>

class ConnectionManager
{
	private:
		EpollManager&				 _epollManager;
		std::vector< ClientSocket* > _clients;

		void disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents);
		~ConnectionManager();

		void acceptNewClient(ServerSocket& serverSocket);
		void handleClientRead(int fd);
		void handleClientWrite(int fd);
};

#endif
