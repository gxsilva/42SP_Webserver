#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/value_objects/CgiEnvironment.hpp"
#include "../../domain/value_objects/CgiResponse.hpp"
#include "../../interfaces/port/InitCgiGateway.hpp"
#include <string>

struct CgiRouteConfig
{
		std::string scriptPath;
		std::string interpreterPath;
		std::string serverName;
		int			serverPort;
};

class CgiHandler
{
	public:
		CgiHandler(InitCgiGateway& gateway, const CgiRouteConfig& config);
		~CgiHandler();

		bool startCgi(const HttpRequest& request);

		InitCgiGateway& getGateway();

		HttpResponse buildResponse();

	private:
		CgiHandler();
		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);

		InitCgiGateway& _gateway;
		CgiRouteConfig	_config;
};

#endif
