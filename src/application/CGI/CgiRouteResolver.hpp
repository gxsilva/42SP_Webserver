#ifndef CGIROUTERESOLVER_HPP
#define CGIROUTERESOLVER_HPP

#include "CgiHandler.hpp"
#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include <string>

class CgiRouteResolver
{
	public:
		static bool isCgiRequest(const std::string& uri);
		static CgiRouteConfig buildConfig(const HttpRequest& request, const ServerBlock& serverConfig);

	private:
		static std::string getInterpreter(const std::string& uri);
};

#endif
