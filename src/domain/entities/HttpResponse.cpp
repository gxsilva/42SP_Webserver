#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _statusCode(200) {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(int code) { _statusCode = code; }

void HttpResponse::setHeader(const std::string& name, const std::string& value)
{
	_headers[normalizeHeaderName(name)] = value;
}

void HttpResponse::setBody(const std::string& body) { _body = body; }

int HttpResponse::getStatusCode() const { return (_statusCode); }

std::string HttpResponse::getHeader(const std::string& name) const
{
	std::map< std::string, std::string >::const_iterator it = _headers.find(normalizeHeaderName(name));
	if (it != _headers.end())
		return (it->second);
	return ("");
}

const std::map< std::string, std::string >& HttpResponse::getHeaders() const { return (_headers); }

const std::string& HttpResponse::getBody() const { return (_body); }

std::string HttpResponse::serialize() const
{
	StatusCodeResponse statusHelper;
	std::ostringstream oss;

	oss << "HTTP/1.1 " << statusHelper.statusReturn(static_cast< HttpStatusCode >(_statusCode)) << "\r\n";

	std::map< std::string, std::string >::const_iterator it;
	bool												 hasContentLength = false;
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		oss << it->first << ": " << it->second << "\r\n";
		if (it->first == "CONTENT-LENGTH")
			hasContentLength = true;
	}

	if (!hasContentLength)
	{
		std::ostringstream lenStream;
		lenStream << _body.size();
		oss << "Content-Length: " << lenStream.str() << "\r\n";
	}

	oss << "\r\n";
	oss << _body;

	return (oss.str());
}

std::string HttpResponse::normalizeHeaderName(const std::string& name) const
{
	std::string normalized = name;
	for (size_t i = 0; i < normalized.length(); ++i)
		normalized[i] = static_cast< char >(std::toupper(static_cast< unsigned char >(normalized[i])));
	return (normalized);
}
