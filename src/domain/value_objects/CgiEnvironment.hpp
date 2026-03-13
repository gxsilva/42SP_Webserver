#ifndef CGIENVIRONMENT_HPP
#define CGIENVIRONMENT_HPP

#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <sstream>

class HttpRequest;

class CgiEnvironment
{
    public:
        CgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port);
        ~CgiEnvironment();

        char** toEnvArray() const;
        static void freeEnvArray(char** envp);

    private:
        std::vector<std::string>    _var;

        CgiEnvironment();
        CgiEnvironment(const CgiEnvironment&);
        CgiEnvironment& operator=(const CgiEnvironment&);


        void addVariable(const std::string& key, const std::string& value);
        void buildFromRequest(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port);

};

#endif