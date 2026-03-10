#ifndef STATUSCODERESPONSE_HPP
#define STATUSCODERESPONSE_HPP

#include "../value_objects/HttpStatusCode.hpp"
#include <string>
#include <sstream>
#include <map>

class StatusCodeResponse
{
    private:
        std::map<HttpStatusCode, std::string>   messages;
    
    public:
        StatusCodeResponse();
        ~StatusCodeResponse();

        std::string findMsg(HttpStatusCode code);
        std::string statusReturn(HttpStatusCode code);
};

#endif