#include "statusCodeResponse.hpp"

StatusCodeResponse::StatusCodeResponse()
{
	messages[OK]						 = "OK";
	messages[CREATED]					 = "Created";
	messages[ACCEPTED]					 = "Accepted";
	messages[NO_CONTENT]				 = "No content";
	messages[MOVED_PERMANENTLY]			 = "Moved permanently";
	messages[FOUND]						 = "Found";
	messages[NOT_MODIFIED]				 = "Not modified";
	messages[BAD_REQUEST]				 = "Bad request";
	messages[FORBIDDEN]					 = "Forbidden";
	messages[NOT_FOUND]					 = "Not found";
	messages[METHOD_NOT_ALLOWED]		 = "Method not allowed";
	messages[CONTENT_TOO_LARGE]			 = "Content too large";
	messages[URI_TOO_LONG]				 = "URI too long";
	messages[TOO_MANY_REQUESTS]			 = "Too many requests";
	messages[INTERNAL_SERVER_ERROR]		 = "Internal server error";
	messages[NOT_IMPLEMENTED]			 = "Not implemented";
	messages[SERVICE_UNAVAILABLE]		 = "Service unavailable";
	messages[HTTP_VERSION_NOT_SUPPORTED] = "HTTP version not supported";
}

StatusCodeResponse::~StatusCodeResponse() {}

std::string StatusCodeResponse::findMsg(HttpStatusCode code)
{
	std::map<HttpStatusCode, std::string>::iterator it = messages.find(code);
	if (it == messages.end())
		return ("Unknown Status");
	return (it->second);
}

std::string StatusCodeResponse::statusReturn(HttpStatusCode code)
{
	std::stringstream ss;

	ss << static_cast<int>(code) << " " << findMsg(code);
	return (ss.str());
}
