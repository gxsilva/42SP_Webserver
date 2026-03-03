#ifndef HTTPREQUESTVALIDATOR_HPP
#define HTTPREQUESTVALIDATOR_HPP

#include "../entities/HttpRequest.hpp"
#include "../errors/ValidationError.hpp"
#include <string>

class HttpRequestValidator {
public:
    HttpRequestValidator();
    ~HttpRequestValidator();

    // Returns empty string if valid, error message if invalid
    std::string validate(const HttpRequest &req) const;

private:
    bool isValidMethod(const std::string &method) const;
    bool isValidVersion(const std::string &version) const;
    bool isValidUri(const std::string &uri) const;
    bool hasRequiredHeaders(const HttpRequest &req) const;
    bool isValidContentLength(const HttpRequest &req) const;
};

#endif // HTTPREQUESTVALIDATOR_HPP
