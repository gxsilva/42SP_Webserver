#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "fileDescriptor.hpp"
#include <string>

class   ClientSocket
{
    private:
        FileDescriptor _fd;
        std::string    _writeBuffer;

    public:
        ClientSocket();
        explicit ClientSocket(int fd);
        ~ClientSocket();

        ssize_t receiveData(char* buffer, size_t size);
        ssize_t sendData(const char* data, size_t size);
        void    invalidate();

        void    appendToWriteBuffer(const char* data, size_t len);
        bool    hasDataToSend() const;
        bool    flushWriteBuffer();
        
        int     getPollFd() const;
        bool    isValid() const;

};

#endif
