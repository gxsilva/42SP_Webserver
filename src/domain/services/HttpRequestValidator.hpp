#ifndef HTTPREQUESTVALIDATOR_HPP
#define HTTPREQUESTVALIDATOR_HPP

#include "../entities/HttpRequest.hpp"
#include "../errors/ValidationError.hpp"
#include <string>

// TODO: fazer validação de mais coisas depois (ex: content-length, etc)
class HttpRequestValidator {
    public:
        HttpRequestValidator();
        ~HttpRequestValidator();

        std::string validate(const HttpRequest &req) const;

    private:
        std::string toUpperCase(const std::string &str) const;
        bool isValidMethod(const std::string &method) const;
        bool isValidVersion(const std::string &version) const;
        bool isValidUri(const std::string &uri) const;
        bool hasRequiredHeaders(const HttpRequest &req) const;
        bool isValidContentLength(const HttpRequest &req) const;
};

#endif // HTTPREQUESTVALIDATOR_HPP
