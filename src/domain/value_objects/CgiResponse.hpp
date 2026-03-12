#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <sstream>

class CgiResponse
{
    public:
        CgiResponse();
        ~CgiResponse();

        bool parse(const std::string& rawOutput);

        const std::string& getBody() const;
        std::string        getHeader(const std::string& key) const;
        int                getStatusCode() const;
        const std::vector<std::pair<std::string, std::string> >& getHeaders() const;

    private:
        int                                                _statusCode;
        std::vector<std::pair<std::string, std::string> >  _headers;
        std::string                                        _body;

        void parseStatusFromHeaders();
};

#endif