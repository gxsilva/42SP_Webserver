#include "GetRequestHandler.hpp"

#include "../../domain/value_objects/MimeType.hpp"
#include "../../infrastructure/io/DirectoryReader.hpp"
#include "../../infrastructure/io/config/FileReader.hpp"
#include "HttpResponseBuilders.hpp"

#include <unistd.h>

GetRequestHandler::GetRequestHandler() : _hasServerConfig(false) {}

GetRequestHandler::~GetRequestHandler() {}

void GetRequestHandler::configure(const ServerBlock& serverConfig)
{
	_serverConfig	 = serverConfig;
	_hasServerConfig = true;
}

std::string GetRequestHandler::stripUriQuery(const std::string& uri) const
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

const LocationBlock* GetRequestHandler::findBestLocation(const std::string& uriPath) const
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

std::string GetRequestHandler::resolveFilePath(const std::string& root, const std::string& uriPath) const
{
	std::string path = root;

	if (!path.empty() && path[path.size() - 1] == '/' && !uriPath.empty() && uriPath[0] == '/')
		path += uriPath.substr(1);
	else
		path += uriPath;
	return (path);
}

std::string GetRequestHandler::resolveRoot(const LocationBlock* location) const
{
	if (location != NULL && !location->root.empty())
		return (location->root);
	if (_hasServerConfig && !_serverConfig.root.empty())
		return (_serverConfig.root);
	return ("./www");
}

std::vector< std::string > GetRequestHandler::resolveIndexFiles() const
{
	if (_hasServerConfig && !_serverConfig.index.empty())
		return (_serverConfig.index);

	std::vector< std::string > defaults;
	defaults.push_back("index.html");
	defaults.push_back("index.htm");
	return defaults;
}

bool GetRequestHandler::resolveAutoIndex(const LocationBlock* location) const
{
	if (location != NULL)
		return (location->autoindex);
	return (false);
}

bool GetRequestHandler::isTextFile(const std::string& mimeType) const
{
	if (mimeType.find("text/") == 0)
		return (true);
	if (mimeType.find("application/javascript") == 0)
		return (true);
	if (mimeType.find("application/json") == 0)
		return (true);
	if (mimeType.find("application/xml") == 0)
		return (true);
	if (mimeType.find("image/svg+xml") == 0)
		return (true);
	return (false);
}

HttpResponse GetRequestHandler::buildErrorResponse(HttpStatusCode code, const LocationBlock* location) const
{
	if (_hasServerConfig)
		return buildHtmlErrorResponse(code, _serverConfig, location);
	return buildHtmlErrorResponse(code);
}

HttpResponse GetRequestHandler::handle(const HttpRequest& request)
{
	std::string uriPath = stripUriQuery(request.getUri());

	if (uriPath.find("..") != std::string::npos)
		return (buildErrorResponse(FORBIDDEN, NULL));

	const LocationBlock* location = findBestLocation(uriPath);
	if (location != NULL && !location->redirectUri.empty())
		return (buildPlainTextRedirectResponse(location->redirectCode, location->redirectUri));

	if (location != NULL && !location->allowedMethods.empty() &&
		location->allowedMethods.find("GET") == location->allowedMethods.end())
		return (buildErrorResponse(METHOD_NOT_ALLOWED, location));

	std::string				   root		  = resolveRoot(location);
	std::vector< std::string > indexFiles = resolveIndexFiles();
	bool					   autoIndex  = resolveAutoIndex(location);
	std::string				   filePath	  = resolveFilePath(root, uriPath);

	if (DirectoryReader::isDirectory(filePath))
		return (serveDirectory(filePath, uriPath, indexFiles, autoIndex, location));

	if (access(filePath.c_str(), F_OK) != 0)
		return (buildErrorResponse(NOT_FOUND, location));

	if (access(filePath.c_str(), R_OK) != 0)
		return (buildErrorResponse(FORBIDDEN, location));

	return (serveFile(filePath, location));
}

HttpResponse GetRequestHandler::serveFile(const std::string& filePath, const LocationBlock* location)
{
	MimeType	mimeType(filePath);
	std::string content;
	bool		readOk;

	if (isTextFile(mimeType.getValue()))
		readOk = FileReader::readFile(filePath, content);
	else
		readOk = FileReader::readBinaryFile(filePath, content);

	if (!readOk)
		return (buildErrorResponse(INTERNAL_SERVER_ERROR, location));

	HttpResponse response;
	response.setStatusCode(static_cast< int >(OK));
	response.setHeader("Content-Type", mimeType.getValue());
	response.setBody(content);
	return (response);
}

HttpResponse GetRequestHandler::serveDirectory(const std::string& dirPath, const std::string& uri,
											   const std::vector< std::string >& indexFiles, bool autoIndex,
											   const LocationBlock* location)
{
	std::string base = dirPath;
	if (!base.empty() && base[base.size() - 1] != '/')
		base += "/";

	for (size_t i = 0; i < indexFiles.size(); ++i)
	{
		std::string indexPath = base + indexFiles[i];
		if (access(indexPath.c_str(), F_OK) == 0 && !DirectoryReader::isDirectory(indexPath))
			return (serveFile(indexPath, location));
	}

	if (!autoIndex)
		return (buildErrorResponse(FORBIDDEN, location));

	std::vector< std::string > entries;
	if (!DirectoryReader::readDirectory(dirPath, entries))
		return (buildErrorResponse(FORBIDDEN, location));

	std::string html = _directoryLister.generateHtml(uri, entries);

	HttpResponse response;
	response.setStatusCode(static_cast< int >(OK));
	response.setHeader("Content-Type", "text/html");
	response.setBody(html);
	return (response);
}
