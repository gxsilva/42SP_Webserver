#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../interfaces/port/InitCgiGateway.hpp"
#include "../../domain/value_objects/CgiEnvironment.hpp"
#include "../../domain/value_objects/CgiResponse.hpp"
#include <string>

struct CgiRouteConfig
    {
        std::string scriptPath;
        std::string interpreterPath;
        std::string serverName;
        int         serverPort;
    };

    class CgiHandler
    {
    public:
        CgiHandler(InitCgiGateway& gateway, const CgiRouteConfig& config);
        ~CgiHandler();

        // Inicia execução CGI, retorna false se falhar
        bool startCgi(const HttpRequest& request);

        // Chamado pelo event loop quando poll() sinaliza readiness
        InitCgiGateway& getGateway();

        // Monta HttpResponse quando CGI termina
        HttpResponse buildResponse();

    private:
        CgiHandler();
        CgiHandler(const CgiHandler&);
        CgiHandler& operator=(const CgiHandler&);

        InitCgiGateway&   _gateway;
        CgiRouteConfig _config;
};

#endif