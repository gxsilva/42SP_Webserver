#include "PostRequestHandler.hpp"

#include "../../infrastructure/io/DirectoryReader.hpp"
#include "HttpResponseBuilders.hpp"

#include <cctype>
#include <fcntl.h>
#include <unistd.h>

PostRequestHandler::PostRequestHandler() : _hasServerConfig(false) {}

PostRequestHandler::~PostRequestHandler() {}

void PostRequestHandler::configure(const ServerBlock& serverConfig)
{
	_serverConfig	 = serverConfig;
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

	const LocationBlock* best	 = NULL;
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
			best	= &candidate;
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

bool PostRequestHandler::isSafeUploadFileName(const std::string& fileName) const
{
	if (fileName.empty())
		return (false);

	if (fileName == "." || fileName == "..")
		return (false);

	if (fileName.find('/') != std::string::npos || fileName.find('\\') != std::string::npos)
		return (false);

	if (fileName.find("..") != std::string::npos)
		return (false);

	for (size_t i = 0; i < fileName.size(); ++i)
	{
		unsigned char c = static_cast< unsigned char >(fileName[i]);
		if (std::isalnum(c) || c == '.' || c == '_' || c == '-')
			continue;
		return (false);
	}

	return (true);
}

bool PostRequestHandler::extractUploadFileName(const std::string& uri, std::string& uploadFileName) const
{
	uploadFileName.clear();

	std::string::size_type queryStart = uri.find('?');
	if (queryStart == std::string::npos)
		return (true);

	std::string::size_type fragStart = uri.find('#', queryStart + 1);
	std::string			   query =
		uri.substr(queryStart + 1, (fragStart == std::string::npos) ? std::string::npos : fragStart - queryStart - 1);

	std::string::size_type pos = 0;
	while (pos <= query.size())
	{
		std::string::size_type amp	= query.find('&', pos);
		std::string			   pair = query.substr(pos, (amp == std::string::npos) ? std::string::npos : amp - pos);

		std::string::size_type eq	 = pair.find('=');
		std::string			   key	 = pair.substr(0, eq);
		std::string			   value = (eq == std::string::npos) ? "" : pair.substr(eq + 1);

		if (key == "filename")
		{
			if (!isSafeUploadFileName(value))
				return (false);
			uploadFileName = value;
			return (true);
		}

		if (amp == std::string::npos)
			break;
		pos = amp + 1;
	}

	return (true);
}

std::string PostRequestHandler::buildUploadTarget(const std::string& filePath, const std::string& uploadFileName) const
{
	if (!DirectoryReader::isDirectory(filePath))
		return (filePath);

	std::string target = filePath;
	if (!target.empty() && target[target.size() - 1] != '/')
		target += "/";
	if (uploadFileName.empty())
		target += "post_upload.txt";
	else
		target += uploadFileName;
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

HttpResponse PostRequestHandler::buildErrorResponse(HttpStatusCode code, const LocationBlock* location) const
{
	if (_hasServerConfig)
		return buildHtmlErrorResponse(code, _serverConfig, location);
	return buildHtmlErrorResponse(code);
}

HttpResponse PostRequestHandler::writeRequestBody(const HttpRequest& request, const std::string& targetPath,
												  const LocationBlock* location) const
{
	std::string dirPath = parentDirectory(targetPath);
	if (!DirectoryReader::isDirectory(dirPath))
		return (buildErrorResponse(NOT_FOUND, location));

	if (access(dirPath.c_str(), W_OK) != 0)
		return (buildErrorResponse(FORBIDDEN, location));

	int fd = open(targetPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (buildErrorResponse(INTERNAL_SERVER_ERROR, location));

	const std::string& body			= request.getBody();
	ssize_t			   totalWritten = 0;

	while (totalWritten < static_cast< ssize_t >(body.size()))
	{
		ssize_t written = write(fd, body.data() + totalWritten, body.size() - totalWritten);
		if (written <= 0)
		{
			close(fd);
			return (buildErrorResponse(INTERNAL_SERVER_ERROR, location));
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
		return (buildErrorResponse(FORBIDDEN, NULL));

	const LocationBlock* location = findBestLocation(uriPath);
	if (location != NULL && !location->redirectUri.empty())
		return (buildPlainTextRedirectResponse(location->redirectCode, location->redirectUri));

	std::string uploadFileName;
	if (!extractUploadFileName(request.getUri(), uploadFileName))
		return (buildErrorResponse(BAD_REQUEST, location));

	if (!isMethodAllowed(location))
		return (buildErrorResponse(METHOD_NOT_ALLOWED, location));

	if (_hasServerConfig && _serverConfig.clientMaxBodySize > 0 &&
		request.getBody().size() > _serverConfig.clientMaxBodySize)
		return (buildErrorResponse(CONTENT_TOO_LARGE, location));

	std::string root	   = resolveRoot(location);
	std::string filePath   = resolveFilePath(root, uriPath);
	std::string targetPath = buildUploadTarget(filePath, uploadFileName);

	return (writeRequestBody(request, targetPath, location));
}
