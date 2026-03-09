#include "HttpRequest.hpp"
#include <cctype>

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

const std::string &HttpRequest::getMethod() const {
    return _method;
}

const std::string &HttpRequest::getUri() const {
    return _uri;
}

const std::string &HttpRequest::getVersion() const {
    return _version;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
    return _headers;
}

const std::string &HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::setMethod(const std::string &method) {
    _method = method;
}

void HttpRequest::setUri(const std::string &uri) {
    _uri = uri;
}

void HttpRequest::setVersion(const std::string &version) {
    _version = version;
}

void HttpRequest::setHeader(const std::string &name, const std::string &value) {
    _headers[normalizeHeaderName(name)] = value;
}

void HttpRequest::setBody(const std::string &body) {
    _body = body;
}

std::string HttpRequest::normalizeHeaderName(const std::string &name) const {
    std::string normalized = name;
    for (size_t i = 0; i < normalized.length(); ++i) {
        normalized[i] = std::toupper(normalized[i]);
    }
    return normalized;
}
