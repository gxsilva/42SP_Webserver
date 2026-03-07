#include "server.hpp"

bool    Server::handleError(const std::string& msg)
{
    perror(msg.c_str());
    return (false);
}
