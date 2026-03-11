#include "HttpRequestParser.hpp"
#include <sstream>

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::parse(const std::string& raw)
{
	HttpRequest req;

	size_t		pos		= raw.find("\r\n\r\n");
	size_t		eol_len = 4;
	std::string head;

	if (pos == std::string::npos)
	{
		pos		= raw.find("\n\n");
		eol_len = 2;
	}

	if (pos != std::string::npos)
	{
		head			  = raw.substr(0, pos);
		size_t body_start = pos + eol_len;
		if (body_start < raw.size())
			req.setBody(raw.substr(body_start));
	}

	size_t line_end = head.find("\r\n");
	if (line_end == std::string::npos)
		line_end = head.find('\n');

	std::string request_line;
	if (line_end != std::string::npos)
		request_line = head.substr(0, line_end);
	else
		request_line = head;

	parseRequestLine(request_line, req);

	size_t headers_start = 0;
	if (line_end != std::string::npos)
	{
		if (line_end + 1 < head.size() && head[line_end] == '\r' && head[line_end + 1] == '\n')
			headers_start = line_end + 2;
		else
			headers_start = line_end + 1;
	}
	else
	{
		headers_start = request_line.size();
	}

	std::string rest = (headers_start < head.size()) ? head.substr(headers_start) : std::string();
	parseHeaders(rest, req);

	return req;
}

void HttpRequestParser::parseRequestLine(const std::string& line, HttpRequest& req)
{
	std::istringstream rl(line);
	std::string		   method, uri, version;
	rl >> method >> uri >> version;
	req.setMethod(method);
	req.setUri(uri);
	req.setVersion(version);
}

void HttpRequestParser::parseHeaders(const std::string& headerBlock, HttpRequest& req)
{
	size_t start = 0;
	while (start < headerBlock.size())
	{
		size_t eol			= headerBlock.find("\r\n", start);
		size_t line_eol_len = 2;

		if (eol == std::string::npos)
		{
			eol			 = headerBlock.find('\n', start);
			line_eol_len = 1;
		}
		if (eol == std::string::npos)
			eol = headerBlock.size();

		std::string line = headerBlock.substr(start, eol - start);
		if (line.empty())
			break;

		size_t colon = line.find(':');
		if (colon != std::string::npos)
		{
			std::string name  = line.substr(0, colon);
			std::string value = line.substr(colon + 1);
			value			  = trim(value);
			req.setHeader(name, value);
		}
		start = eol + line_eol_len;
	}
}

std::string HttpRequestParser::trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}
