#include "clientSocket.hpp"

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

void    ClientSocket::appendToWriteBuffer(const char* data, size_t len)
{
    _writeBuffer.append(data, len);
}

bool    ClientSocket::hasDataToSend() const
{
    return (!_writeBuffer.empty());
}

bool    ClientSocket::flushWriteBuffer()
{
    if (_writeBuffer.empty())
        return (true);

    ssize_t sent = sendData(_writeBuffer.c_str(), _writeBuffer.size());
    if (sent < 0)
        return (false);

    _writeBuffer.erase(0, static_cast<size_t>(sent));
    return (_writeBuffer.empty());
}
