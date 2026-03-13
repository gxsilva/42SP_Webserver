#include "../value_objects/CgiResponse.hpp"

CgiResponse::CgiResponse() : _statusCode(200) {}

CgiResponse::~CgiResponse() {}

bool CgiResponse::parse(const std::string& rawOutput)
{
	_headers.clear();
	_body.clear();
	_statusCode = 200;

	std::string::size_type separator	 = rawOutput.find("\r\n\r\n");
	std::string::size_type separationLen = 4;

	if (separator == std::string::npos)
	{
		separator	  = rawOutput.find("\n\n");
		separationLen = 2;
	}
	if (separator == std::string::npos)
		return (false);

	std::string headerSection = rawOutput.substr(0, separator);
	_body					  = rawOutput.substr(separator + separationLen);
	std::istringstream stream(headerSection);
	std::string		   line;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::string::size_type colon = line.find(':');
		if (colon == std::string::npos)
			continue;
		std::string			   key	 = line.substr(0, colon);
		std::string			   value = line.substr(colon + 1);
		std::string::size_type start = value.find_first_not_of(' ');
		if (start != std::string::npos)
			value = value.substr(start);
		_headers.push_back(std::make_pair(key, value));
	}
	parseStatusFromHeaders();
	return (true);
}

void CgiResponse::parseStatusFromHeaders()
{
	for (std::size_t i = 0; i < _headers.size(); ++i)
	{
		if (_headers[i].first == "STATUS")
		{
			int code	= std::atoi(_headers[i].second.c_str());
			_statusCode = (code >= 100 && code <= 599) ? code : 500;
			_headers.erase(_headers.begin() + static_cast<long>(i));
			return;
		}
	}
	_statusCode = 200;
}

const std::string& CgiResponse::getBody() const { return (_body); }

std::string CgiResponse::getHeader(const std::string& key) const
{
	std::string normalizedKey = toUpper(key);
	for (std::size_t i = 0; i < _headers.size(); ++i)
	{
		if (_headers[i].first == normalizedKey)
			return (_headers[i].second);
	}
	return ("");
}

int CgiResponse::getStatusCode() const { return (_statusCode); }

void CgiResponse::setStatusCode(int code)
{
	if (code >= 100 && code <= 599)
		_statusCode = code;
	else
		_statusCode = 500;
}

const std::vector<std::pair<std::string, std::string>>& CgiResponse::getHeaders() const
{
	return (_headers);
}

static std::string toUpper(const std::string& str)
{
	std::string result = str;
	for (std::string::size_type i = 0; i < result.size(); ++i)
		result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
	return (result);
}
