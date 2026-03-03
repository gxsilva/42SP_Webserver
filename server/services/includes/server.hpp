#ifndef  SERVER_HPP
#define  SERVER_HPP

#include <iostream>
#include <string>
#include <stdio.h>
#include <cstddef>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "../../domain/includes/port.hpp"
#include "../../domain/includes/ipAddr.hpp"
#include "../../domain/includes/epollEvents.hpp"
#include "../../infra/includes/fileDescriptor.hpp"
#include "../../infra/includes/serverSocket.hpp"
#include "../../infra/includes/clientSocket.hpp"
#include "connectionManager.hpp"
#include "epollManager.hpp"

class  Server
{
    private:
        ServerSocket			_serverSocket;
        EpollManager*			_epollManager;
        ConnectionManager*		_connectionManager;
        bool					_isValid;
        void    processEvents(int count);
        void    handleEventByIndex(int index);
        bool    isServerSocket(int fd) const;

        bool	handleError(const std::string& msg);

    public:
        Server();
        Server(const Port& port, const IpAddr& ipAddr);
        ~Server();

        bool	isValid() const;
        void	run();
};

#endif

//ncdu