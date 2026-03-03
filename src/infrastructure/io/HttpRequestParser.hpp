#ifndef HTTPREQUESTPARSER_HPP
#define HTTPREQUESTPARSER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include <string>

class HttpRequestParser {
public:
    HttpRequestParser();
    ~HttpRequestParser();

    HttpRequest parse(const std::string &raw);

private:
    void parseRequestLine(const std::string &line, HttpRequest &req);
    void parseHeaders(const std::string &headerBlock, HttpRequest &req);
};

#endif // HTTPREQUESTPARSER_HPP
