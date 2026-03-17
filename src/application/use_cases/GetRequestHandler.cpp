#include "GetRequestHandler.hpp"

#include "../../domain/services/ErrPage.hpp"
#include "../../domain/services/statusCodeResponse.hpp"
#include "../../domain/value_objects/MimeType.hpp"
#include "../../infrastructure/io/DirectoryReader.hpp"
#include "../../infrastructure/io/config/FileReader.hpp"

#include <unistd.h>

GetRequestHandler::GetRequestHandler()
	: _rootDirectory("./www"), _autoIndex(false)
{
	_indexFiles.push_back("index.html");
	_indexFiles.push_back("index.htm");
}

GetRequestHandler::~GetRequestHandler() {}

std::string GetRequestHandler::resolveFilePath(const std::string& uri) const
{
	std::string path = _rootDirectory;

	if (!path.empty() && path[path.size() - 1] == '/' && !uri.empty() && uri[0] == '/')
		path += uri.substr(1);
	else
		path += uri;
	return (path);
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

HttpResponse GetRequestHandler::handle(const HttpRequest& request)
{
	std::string filePath = resolveFilePath(request.getUri());

	if (DirectoryReader::isDirectory(filePath))
		return (serveDirectory(filePath, request.getUri()));

	if (access(filePath.c_str(), F_OK) != 0)
		return (buildErrorResponse(NOT_FOUND));

	if (access(filePath.c_str(), R_OK) != 0)
		return (buildErrorResponse(FORBIDDEN));

	return (serveFile(filePath));
}

HttpResponse GetRequestHandler::serveFile(const std::string& filePath)
{
	MimeType	mimeType(filePath);
	std::string content;
	bool		readOk;

	if (isTextFile(mimeType.getValue()))
		readOk = FileReader::readFile(filePath, content);
	else
		readOk = FileReader::readBinaryFile(filePath, content);

	if (!readOk)
		return (buildErrorResponse(INTERNAL_SERVER_ERROR));

	HttpResponse response;
	response.setStatusCode(static_cast<int>(OK));
	response.setHeader("Content-Type", mimeType.getValue());
	response.setBody(content);
	return (response);
}

HttpResponse GetRequestHandler::serveDirectory(const std::string& dirPath,
											   const std::string& uri)
{
	std::string base = dirPath;
	if (!base.empty() && base[base.size() - 1] != '/')
		base += "/";

	for (size_t i = 0; i < _indexFiles.size(); ++i)
	{
		std::string indexPath = base + _indexFiles[i];
		if (access(indexPath.c_str(), F_OK) == 0)
			return (serveFile(indexPath));
	}

	if (!_autoIndex)
		return (buildErrorResponse(NOT_FOUND));

	std::vector<std::string> entries;
	if (!DirectoryReader::readDirectory(dirPath, entries))
		return (buildErrorResponse(FORBIDDEN));

	std::string html = _directoryLister.generateHtml(uri, entries);

	HttpResponse response;
	response.setStatusCode(static_cast<int>(OK));
	response.setHeader("Content-Type", "text/html");
	response.setBody(html);
	return (response);
}

HttpResponse GetRequestHandler::buildErrorResponse(HttpStatusCode code)
{
	StatusCodeResponse statusHelper;
	HttpResponse	   response;
	response.setStatusCode(static_cast<int>(code));
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Connection", "close");
	response.setBody(ErrorPageGenerator::generate(code, statusHelper));
	return (response);
}
