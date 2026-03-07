#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "fileDescriptor.hpp"
#include "../../domain/network/port.hpp"
#include "../../domain/network/ipAddr.hpp"

class   ServerSocket
{
    private:
        FileDescriptor  _fd;
        sockaddr_in     _saddr;

        void    configAddr(const Port& port, const IpAddr& ipAddr);

    public:
        ServerSocket();
        ServerSocket(const Port& port, const IpAddr& ipAddr);
        ~ServerSocket();
        bool    setBind();
        bool    setListen(int backlog);
        int     setAccept();

        int     getPollFd() const;
        bool    isValid() const;
        
};

#endif
