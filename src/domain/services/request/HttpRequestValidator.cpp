#include "HttpRequestValidator.hpp"
#include <cstdlib>
#include <sstream>

const size_t HttpRequestValidator::DEFAULT_MAX_CONTENT_LENGTH = 1048576;

HttpRequestValidator::HttpRequestValidator() {}

HttpRequestValidator::~HttpRequestValidator() {}

std::string HttpRequestValidator::toUpperCase(const std::string& str) const
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
	{
		result[i] = std::toupper(result[i]);
	}
	return result;
}

std::string HttpRequestValidator::validate(const HttpRequest& req) const
{
	HttpRequestValidationIssue issue = validateDetailed(req, DEFAULT_MAX_CONTENT_LENGTH);
	if (!issue.hasError())
		return std::string();
	return issue.getMessage();
}

HttpRequestValidationIssue HttpRequestValidator::validateDetailed(const HttpRequest& req, size_t maxContentLength) const
{
	if (!isValidMethod(toUpperCase(req.getMethod())))
	{
		return HttpRequestValidationIssue::badRequest("Invalid HTTP method: " + req.getMethod());
	}

	if (!isValidVersion(toUpperCase(req.getVersion())))
	{
		return HttpRequestValidationIssue::badRequest("Invalid HTTP version: " + req.getVersion() +
													  ". Only HTTP/1.0 and HTTP/1.1 are supported.");
	}

	if (!isValidUri(req.getUri()))
	{
		return HttpRequestValidationIssue::badRequest("Invalid URI: URI cannot be empty.");
	}

	if (!hasRequiredHeaders(req))
	{
		return HttpRequestValidationIssue::badRequest("Missing required header: Host is required.");
	}

	if (isPostWithoutContentLength(req))
	{
		return HttpRequestValidationIssue::badRequest("POST requires Content-Length header");
	}

	if (!isValidContentLength(req))
	{
		return HttpRequestValidationIssue::badRequest("Invalid Content-Length header value.");
	}

	if (isContentTooLarge(req, maxContentLength))
	{
		std::ostringstream oss;
		oss << "Content-Length exceeds configured limit";
		if (maxContentLength > 0)
			oss << " (limit=" << maxContentLength << " bytes)";
		return HttpRequestValidationIssue::contentTooLarge(oss.str());
	}

	if (!isBodyLengthValid(req))
	{
		return HttpRequestValidationIssue::contentTooLarge("Content-Length mismatch with actual body size");
	}

	return HttpRequestValidationIssue::none();
}

bool HttpRequestValidator::isValidMethod(const std::string& method) const
{
	return (method == "GET" || method == "POST" || method == "DELETE");
}

bool HttpRequestValidator::isValidVersion(const std::string& version) const
{
	return (version == "HTTP/1.0" || version == "HTTP/1.1");
}

bool HttpRequestValidator::isValidUri(const std::string& uri) const { return !uri.empty(); }

bool HttpRequestValidator::hasRequiredHeaders(const HttpRequest& req) const
{
	std::map< std::string, std::string >::const_iterator itBegin = req.getHeaders().begin();
	std::map< std::string, std::string >::const_iterator itEnd	 = req.getHeaders().end();

	while (itBegin != itEnd)
	{
		if (toUpperCase(itBegin->first) == "HOST")
		{
			return true;
		}
		++itBegin;
	}
	return false;
}

bool HttpRequestValidator::hasContentLengthHeader(const HttpRequest& req) const
{
	const std::map< std::string, std::string >& headers = req.getHeaders();
	return headers.find("CONTENT-LENGTH") != headers.end();
}

bool HttpRequestValidator::isPostWithoutContentLength(const HttpRequest& req) const
{
	return req.getMethod() == "POST" && !hasContentLengthHeader(req);
}

long HttpRequestValidator::getContentLengthValue(const HttpRequest& req) const
{
	std::map< std::string, std::string >::const_iterator it = req.getHeaders().find("CONTENT-LENGTH");
	if (it == req.getHeaders().end())
		return -1;

	char* endPtr = NULL;
	long  value	 = strtol(it->second.c_str(), &endPtr, 10);

	if (*endPtr != '\0')
		return -2;

	return value;
}

bool HttpRequestValidator::isValidContentLength(const HttpRequest& req) const
{
	if (!hasContentLengthHeader(req))
	{
		return true;
	}

	long contentLength = getContentLengthValue(req);

	if (contentLength < 0)
	{
		return false;
	}

	return true;
}
bool HttpRequestValidator::isContentTooLarge(const HttpRequest& req, size_t maxContentLength) const
{
	if (maxContentLength == 0 || !hasContentLengthHeader(req))
		return false;

	long contentLength = getContentLengthValue(req);
	if (contentLength < 0)
		return false;

	if (static_cast< size_t >(contentLength) > maxContentLength)
	{
		return true;
	}

	return false;
}

bool HttpRequestValidator::isBodyLengthValid(const HttpRequest& req) const
{
	if (!hasContentLengthHeader(req))
	{
		return true;
	}

	long   expectedLength = getContentLengthValue(req);
	size_t actualLength	  = req.getBody().length();

	return actualLength == static_cast< size_t >(expectedLength);
}
