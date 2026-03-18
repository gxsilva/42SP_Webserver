#ifndef STATUSCODERESPONSE_HPP
#define STATUSCODERESPONSE_HPP

#include "../value_objects/HttpStatusCode.hpp"
#include <map>
#include <sstream>
#include <string>

class StatusCodeResponse
{
	private:
		std::map< HttpStatusCode, std::string > messages;

	public:
		StatusCodeResponse();
		~StatusCodeResponse();

		std::string findMsg(HttpStatusCode code);
		std::string statusReturn(HttpStatusCode code);
};

#endif
