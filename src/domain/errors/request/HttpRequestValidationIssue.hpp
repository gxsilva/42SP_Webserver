#ifndef HTTPREQUESTVALIDATIONISSUE_HPP
#define HTTPREQUESTVALIDATIONISSUE_HPP

#include "../../value_objects/HttpStatusCode.hpp"
#include <string>

class HttpRequestValidationIssue
{
	public:
		enum Type
		{
			NONE,
			BAD_REQUEST,
			CONTENT_TOO_LARGE
		};

		HttpRequestValidationIssue() : _type(NONE), _statusCode(OK), _message("") {}

		HttpRequestValidationIssue(Type type, HttpStatusCode statusCode, const std::string& message)
			: _type(type), _statusCode(statusCode), _message(message) {}

		static HttpRequestValidationIssue none()
		{
			return HttpRequestValidationIssue();
		}

		static HttpRequestValidationIssue badRequest(const std::string& message)
		{
			return HttpRequestValidationIssue(BAD_REQUEST, ::BAD_REQUEST, message);
		}

		static HttpRequestValidationIssue contentTooLarge(const std::string& message)
		{
			return HttpRequestValidationIssue(CONTENT_TOO_LARGE, ::CONTENT_TOO_LARGE, message);
		}

		bool hasError() const { return _type != NONE; }
		Type getType() const { return _type; }
		HttpStatusCode getStatusCode() const { return _statusCode; }
		const std::string& getMessage() const { return _message; }

	private:
		Type		 _type;
		HttpStatusCode _statusCode;
		std::string _message;
};

#endif
