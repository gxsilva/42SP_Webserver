#ifndef HTTPRESPONSEBUILDERS_HPP
#define HTTPRESPONSEBUILDERS_HPP

#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/server/LocationBlock.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "../../domain/services/ErrPage.hpp"
#include "../../domain/services/statusCodeResponse.hpp"
#include "../../domain/value_objects/HttpStatusCode.hpp"
#include "../../infrastructure/io/config/FileReader.hpp"

#include <string>

inline std::string joinPath(const std::string& base, const std::string& suffix)
{
	if (base.empty())
		return suffix;
	if (suffix.empty())
		return base;
	if (base[base.size() - 1] == '/' && suffix[0] == '/')
		return base + suffix.substr(1);
	if (base[base.size() - 1] != '/' && suffix[0] != '/')
		return base + "/" + suffix;
	return base + suffix;
}

inline std::string resolveConfiguredErrorPagePath(HttpStatusCode code,
	const ServerBlock& serverConfig,
	const LocationBlock* location)
{
	std::string configured;
	int statusCode = static_cast<int>(code);

	if (location != NULL)
	{
		std::map<int, std::string>::const_iterator it = location->errorPages.find(statusCode);
		if (it != location->errorPages.end())
			configured = it->second;
	}

	if (configured.empty())
	{
		std::map<int, std::string>::const_iterator it = serverConfig.errorPages.find(statusCode);
		if (it != serverConfig.errorPages.end())
			configured = it->second;
	}

	if (configured.empty())
		return configured;

	if (configured.find("./") == 0 || configured.find("../") == 0)
		return configured;

	std::string root = serverConfig.root.empty() ? "./www" : serverConfig.root;
	if (location != NULL && !location->root.empty())
		root = location->root;

	return joinPath(root, configured);
}

inline HttpResponse buildPlainTextRedirectResponse(int statusCode, const std::string& target)
{
	HttpResponse response;
	response.setStatusCode(statusCode);
	response.setHeader("Location", target);
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Connection", "close");
	response.setBody("Redirecting to " + target + "\n");
	return (response);
}

inline HttpResponse buildHtmlErrorResponse(HttpStatusCode code,
	const ServerBlock& serverConfig,
	const LocationBlock* location)
{
	StatusCodeResponse statusHelper;
	HttpResponse response;
	std::string body;
	std::string customPagePath = resolveConfiguredErrorPagePath(code, serverConfig, location);

	if (!customPagePath.empty() && FileReader::readFile(customPagePath, body))
	{
		if (body.empty())
			body = ErrorPageGenerator::generate(code, statusHelper);
	}
	else
		body = ErrorPageGenerator::generate(code, statusHelper);

	response.setStatusCode(static_cast<int>(code));
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Connection", "close");
	response.setBody(body);
	return (response);
}

inline HttpResponse buildHtmlErrorResponse(HttpStatusCode code)
{
	ServerBlock emptyConfig;
	emptyConfig.root = "./www";
	return buildHtmlErrorResponse(code, emptyConfig, NULL);
}

#endif
