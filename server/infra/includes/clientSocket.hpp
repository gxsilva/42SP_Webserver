#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "fileDescriptor.hpp"

class   ClientSocket
{
    private:
        FileDescriptor _fd;

    public:
        ClientSocket();
        explicit ClientSocket(int fd);
        ~ClientSocket();

        ssize_t receiveData(char* buffer, size_t size);
        ssize_t sendData(const char* data, size_t size);
        void    invalidate();
        
        int     getPollFd() const;
        bool    isValid() const;

};

#endif
