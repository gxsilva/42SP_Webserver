#include "DeleteRequestHandler.hpp"

#include "../../infrastructure/io/DirectoryReader.hpp"
#include "HttpResponseBuilders.hpp"

#include <cstdio>
#include <unistd.h>

DeleteRequestHandler::DeleteRequestHandler()
	: _hasServerConfig(false)
{
}

DeleteRequestHandler::~DeleteRequestHandler() {}

void DeleteRequestHandler::configure(const ServerBlock& serverConfig)
{
	_serverConfig = serverConfig;
	_hasServerConfig = true;
}

std::string DeleteRequestHandler::stripUriQuery(const std::string& uri) const
{
	std::string::size_type endPos = uri.find('?');
	if (endPos == std::string::npos)
		endPos = uri.find('#');
	if (endPos == std::string::npos)
		return (uri.empty() ? "/" : uri);
	if (endPos == 0)
		return "/";
	return (uri.substr(0, endPos));
}

const LocationBlock* DeleteRequestHandler::findBestLocation(const std::string& uriPath) const
{
	if (!_hasServerConfig)
		return NULL;

	const LocationBlock* best = NULL;
	size_t				 bestLen = 0;

	for (size_t i = 0; i < _serverConfig.locations.size(); ++i)
	{
		const LocationBlock& candidate = _serverConfig.locations[i];
		if (candidate.path.empty())
			continue;

		if (uriPath.compare(0, candidate.path.size(), candidate.path) != 0)
			continue;

		if (candidate.path.size() > bestLen)
		{
			best = &candidate;
			bestLen = candidate.path.size();
		}
	}

	return best;
}

std::string DeleteRequestHandler::resolveRoot(const LocationBlock* location) const
{
	if (location != NULL && !location->root.empty())
		return (location->root);
	if (_hasServerConfig && !_serverConfig.root.empty())
		return (_serverConfig.root);
	return ("./www");
}

std::string DeleteRequestHandler::resolveFilePath(const std::string& root, const std::string& uriPath) const
{
	std::string path = root;

	if (!path.empty() && path[path.size() - 1] == '/' && !uriPath.empty() && uriPath[0] == '/')
		path += uriPath.substr(1);
	else
		path += uriPath;
	return (path);
}

std::string DeleteRequestHandler::parentDirectory(const std::string& path) const
{
	std::string::size_type pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (".");
	if (pos == 0)
		return ("/");
	return (path.substr(0, pos));
}

bool DeleteRequestHandler::isMethodAllowed(const LocationBlock* location) const
{
	if (location == NULL)
		return true;
	if (location->allowedMethods.empty())
		return true;
	return location->allowedMethods.find("DELETE") != location->allowedMethods.end();
}

HttpResponse DeleteRequestHandler::buildErrorResponse(HttpStatusCode code, const LocationBlock* location) const
{
	if (_hasServerConfig)
		return buildHtmlErrorResponse(code, _serverConfig, location);
	return buildHtmlErrorResponse(code);
}

HttpResponse DeleteRequestHandler::buildNoContentResponse() const
{
	HttpResponse response;
	response.setStatusCode(static_cast< int >(NO_CONTENT));
	response.setHeader("Connection", "close");
	response.setBody("");
	return (response);
}

HttpResponse DeleteRequestHandler::handle(const HttpRequest& request)
{
	std::string uriPath = stripUriQuery(request.getUri());

	if (uriPath.find("..") != std::string::npos)
		return (buildErrorResponse(FORBIDDEN, NULL));

	const LocationBlock* location = findBestLocation(uriPath);
	if (location != NULL && !location->redirectUri.empty())
		return (buildPlainTextRedirectResponse(location->redirectCode, location->redirectUri));

	if (!isMethodAllowed(location))
		return (buildErrorResponse(METHOD_NOT_ALLOWED, location));

	std::string root = resolveRoot(location);
	std::string filePath = resolveFilePath(root, uriPath);

	if (access(filePath.c_str(), F_OK) != 0)
		return (buildErrorResponse(NOT_FOUND, location));

	if (DirectoryReader::isDirectory(filePath))
		return (buildErrorResponse(FORBIDDEN, location));

	std::string dirPath = parentDirectory(filePath);
	if (access(dirPath.c_str(), W_OK) != 0)
		return (buildErrorResponse(FORBIDDEN, location));

	if (std::remove(filePath.c_str()) != 0)
		return (buildErrorResponse(INTERNAL_SERVER_ERROR, location));

	return (buildNoContentResponse());
}
