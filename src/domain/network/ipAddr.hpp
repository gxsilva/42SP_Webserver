#ifndef IPADDR_HPP
#define IPADDR_HPP

#include <sstream>
#include <stdexcept>
#include <string>

class IpAddr
{
	private:
		std::string _ip;

		bool isValidIp(const std::string& ip) const;

	public:
		IpAddr();
		explicit IpAddr(const std::string& ip);
		~IpAddr();

		std::string getValue() const;
};

#endif
