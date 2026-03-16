#ifndef INITCGIGATEWAY_HPP
#define INITCGIGATEWAY_HPP

#include "../../domain/value_objects/CgiEnvironment.hpp"
#include "../../domain/value_objects/CgiResponse.hpp"
#include <string>

enum CgiProcessState
{
	CGI_RUNNING,
	CGI_FINISHED,
	CGI_ERROR,
	CGI_TIMEOUT
};

class InitCgiGateway
{
	public:
		virtual ~InitCgiGateway(){};

		virtual bool start(const std::string& scriptPath, const std::string& interpreterPath,
						   const CgiEnvironment& env, const std::string& requestBody) = 0;

		virtual int getReadFd() const  = 0;
		virtual int getWriteFd() const = 0;

		virtual bool onWriteReady() = 0;
		virtual bool onReadReady()	= 0;

		virtual CgiProcessState checkState() = 0;

		virtual CgiResponse getResponse() = 0;

		virtual void cleanup() = 0;
};

#endif
