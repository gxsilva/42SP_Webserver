#include "HttpMethodOrchestrator.hpp"

#include "../../domain/services/ErrPage.hpp"
#include "../../domain/services/statusCodeResponse.hpp"

HttpMethodOrchestrator::HttpMethodOrchestrator() {}

HttpMethodOrchestrator::~HttpMethodOrchestrator() {}

void HttpMethodOrchestrator::configure(const ServerBlock& serverConfig)
{
	_getHandler.configure(serverConfig);
	_postHandler.configure(serverConfig);
	_deleteHandler.configure(serverConfig);
}

HttpResponse HttpMethodOrchestrator::buildMethodNotAllowedResponse() const
{
	StatusCodeResponse statusHelper;
	HttpResponse	   response;

	response.setStatusCode(405);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Allow", "GET, POST, DELETE");
	response.setHeader("Connection", "close");
	response.setBody(ErrorPageGenerator::generate(METHOD_NOT_ALLOWED, statusHelper));
	return (response);
}

HttpResponse HttpMethodOrchestrator::handle(const HttpRequest& request)
{
	const std::string& method = request.getMethod();

	if (method == "GET")
		return (_getHandler.handle(request));

	if (method == "POST")
		return (_postHandler.handle(request));

	if (method == "DELETE")
		return (_deleteHandler.handle(request));

	return (buildMethodNotAllowedResponse());
}
