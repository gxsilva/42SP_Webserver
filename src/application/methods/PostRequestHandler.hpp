#ifndef POSTREQUESTHANDLER_HPP
#define POSTREQUESTHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"

#include <string>

class PostRequestHandler
{
	public:
		PostRequestHandler();
		~PostRequestHandler();

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
		std::string			 buildUploadTarget(const std::string& filePath) const;
		bool				 isMethodAllowed(const LocationBlock* location) const;
		HttpResponse		 buildErrorResponse(HttpStatusCode code, const LocationBlock* location) const;
		HttpResponse		 writeRequestBody(const HttpRequest& request,
			const std::string& targetPath,
			const LocationBlock* location) const;
		HttpResponse		 buildCreatedResponse(const std::string& targetPath) const;

		PostRequestHandler(const PostRequestHandler&);
		PostRequestHandler& operator=(const PostRequestHandler&);
};

#endif
