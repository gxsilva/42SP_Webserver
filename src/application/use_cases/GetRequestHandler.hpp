#ifndef GETREQUESTHANDLER_HPP
#define GETREQUESTHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../domain/services/DirectoryLister.hpp"
#include "../../domain/value_objects/HttpStatusCode.hpp"

#include <string>
#include <vector>

class GetRequestHandler
{
	public:
		GetRequestHandler();
		~GetRequestHandler();

		HttpResponse handle(const HttpRequest& request);

	private:
		std::string				 _rootDirectory;
		std::vector<std::string> _indexFiles;
		bool					 _autoIndex;
		DirectoryLister			 _directoryLister;

		std::string	 resolveFilePath(const std::string& uri) const;
		HttpResponse serveFile(const std::string& filePath);
		HttpResponse serveDirectory(const std::string& dirPath, const std::string& uri);
		HttpResponse buildErrorResponse(HttpStatusCode code);
		bool		 isTextFile(const std::string& mimeType) const;

		GetRequestHandler(const GetRequestHandler&);
		GetRequestHandler& operator=(const GetRequestHandler&);
};

#endif
