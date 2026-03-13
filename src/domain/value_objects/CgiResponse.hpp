#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include <cstdlib>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class CgiResponse
{
	public:
		CgiResponse();
		~CgiResponse();

		bool parse(const std::string& rawOutput);

		const std::string& getBody() const;
		std::string		   getHeader(const std::string& key) const;
		int				   getStatusCode() const;
		void			   setStatusCode(int code);
		const std::vector<std::pair<std::string, std::string> >& getHeaders() const;

	private:
		int												 	_statusCode;
		std::vector<std::pair<std::string, std::string> >	_headers;
		std::string										 	_body;

		void parseStatusFromHeaders();
};

#endif
