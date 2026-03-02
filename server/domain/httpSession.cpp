#include "includes/httpSession.hpp"

HttpSession::HttpSession(ClientSocket* socket)
: _socket(socket),
  _buffer(),
  _state(READING)
{}

HttpSession::~HttpSession() {}

ClientSocket* HttpSession::getSocket() const
{
    return (_socket);
}

HttpSession::connectionState HttpSession::getState() const
{
    return (_state);
}

void HttpSession::setState(connectionState state)
{
    _state = state;
}

void HttpSession::appendToBuffer(const char* data, ssize_t size)
{
    if (data && size > 0)
        _buffer.append(data, size);
}

const std::string& HttpSession::getBuffer() const
{
    return (_buffer);
}

bool HttpSession::isReadComplete() const
{
    return (_buffer.find("\r\n\r\n") != std::string::npos);
}