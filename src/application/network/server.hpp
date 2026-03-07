#ifndef  SERVER_HPP
#define  SERVER_HPP

#include <iostream>
#include <string>
#include <stdio.h>
#include <cstddef>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "../../domain/network/port.hpp"
#include "../../domain/network/ipAddr.hpp"
#include "../../domain/events/epollEvents.hpp"
#include "../../infrastructure/network/fileDescriptor.hpp"
#include "../../infrastructure/network/serverSocket.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
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

        Server(const Server&);
        Server& operator=(const Server&);

    public:
        Server();
        Server(const Port& port, const IpAddr& ipAddr);
        ~Server();

        bool	isValid() const;
        void	run();
};

#endif

//ncdu