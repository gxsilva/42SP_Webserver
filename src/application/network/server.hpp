#ifndef SERVER_HPP
#define SERVER_HPP

#include "../../domain/entities/server/HttpBlock.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "../../domain/events/epollEvents.hpp"
#include "../../domain/network/ipAddr.hpp"
#include "../../domain/network/port.hpp"
#include "../../infrastructure/logging/Logger.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
#include "../../infrastructure/network/fileDescriptor.hpp"
#include "../../infrastructure/network/serverSocket.hpp"
#include "connectionManager.hpp"
#include "epollManager.hpp"
#include <csignal>
#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

class CgiOrchestrator;

class Server
{
	private:
		std::vector< ServerSocket* > _serverSockets;
		std::vector< ServerBlock >	 _serverConfigs;
		EpollManager*				 _epollManager;
		ConnectionManager*			 _connectionManager;
		CgiOrchestrator*			 _cgiOrchestrator;
		Logger*						 _logger;
		bool						 _isValid;
		void						 processEvents(int count);
		void						 handleEventByIndex(int index);
		ServerSocket*				 findServerSocketByFd(int fd) const;

		bool handleError(const std::string& msg);

		Server(const Server&);
		Server& operator=(const Server&);

	public:
		Server();
		Server(const std::vector< ServerBlock >& serverConfigs, Logger* logger);
		~Server();

		bool isValid() const;
		void run();

		static void requestStop();
		static bool shouldStop();
		void		displayServerStatus(const std::vector< ServerBlock >& serverConfigs) const;
};

#endif

// ncdu
