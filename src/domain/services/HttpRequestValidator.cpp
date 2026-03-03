#include "HttpRequestValidator.hpp"
#include <sstream>
#include <cstdlib>

// criar constantes ou entitidades para para valores que podemos receber (métodos, versões, etc)

HttpRequestValidator::HttpRequestValidator() {}

HttpRequestValidator::~HttpRequestValidator() {}

std::string HttpRequestValidator::validate(const HttpRequest &req) const {
    if (!isValidMethod(req.getMethod())) {
        return "Invalid HTTP method: " + req.getMethod();
    }

    if (!isValidVersion(req.getVersion())) {
        return "Invalid HTTP version: " + req.getVersion() + ". Only HTTP/1.0 is supported.";
    }

    if (!isValidUri(req.getUri())) {
        return "Invalid URI: URI cannot be empty.";
    }

    if (!hasRequiredHeaders(req)) {
        return "Missing required header: Host is required.";
    }

    if (!isValidContentLength(req)) {
        return "Invalid Content-Length header value.";
    }

    return std::string();
}

// headers são case insensitive, faz sentido fazer a validação desse jeito?
bool HttpRequestValidator::isValidMethod(const std::string &method) const {
    return (method == "GET" || method == "POST" || method == "DELETE");
}

// tratar case sensitive aqui
bool HttpRequestValidator::isValidVersion(const std::string &version) const {
    return (version == "HTTP/1.0");
}

bool HttpRequestValidator::isValidUri(const std::string &uri) const {
    return !uri.empty();
}

bool HttpRequestValidator::hasRequiredHeaders(const HttpRequest &req) const {
    const std::map<std::string, std::string> &headers = req.getHeaders();
    return headers.find("Host") != headers.end();
}

bool HttpRequestValidator::isValidContentLength(const HttpRequest &req) const {
    const std::map<std::string, std::string> &headers = req.getHeaders();
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
    
    if (it == headers.end()) {
        return true;
    }

    const std::string &contentLengthStr = it->second;
    
    if (contentLengthStr.empty()) {
        return false;
    }

    char *endPtr = NULL;
    long contentLength = strtol(contentLengthStr.c_str(), &endPtr, 10);
    
    if (*endPtr != '\0' || contentLength < 0) {
        return false;
    }

    return true;
}
