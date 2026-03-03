#include "HttpRequestParser.hpp"
#include <sstream>

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

HttpRequest HttpRequestParser::parse(const std::string &raw) {
    HttpRequest req;

    // find end of headers (CRLFCRLF or LFLF)
    size_t header_end = std::string::npos;
    size_t pos = raw.find("\r\n\r\n");
    size_t eol_len = 4;
    if (pos != std::string::npos) {
        header_end = pos;
        eol_len = 4;
    } else {
        pos = raw.find("\n\n");
        if (pos != std::string::npos) {
            header_end = pos;
            eol_len = 2;
        }
    }

    std::string head;
    if (header_end != std::string::npos) head = raw.substr(0, header_end);
    else head = raw;

    // body
    if (header_end != std::string::npos) {
        size_t body_start = header_end + eol_len;
        if (body_start < raw.size()) req.setBody(raw.substr(body_start));
    }

    // extract request line
    size_t line_end = head.find("\r\n");
    if (line_end == std::string::npos) line_end = head.find('\n');
    std::string request_line;
    if (line_end != std::string::npos) request_line = head.substr(0, line_end);
    else request_line = head;

    parseRequestLine(request_line, req);

    // headers: start after the first line
    size_t headers_start = 0;
    if (line_end != std::string::npos) {
        // compute correct offset for CRLF vs LF
        if (line_end + 1 < head.size() && head[line_end] == '\r' && head[line_end+1] == '\n') headers_start = line_end + 2;
        else headers_start = line_end + 1;
    } else {
        headers_start = request_line.size();
    }

    std::string rest = (headers_start < head.size()) ? head.substr(headers_start) : std::string();
    parseHeaders(rest, req);

    return req;
}

void HttpRequestParser::parseRequestLine(const std::string &line, HttpRequest &req) {
    std::istringstream rl(line);
    std::string method, url, version;
    rl >> method >> url >> version;
    req.setMethod(method);
    req.setUri(url);
    req.setVersion(version);
}

void HttpRequestParser::parseHeaders(const std::string &headerBlock, HttpRequest &req) {
    size_t start = 0;
    while (start < headerBlock.size()) {
        size_t eol = headerBlock.find("\r\n", start);
        size_t line_eol_len = 2;
        if (eol == std::string::npos) {
            eol = headerBlock.find('\n', start);
            line_eol_len = 1;
        }
        if (eol == std::string::npos) eol = headerBlock.size();
        std::string line = headerBlock.substr(start, eol - start);
        if (line.empty()) break;
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            // trim leading spaces
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(0, 1);
            // trim trailing spaces
            while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t')) value.erase(value.size() - 1, 1);
            req.setHeader(name, value);
        }
        start = eol + line_eol_len;
    }
}
