#ifndef HTTPREQUESTFRAMER_HPP
#define HTTPREQUESTFRAMER_HPP

#include <cstddef>
#include <string>

class HttpRequestFramer
{
	public:
		static bool popCompleteRequestFromBuffer(std::string& buffer, std::string& rawRequest);
};

#endif
