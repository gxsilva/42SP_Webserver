#ifndef HTTPRESPONSEBUILDERS_HPP
#define HTTPRESPONSEBUILDERS_HPP

#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/services/ErrPage.hpp"
#include "../../domain/services/statusCodeResponse.hpp"
#include "../../domain/value_objects/HttpStatusCode.hpp"

#include <string>

inline HttpResponse buildPlainTextRedirectResponse(int statusCode, const std::string& target)
{
	HttpResponse response;
	response.setStatusCode(statusCode);
	response.setHeader("Location", target);
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Connection", "close");
	response.setBody("Redirecting to " + target + "\n");
	return (response);
}

inline HttpResponse buildHtmlErrorResponse(HttpStatusCode code)
{
	StatusCodeResponse statusHelper;
	HttpResponse response;
	response.setStatusCode(static_cast<int>(code));
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Connection", "close");
	response.setBody(ErrorPageGenerator::generate(code, statusHelper));
	return (response);
}

#endif
