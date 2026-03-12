#ifndef CGIENVIRONMENT_HPP
#define CGIENVIRONMENT_HPP

#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "../entities/HttpRequest.hpp"

class CgiEnvironment
{
    public:
        CgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port);
        ~CgiEnvironment();

    private:
        std::vector<std::string>    _var;

        CgiEnvironment();
        void addVariable(const std::string& key, const std::string& value);
       
};

#endif