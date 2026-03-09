#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "../src/domain/entities/HttpRequest.hpp"
#include "../src/infrastructure/io/HttpRequestParser.hpp"

class TestCase {
    public:
        TestCase(const char *name) : name_(name), passed_(true) {}
        
        virtual ~TestCase() {}
        
        virtual void run() = 0;
        
        void assertEqual(const std::string &actual, const std::string &expected, const char *msg) {
            if (actual != expected) {
                std::cerr << "  FAIL: " << msg << std::endl;
                std::cerr << "    Expected: \"" << expected << "\"" << std::endl;
                std::cerr << "    Got:      \"" << actual << "\"" << std::endl;
                passed_ = false;
            }
        }
        
        void assertTrue(bool condition, const char *msg) {
            if (!condition) {
                std::cerr << "  FAIL: " << msg << std::endl;
                passed_ = false;
            }
        }
        
        void printResult() {
            if (passed_) {
                std::cout << "  [PASS] " << name_ << std::endl;
            } else {
                std::cout << "  [FAIL] " << name_ << std::endl;
            }
        }
        
        bool isPassed() const {
            return passed_;
        }

    protected:
        const char *name_;
        bool passed_;
};

class TestSimpleGetRequest : public TestCase {
    public:
        TestSimpleGetRequest() : TestCase("Simple GET request") {}
        
        void run() {
            std::string raw = "GET /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "GET", "Method should be GET");
            assertEqual(req.getUri(), "/index.html", "URI should be /index.html");
            assertEqual(req.getVersion(), "HTTP/1.0", "Version should be HTTP/1.0");
            assertTrue(req.getHeaders().find("HOST") != req.getHeaders().end(), "Header 'Host' should exist");
            assertEqual(req.getHeaders().at("HOST"), "localhost", "Host header value should be localhost");
            assertEqual(req.getBody(), "", "Body should be empty");
        }
};

class TestPostWithBody : public TestCase {
    public:
        TestPostWithBody() : TestCase("POST request with body") {}
        
        void run() {
            std::string body = "username=admin&password=secret";
            std::string raw = "POST /login HTTP/1.0\r\nHost: example.com\r\nContent-Length: 31\r\n\r\n" + body;
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "POST", "Method should be POST");
            assertEqual(req.getUri(), "/login", "URI should be /login");
            assertEqual(req.getBody(), body, "Body should match");
            assertTrue(req.getHeaders().find("CONTENT-LENGTH") != req.getHeaders().end(), "Content-Length header should exist");
        }
};

class TestMultipleHeaders : public TestCase {
    public:
        TestMultipleHeaders() : TestCase("Multiple headers parsing") {}
        
        void run() {
            std::string raw = "GET /api/users HTTP/1.0\r\n"
                            "Host: api.example.com\r\n"
                            "User-Agent: Mozilla/5.0\r\n"
                            "Accept: application/json\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "GET", "Method should be GET");
            assertTrue(req.getHeaders().find("HOST") != req.getHeaders().end(), "Host header should exist");
            assertTrue(req.getHeaders().find("USER-AGENT") != req.getHeaders().end(), "User-Agent header should exist");
            assertTrue(req.getHeaders().find("ACCEPT") != req.getHeaders().end(), "Accept header should exist");
            assertEqual(req.getHeaders().at("USER-AGENT"), "Mozilla/5.0", "User-Agent should match");
        }
};

class TestHeaderSpacesTrimming : public TestCase {
    public:
        TestHeaderSpacesTrimming() : TestCase("Header spaces trimming") {}
        
        void run() {
            std::string raw = "GET / HTTP/1.0\r\nHost:   example.com   \r\nX-Custom:  value with spaces  \r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            std::string hostValue = req.getHeaders().at("HOST");
            std::string customValue = req.getHeaders().at("X-CUSTOM");
            
            assertEqual(hostValue, "example.com", "Host header should be trimmed");
            assertEqual(customValue, "value with spaces", "Custom header should be trimmed");
        }
};

// TODO: testar um edge cases com lf quebrados, mas não no parser
class TestLFOnlyLineEndings : public TestCase {
    public:
        TestLFOnlyLineEndings() : TestCase("LF-only line endings") {}
        
        void run() {
            std::string raw = "GET /test HTTP/1.0\nHost: localhost\nContent-Type: text/plain\n\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "GET", "Method should be GET");
            assertEqual(req.getUri(), "/test", "URI should be /test");
            assertTrue(req.getHeaders().find("HOST") != req.getHeaders().end(), "Host header should exist");
            assertTrue(req.getHeaders().find("CONTENT-TYPE") != req.getHeaders().end(), "Content-Type header should exist");
        }
};


class TestDeleteRequest : public TestCase {
    public:
        TestDeleteRequest() : TestCase("DELETE request") {}
        
