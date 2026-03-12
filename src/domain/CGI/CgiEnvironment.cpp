#include "../value_objects/CgiEnvironment.hpp"

CgiEnvironment::CgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port)
{
    buildFromRequest(request, scriptPath, serverName, Port);
}

CgiEnvironment::~CgiEnvironment() {}

void CgiEnvironment::addVariable(const std::string& key, const std::string& value)
{
    _var.push_back(key + "=" + value);
}

void CgiEnvironment::buildFromRequest(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port)
{
    std::ostringstream  PortConv;
    PortConv << Port;

    addVariable("REQUEST_METHOD", request.getMethod());
    //addVariable("QUERY_STRING", request.getQuerryString("Content-Type"));
    //addVariable("CONTENT_TYPE", request.getHeaders("Content-Length"));
    //addVariable("CONTENT_LENGTH", request.getHeaders());
    addVariable("SCRIPT_NAME", scriptPath);
    //addVariable("PATH_INFO", request.getPathInfo());
    addVariable("SERVER_NAME", serverName);
    addVariable("SERVER_PORT", PortConv.str());
    //addVariable("SERVER_PROTOCOL", "HTTP/1.1");
    //addVariable("GATEWAY_INTERFACE", "CGI/1.1");
    //addVariable("REDIRECT_STATUS", "200");

     std::vector<std::pair<std::string, std::string> > headers = request.getHeaders();
     for (std::size_t i = 0; i < headers.size(); ++i)
     {
        std::string key = "HTTP_";

        for (std::size_t j = 0; j < headers[i].first.size(); ++j)
        {
            char c = headers[i].first[j];
            if (c == '-')
                key += '_';
            else
                key += static_cast<char>(std::toupper(c));
        }
        addVariable(key, headers[i].second);
     }
}

char** CgiEnvironment::toEnvArray() const
{
    char** envp = new char*[_var.size() + 1];
    for (std::size_t i = 0; i < _var.size(); ++i)
    {
        envp[i] = new char[_var[i].size() + 1];
        std::strcpy(envp[i], _var[i].c_str());
    }
    envp[_var.size()] = NULL;
    return (envp);
}

void CgiEnvironment::freeEnvArray(char** envp)
{
    if (!envp)
        return;
    for (std::size_t i = 0; envp[i] != NULL; ++i)
        delete[] envp[i];
    delete[] envp;
}
