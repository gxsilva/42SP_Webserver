#ifndef IPADDR_HPP
#define IPADDR_HPP

#include <string>
#include <stdexcept>
#include <sstream>

class	IpAddr
{
	private:
		std::string	_ip;
		
		bool	isValidIp(const std::string& ip) const;

	public:
		IpAddr();
		explicit IpAddr(const std::string& ip);
		~IpAddr();
		
		std::string	getValue() const;
};

#endif