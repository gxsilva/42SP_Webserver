#include "ipAddr.hpp"

IpAddr::IpAddr()
: _ip("0.0.0.0")
{}

IpAddr::IpAddr(const std::string& ip)
{
	if (!isValidIp(ip))
		throw std::invalid_argument("Invalid IP address format");
	_ip = ip;
}

IpAddr::~IpAddr() {}

bool	IpAddr::isValidIp(const std::string& ip) const
{
	if (ip.empty())
		return (false);
	return (true);
}

std::string	IpAddr::getValue() const
{
	return (_ip);
}