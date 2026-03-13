#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "../services/statusCodeResponse.hpp"
#include <map>
#include <string>
#include <sstream>
#include <cctype>

class HttpResponse
{
	public:
		HttpResponse();
		~HttpResponse();

		void setStatusCode(int code);
		void setHeader(const std::string& name, const std::string& value);
		void setBody(const std::string& body);

		int												getStatusCode() const;
		std::string										getHeader(const std::string& name) const;
		const std::map<std::string, std::string>&		getHeaders() const;
		const std::string&								getBody() const;

		std::string serialize() const;

	private:
		int									_statusCode;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		std::string normalizeHeaderName(const std::string& name) const;
};

#endif // HTTPRESPONSE_HPP
