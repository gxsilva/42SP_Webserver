#include "CgiHandler.hpp"

CgiHandler::CgiHandler(InitCgiGateway& gateway, const CgiRouteConfig& config) : _gateway(gateway), _config(config) {}

CgiHandler::~CgiHandler() {}

bool CgiHandler::startCgi(const HttpRequest& request)
{
	CgiEnvironment env(request, _config.scriptPath, _config.serverName, _config.serverPort);

	return (_gateway.start(_config.scriptPath, _config.interpreterPath, env, request.getBody()));
}

InitCgiGateway& CgiHandler::getGateway() { return (_gateway); }

HttpResponse CgiHandler::buildResponse()
{
	CgiResponse	 cgiResp = _gateway.getResponse();
	HttpResponse response;

	response.setStatusCode(cgiResp.getStatusCode());

	const std::vector< std::pair< std::string, std::string > >& headers = cgiResp.getHeaders();
	for (std::size_t i = 0; i < headers.size(); ++i)
		response.setHeader(headers[i].first, headers[i].second);

	response.setBody(cgiResp.getBody());

	return (response);
}
