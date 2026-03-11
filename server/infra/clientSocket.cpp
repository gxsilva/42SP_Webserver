#include "includes/clientSocket.hpp"

ClientSocket::ClientSocket() 
: _fd(-1)
{}

ClientSocket::ClientSocket(int fd) : _fd(fd)
{
    if (_fd.isValid())
        _fd.setNonBlocking();
}

ClientSocket::~ClientSocket()
{
    invalidate();
}

void    ClientSocket::invalidate()
{
    _fd.invalidate();
}

bool    ClientSocket::isValid() const 
{
    return (_fd.isValid());
}

int     ClientSocket::getPollFd() const
{
    return (_fd.get());
}

ssize_t ClientSocket::receiveData(char* buffer, size_t size)
{
    if (!_fd.isValid())
        return (-1);

    return (recv(_fd.get(), buffer, size, 0));
}

ssize_t ClientSocket::sendData(const char* data, size_t size)
{
    if (!_fd.isValid())
        return (-1);

    return (send(_fd.get(), data, size, 0));
}
