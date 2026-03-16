#ifndef ERRORPAGEGENERATOR_HPP
#define ERRORPAGEGENERATOR_HPP

#include "statusCodeResponse.hpp"
#include <string>

class ErrorPageGenerator
{
	public:
		// Recebe o código e uma referência para o helper de mensagens
		static std::string generate(HttpStatusCode code, StatusCodeResponse& helper)
		{
			std::string fullStatus = helper.statusReturn(code);
			// std::string msg = helper.findMsg(code);
			//(void)msg;
			return "<html>\r\n"
				   "<head><title>" +
				fullStatus +
				"</title></head>\r\n"
				"<body>\r\n"
				"<center><h1>" +
				fullStatus +
				"</h1></center>\r\n"
				"<hr><center>42_Webserver/1.0</center>\r\n"
				"</body>\r\n"
				"</html>\r\n";
		}
};

#endif
