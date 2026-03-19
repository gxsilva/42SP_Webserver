#include "CgiRouteResolver.hpp"

bool CgiRouteResolver::isCgiRequest(const std::string& uri)
{
	std::string			   path		= uri;
	std::string::size_type queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);

	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return true;
	if (path.size() >= 4 && path.substr(path.size() - 4) == ".php")
		return true;
	return false;
}

CgiRouteConfig CgiRouteResolver::buildConfig(const HttpRequest& request, const ServerBlock& serverConfig)
{
	CgiRouteConfig		   config;
	std::string			   uriPath	= request.getUri();
	std::string::size_type queryPos = uriPath.find('?');
	if (queryPos != std::string::npos)
		uriPath = uriPath.substr(0, queryPos);

	std::string root = serverConfig.root.empty() ? "./www" : serverConfig.root;
	if (!root.empty() && root[root.size() - 1] == '/' && !uriPath.empty() && uriPath[0] == '/')
		config.scriptPath = root + uriPath.substr(1);
	else
		config.scriptPath = root + uriPath;
	config.interpreterPath = getInterpreter(request.getUri());
	config.serverName	   = serverConfig.serverName.empty() ? "localhost" : serverConfig.serverName;
	config.serverPort	   = serverConfig.port;
	return config;
}

std::string CgiRouteResolver::getInterpreter(const std::string& uri)
{
	std::string			   path		= uri;
	std::string::size_type queryPos = path.find('?');
	if (queryPos != std::string::npos)
		path = path.substr(0, queryPos);

	if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
		return "/usr/bin/python3";
	return "/usr/bin/php-cgi";
}