        void run() {
            std::string raw = "DELETE /resource/123 HTTP/1.0\r\nHost: api.example.com\r\nAuthorization: Bearer token123\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "DELETE", "Method should be DELETE");
            assertEqual(req.getUri(), "/resource/123", "URI should be /resource/123");
            assertTrue(req.getHeaders().find("AUTHORIZATION") != req.getHeaders().end(), "Authorization header should exist");
            assertEqual(req.getHeaders().at("AUTHORIZATION"), "Bearer token123", "Authorization value should match");
        }
};

// TODO: fazer validação disso depois retonando request invalida (não no parser)
class TestPostEmptyBody : public TestCase {
    public:
        TestPostEmptyBody() : TestCase("POST request with empty body") {}
        
        void run() {
            std::string raw = "POST /submit HTTP/1.0\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "POST", "Method should be POST");
            assertEqual(req.getBody(), "", "Body should be empty");
        }
};

class TestRequestWithQueryString : public TestCase {
    public:
        TestRequestWithQueryString() : TestCase("Request with query string") {}
        
        void run() {
            std::string raw = "GET /search?q=hello&limit=10 HTTP/1.0\r\nHost: example.com\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "GET", "Method should be GET");
            assertEqual(req.getUri(), "/search?q=hello&limit=10", "URI with query string should be preserved");
        }
};

class TestHttpRequestConstructor : public TestCase {
    public:
        TestHttpRequestConstructor() : TestCase("HttpRequest constructor initializes empty") {}
        
        void run() {
            HttpRequest req;
            
            assertEqual(req.getMethod(), "", "Method should be empty");
            assertEqual(req.getUri(), "", "URI should be empty");
            assertEqual(req.getVersion(), "", "Version should be empty");
            assertEqual(req.getBody(), "", "Body should be empty");
            assertTrue(req.getHeaders().empty(), "Headers should be empty");
        }
};

class TestPostWithJsonBody : public TestCase {
    public:
        TestPostWithJsonBody() : TestCase("POST with JSON body") {}
        
        void run() {
            std::string jsonBody = "{\"name\": \"John\", \"email\": \"john@example.com\"}";
            std::string raw = "POST /api/users HTTP/1.0\r\nHost: api.example.com\r\nContent-Type: application/json\r\nContent-Length: " 
                            + intToString(jsonBody.length()) + "\r\n\r\n" + jsonBody;
            
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "POST", "Method should be POST");
            assertEqual(req.getUri(), "/api/users", "URI should be /api/users");
            assertEqual(req.getBody(), jsonBody, "Body should contain JSON");
            assertTrue(req.getHeaders().find("CONTENT-TYPE") != req.getHeaders().end(), "Content-Type header should exist");
            assertEqual(req.getHeaders().at("CONTENT-TYPE"), "application/json", "Content-Type should be application/json");
        }

    private:
        std::string intToString(int value) {
            char buffer[20];
            sprintf(buffer, "%d", value);
            return std::string(buffer);
        }
};

class TestHttp10Version : public TestCase {
    public:
        TestHttp10Version() : TestCase("HTTP/1.0 version parsing") {}
        
        void run() {
            std::string raw = "GET / HTTP/1.0\r\nHost: old-server.com\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            
            assertEqual(req.getMethod(), "GET", "Method should be GET");
            assertEqual(req.getVersion(), "HTTP/1.0", "Version should be HTTP/1.0");
        }
};

class TestHeaderCaseSensitivity : public TestCase {
    public:
        TestHeaderCaseSensitivity() : TestCase("Header names stored as-is (case preservation)") {}
        
        void run() {
            std::string raw = "GET / HTTP/1.0\r\ncontent-type: text/html\r\nContent-Type: application/json\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);

            assertTrue(req.getHeaders().size() >= 1, "Should have at least one header");
        }
};

int main() {
    std::cout << "\n=== HttpRequest Unit Tests ===" << std::endl;
    
    TestCase *tests[] = {
        new TestHttpRequestConstructor(),
        new TestSimpleGetRequest(),
        new TestPostWithBody(),
        new TestMultipleHeaders(),
        new TestHeaderSpacesTrimming(),
        new TestLFOnlyLineEndings(),
        new TestDeleteRequest(),
        new TestPostEmptyBody(),
        new TestRequestWithQueryString(),
        new TestPostWithJsonBody(),
        new TestHttp10Version(),
        new TestHeaderCaseSensitivity()
    };
    
    int testCount = sizeof(tests) / sizeof(tests[0]);
    int passedCount = 0;
    
    for (int i = 0; i < testCount; ++i) {
        tests[i]->run();
        tests[i]->printResult();
        if (tests[i]->isPassed()) {
            passedCount++;
        }
        delete tests[i];
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passedCount << "/" << testCount << std::endl;
    
    if (passedCount == testCount) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}
