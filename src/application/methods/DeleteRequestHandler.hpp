#ifndef DELETEREQUESTHANDLER_HPP
#define DELETEREQUESTHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "../../domain/value_objects/HttpStatusCode.hpp"

#include <string>

class DeleteRequestHandler
{
	public:
		DeleteRequestHandler();
		~DeleteRequestHandler();

		void		 configure(const ServerBlock& serverConfig);
		HttpResponse handle(const HttpRequest& request);

	private:
		ServerBlock _serverConfig;
		bool		_hasServerConfig;

		std::string			 stripUriQuery(const std::string& uri) const;
		const LocationBlock* findBestLocation(const std::string& uriPath) const;
		std::string			 resolveRoot(const LocationBlock* location) const;
		std::string			 resolveFilePath(const std::string& root, const std::string& uriPath) const;
		std::string			 parentDirectory(const std::string& path) const;
		bool				 isMethodAllowed(const LocationBlock* location) const;
		HttpResponse		 buildNoContentResponse() const;
		HttpResponse		 buildErrorResponse(HttpStatusCode code) const;

		DeleteRequestHandler(const DeleteRequestHandler&);
		DeleteRequestHandler& operator=(const DeleteRequestHandler&);
};

#endif
