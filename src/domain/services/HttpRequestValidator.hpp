#ifndef HTTPREQUESTVALIDATOR_HPP
#define HTTPREQUESTVALIDATOR_HPP

#include "../entities/HttpRequest.hpp"
#include "../errors/ValidationError.hpp"
#include <string>

class HttpRequestValidator
{
	private:
		static const size_t MAX_CONTENT_LENGTH = 1048576; // 1MB

	public:
		HttpRequestValidator();
		~HttpRequestValidator();

		std::string validate(const HttpRequest& req) const;

	private:
		std::string toUpperCase(const std::string& str) const;
		bool		isValidMethod(const std::string& method) const;
		bool		isValidVersion(const std::string& version) const;
		bool		isValidUri(const std::string& uri) const;
		bool		hasRequiredHeaders(const HttpRequest& req) const;
		bool		isValidContentLength(const HttpRequest& req) const;
		bool		hasContentLengthHeader(const HttpRequest& req) const;
		bool		isPostWithoutContentLength(const HttpRequest& req) const;
		bool		isBodyLengthValid(const HttpRequest& req) const;
		long		getContentLengthValue(const HttpRequest& req) const;
};

#endif // HTTPREQUESTVALIDATOR_HPP
