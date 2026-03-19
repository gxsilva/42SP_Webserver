#include "PostRequestHandler.hpp"

#include "../../infrastructure/io/DirectoryReader.hpp"
#include "HttpResponseBuilders.hpp"

#include <fcntl.h>
#include <unistd.h>

PostRequestHandler::PostRequestHandler()
	: _hasServerConfig(false)
{
}

PostRequestHandler::~PostRequestHandler() {}

void PostRequestHandler::configure(const ServerBlock& serverConfig)
{
	_serverConfig = serverConfig;
	_hasServerConfig = true;
}

std::string PostRequestHandler::stripUriQuery(const std::string& uri) const
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

const LocationBlock* PostRequestHandler::findBestLocation(const std::string& uriPath) const
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

std::string PostRequestHandler::resolveRoot(const LocationBlock* location) const
{
	if (location != NULL && !location->root.empty())
		return (location->root);
	if (_hasServerConfig && !_serverConfig.root.empty())
		return (_serverConfig.root);
	return ("./www");
}

std::string PostRequestHandler::resolveFilePath(const std::string& root, const std::string& uriPath) const
{
	std::string path = root;

	if (!path.empty() && path[path.size() - 1] == '/' && !uriPath.empty() && uriPath[0] == '/')
		path += uriPath.substr(1);
	else
		path += uriPath;
	return (path);
}

std::string PostRequestHandler::parentDirectory(const std::string& path) const
{
	std::string::size_type pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (".");
	if (pos == 0)
		return ("/");
	return (path.substr(0, pos));
}

std::string PostRequestHandler::buildUploadTarget(const std::string& filePath) const
{
	if (!DirectoryReader::isDirectory(filePath))
		return (filePath);

	std::string target = filePath;
	if (!target.empty() && target[target.size() - 1] != '/')
		target += "/";
	target += "post_upload.txt";
	return (target);
}

bool PostRequestHandler::isMethodAllowed(const LocationBlock* location) const
{
	if (location == NULL)
		return true;
	if (location->allowedMethods.empty())
		return true;
	return location->allowedMethods.find("POST") != location->allowedMethods.end();
}

HttpResponse PostRequestHandler::writeRequestBody(const HttpRequest& request,
	const std::string& targetPath) const
{
	std::string dirPath = parentDirectory(targetPath);
	if (!DirectoryReader::isDirectory(dirPath))
		return (buildHtmlErrorResponse(NOT_FOUND));

	if (access(dirPath.c_str(), W_OK) != 0)
		return (buildHtmlErrorResponse(FORBIDDEN));

	int fd = open(targetPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (buildHtmlErrorResponse(INTERNAL_SERVER_ERROR));

	const std::string& body = request.getBody();
	ssize_t			   totalWritten = 0;

	while (totalWritten < static_cast< ssize_t >(body.size()))
	{
		ssize_t written = write(fd, body.data() + totalWritten, body.size() - totalWritten);
		if (written <= 0)
		{
			close(fd);
			return (buildHtmlErrorResponse(INTERNAL_SERVER_ERROR));
		}
		totalWritten += written;
	}

	close(fd);
	return (buildCreatedResponse(targetPath));
}

HttpResponse PostRequestHandler::buildCreatedResponse(const std::string& targetPath) const
{
	HttpResponse response;
	response.setStatusCode(static_cast< int >(CREATED));
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Connection", "close");
	response.setBody("Created: " + targetPath + "\n");
	return (response);
}

HttpResponse PostRequestHandler::handle(const HttpRequest& request)
{
	std::string uriPath = stripUriQuery(request.getUri());

	if (uriPath.find("..") != std::string::npos)
		return (buildHtmlErrorResponse(FORBIDDEN));

	const LocationBlock* location = findBestLocation(uriPath);
	if (location != NULL && !location->redirectUri.empty())
		return (buildPlainTextRedirectResponse(location->redirectCode, location->redirectUri));

	if (!isMethodAllowed(location))
		return (buildHtmlErrorResponse(METHOD_NOT_ALLOWED));

	if (_hasServerConfig && _serverConfig.clientMaxBodySize > 0
		&& request.getBody().size() > _serverConfig.clientMaxBodySize)
		return (buildHtmlErrorResponse(CONTENT_TOO_LARGE));

	std::string root = resolveRoot(location);
	std::string filePath = resolveFilePath(root, uriPath);
	std::string targetPath = buildUploadTarget(filePath);

	return (writeRequestBody(request, targetPath));
}
