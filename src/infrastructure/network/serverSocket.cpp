#include "serverSocket.hpp"

ServerSocket::ServerSocket() : _fd(-1)
{
	_saddr.sin_family	   = AF_INET;
	_saddr.sin_port		   = 0;
	_saddr.sin_addr.s_addr = INADDR_ANY;
}

ServerSocket::ServerSocket(const Port& port, const IpAddr& ipAddr) : _fd(socket(AF_INET, SOCK_STREAM, 0))
{
	if (!_fd.isValid())
		return;

	if (!_fd.setNonBlocking())
	{
		_fd.invalidate();
		return;
	}

	int opt = 1;
	if (setsockopt(_fd.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		_fd.invalidate();
		return;
	}

	configAddr(port, ipAddr);
}

ServerSocket::~ServerSocket() {}

void ServerSocket::configAddr(const Port& port, const IpAddr& ipAddr)
{
	_saddr.sin_family = AF_INET;
	_saddr.sin_port	  = htons(port.getValue());

	std::string ipStr = ipAddr.getValue();
	if (ipStr == "0.0.0.0" || ipStr.empty())
	{
		_saddr.sin_addr.s_addr = INADDR_ANY;
		return;
	}

	in_addr_t addr = inet_addr(ipStr.c_str());
	if (addr == INADDR_NONE)
	{
		_fd.invalidate();
		return;
	}

	_saddr.sin_addr.s_addr = addr;
}

bool ServerSocket::setBind()
{
	if (bind(_fd.get(), (sockaddr*)&_saddr, sizeof(_saddr)) < 0)
		return (false);
	return (true);
}

int ServerSocket::setAccept() { return (accept(_fd.get(), NULL, NULL)); }

bool ServerSocket::setListen(int backlog)
{
	if (listen(_fd.get(), backlog) < 0)
		return (false);
	return (true);
}

int ServerSocket::getPollFd() const { return (_fd.get()); }

bool ServerSocket::isValid() const { return (_fd.isValid()); }
