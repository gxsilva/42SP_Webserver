#ifndef GETREQUESTHANDLER_HPP
#define GETREQUESTHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "../../domain/services/DirectoryLister.hpp"

#include <string>
#include <vector>

class GetRequestHandler
{
	public:
		GetRequestHandler();
		~GetRequestHandler();

		void		 configure(const ServerBlock& serverConfig);
		HttpResponse handle(const HttpRequest& request);

	private:
		ServerBlock				 _serverConfig;
		bool					 _hasServerConfig;
		DirectoryLister			 _directoryLister;

		std::string	 stripUriQuery(const std::string& uri) const;
		const LocationBlock* findBestLocation(const std::string& uriPath) const;
		std::string	 resolveFilePath(const std::string& root, const std::string& uriPath) const;
		std::string	 resolveRoot(const LocationBlock* location) const;
		std::vector<std::string> resolveIndexFiles() const;
		bool		 resolveAutoIndex(const LocationBlock* location) const;
		HttpResponse serveFile(const std::string& filePath);
		HttpResponse serveDirectory(const std::string& dirPath,
			const std::string& uri,
			const std::vector<std::string>& indexFiles,
			bool autoIndex);
		bool		 isTextFile(const std::string& mimeType) const;

		GetRequestHandler(const GetRequestHandler&);
		GetRequestHandler& operator=(const GetRequestHandler&);
};

#endif
