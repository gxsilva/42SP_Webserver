#include "HttpRequestValidator.hpp"
#include <sstream>
#include <cstdlib>

HttpRequestValidator::HttpRequestValidator() {}

HttpRequestValidator::~HttpRequestValidator() {}

std::string HttpRequestValidator::toUpperCase(const std::string &str) const {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

std::string HttpRequestValidator::validate(const HttpRequest &req) const {
    if (!isValidMethod(toUpperCase(req.getMethod()))) {
        return "Invalid HTTP method: " + req.getMethod();
    }

    if (!isValidVersion(toUpperCase(req.getVersion()))) {
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

bool HttpRequestValidator::isValidMethod(const std::string &method) const {
    return (method == "GET" || method == "POST" || method == "DELETE");
}

bool HttpRequestValidator::isValidVersion(const std::string &version) const {
    return (version == "HTTP/1.0");
}

bool HttpRequestValidator::isValidUri(const std::string &uri) const {
    return !uri.empty();
}

bool HttpRequestValidator::hasRequiredHeaders(const HttpRequest &req) const {
    std::map<std::string, std::string>::const_iterator itBegin = req.getHeaders().begin();
    std::map<std::string, std::string>::const_iterator itEnd = req.getHeaders().end();
    
    while (itBegin != itEnd) {
         if (toUpperCase(itBegin->first) == "HOST") {
             return true;
         }
         ++itBegin;
    }
    return false;
}

bool HttpRequestValidator::isValidContentLength(const HttpRequest &req) const {
    std::map<std::string, std::string>::const_iterator itBegin = req.getHeaders().begin();
    std::map<std::string, std::string>::const_iterator itEnd = req.getHeaders().end();
    
    while (itBegin != itEnd) {
        if (toUpperCase(itBegin->first) == "CONTENT-LENGTH") {
            break;
        }
         ++itBegin;
    }  

    // Content-Length is optional
    if (itBegin == itEnd) {
        return true;
    }

    const std::string &contentLengthStr = itBegin->second;
    
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
