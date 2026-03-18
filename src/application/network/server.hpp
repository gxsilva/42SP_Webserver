#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "connectionManager.hpp"
#include "epollManager.hpp"

#include "../../domain/events/epollEvents.hpp"
#include "../../domain/network/ipAddr.hpp"
#include "../../domain/network/port.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
#include "../../infrastructure/network/fileDescriptor.hpp"
#include "../../infrastructure/network/serverSocket.hpp"

#include "../../domain/entities/server/HttpBlock.hpp"

class CgiOrchestrator;

class Server
{
	private:
		ServerSocket	   _serverSocket;
		EpollManager*	   _epollManager;
		ConnectionManager* _connectionManager;
		CgiOrchestrator*   _cgiOrchestrator;
		HttpBlock*		   _config;
		bool			   _isValid;
		void			   processEvents(int count);
		void			   handleEventByIndex(int index);
		bool			   isServerSocket(int fd) const;
		static std::string intToString(int value);

		bool handleError(const std::string& msg);

		Server(const Server&);
		Server& operator=(const Server&);

	public:
		Server();
		Server(const Port& port, const IpAddr& ipAddr, HttpBlock* config);
		~Server();

		bool isValid() const;
		void run();

		void displayServerStatus() const;
};

#endif

// ncdu
