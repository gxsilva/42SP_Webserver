#ifndef SERVER_HPP
#define SERVER_HPP

#include "../../domain/events/epollEvents.hpp"
#include "../../domain/network/ipAddr.hpp"
#include "../../domain/network/port.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
#include "../../infrastructure/network/fileDescriptor.hpp"
#include "../../infrastructure/network/serverSocket.hpp"
#include "connectionManager.hpp"
#include "epollManager.hpp"
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
		ServerSocket	   _serverSocket;
		EpollManager*	   _epollManager;
		ConnectionManager* _connectionManager;
		CgiOrchestrator*   _cgiOrchestrator;
		bool			   _isValid;
		void			   processEvents(int count);
		void			   handleEventByIndex(int index);
		bool			   isServerSocket(int fd) const;

		bool handleError(const std::string& msg);

		Server(const Server&);
		Server& operator=(const Server&);

	public:
		Server();
		Server(const Port& port, const IpAddr& ipAddr);
		~Server();

		bool isValid() const;
		void run();
};

#endif

// ncdu
