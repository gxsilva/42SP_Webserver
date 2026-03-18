#ifndef HTTPMETHODORCHESTRATOR_HPP
#define HTTPMETHODORCHESTRATOR_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "DeleteRequestHandler.hpp"
#include "GetRequestHandler.hpp"
#include "PostRequestHandler.hpp"

class HttpMethodOrchestrator
{
	public:
		HttpMethodOrchestrator();
		~HttpMethodOrchestrator();

		void		 configure(const ServerBlock& serverConfig);
		HttpResponse handle(const HttpRequest& request);

	private:
		GetRequestHandler	_getHandler;
		PostRequestHandler	_postHandler;
		DeleteRequestHandler _deleteHandler;

		HttpResponse buildMethodNotAllowedResponse() const;

		HttpMethodOrchestrator(const HttpMethodOrchestrator&);
		HttpMethodOrchestrator& operator=(const HttpMethodOrchestrator&);
};

#endif
