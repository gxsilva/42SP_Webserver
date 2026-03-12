#include "../value_objects/CgiEnvironment.hpp"

CgiEnvironment::CgiEnvironment(const HttpRequest& request, const std::string& scriptPath, const std::string& serverName, int Port)
{}

CgiEnvironment::~CgiEnvironment() {}

void CgiEnvironment::addVariable(const std::string& key, const std::string& value)
{
    _var.push_back(key + "=" + value);
}
