#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <cstdio>
#include "../src/domain/entities/HttpRequest.hpp"
#include "../src/domain/value_objects/CgiEnvironment.hpp"
#include "../src/domain/value_objects/CgiResponse.hpp"

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

        void assertIntEqual(int actual, int expected, const char *msg) {
            if (actual != expected) {
                std::cerr << "  FAIL: " << msg << std::endl;
                std::cerr << "    Expected: " << expected << std::endl;
                std::cerr << "    Got:      " << actual << std::endl;
                passed_ = false;
            }
        }

        void assertTrue(bool condition, const char *msg) {
            if (!condition) {
                std::cerr << "  FAIL: " << msg << std::endl;
                passed_ = false;
            }
        }

        void assertFalse(bool condition, const char *msg) {
            assertTrue(!condition, msg);
        }

        void printResult() {
            if (passed_)
                std::cout << "  [PASS] " << name_ << std::endl;
            else
                std::cout << "  [FAIL] " << name_ << std::endl;
        }

        bool isPassed() const { return passed_; }

    protected:
        const char *name_;
        bool passed_;
};

// ========== Helper ==========

static std::string findEnvVar(char** envp, const std::string& key)
{
    std::string prefix = key + "=";
    for (int i = 0; envp[i] != NULL; ++i)
    {
        std::string entry(envp[i]);
        if (entry.compare(0, prefix.size(), prefix) == 0)
            return entry.substr(prefix.size());
    }
    return "";
}

// ========== CgiEnvironment Tests ==========

class TestEnvBasicGetRequest : public TestCase {
    public:
        TestEnvBasicGetRequest() : TestCase("CgiEnvironment: basic GET request") {}

        void run() {
            HttpRequest req;
            req.setMethod("GET");
            req.setUri("/cgi-bin/test.py?name=fabio&age=25");
            req.setVersion("HTTP/1.1");
            req.setHeader("Host", "localhost");

            CgiEnvironment env(req, "/cgi-bin/test.py", "localhost", 8080);
            char** envp = env.toEnvArray();

            assertEqual(findEnvVar(envp, "REQUEST_METHOD"), "GET", "REQUEST_METHOD should be GET");
            assertEqual(findEnvVar(envp, "QUERY_STRING"), "name=fabio&age=25", "QUERY_STRING should be parsed from URI");
            assertEqual(findEnvVar(envp, "PATH_INFO"), "/cgi-bin/test.py", "PATH_INFO should be URI path without query");
            assertEqual(findEnvVar(envp, "SCRIPT_NAME"), "/cgi-bin/test.py", "SCRIPT_NAME should match");
            assertEqual(findEnvVar(envp, "SERVER_NAME"), "localhost", "SERVER_NAME should be localhost");
            assertEqual(findEnvVar(envp, "SERVER_PORT"), "8080", "SERVER_PORT should be 8080");
            assertEqual(findEnvVar(envp, "SERVER_PROTOCOL"), "HTTP/1.1", "SERVER_PROTOCOL should be HTTP/1.1");
            assertEqual(findEnvVar(envp, "GATEWAY_INTERFACE"), "CGI/1.1", "GATEWAY_INTERFACE should be CGI/1.1");

            CgiEnvironment::freeEnvArray(envp);
        }
};

class TestEnvPostRequest : public TestCase {
    public:
        TestEnvPostRequest() : TestCase("CgiEnvironment: POST request with content headers") {}

        void run() {
            HttpRequest req;
            req.setMethod("POST");
            req.setUri("/cgi-bin/form.py");
            req.setVersion("HTTP/1.1");
            req.setHeader("Host", "example.com");
            req.setHeader("Content-Type", "application/x-www-form-urlencoded");
            req.setHeader("Content-Length", "28");
            req.setBody("username=admin&password=1234");

            CgiEnvironment env(req, "/cgi-bin/form.py", "example.com", 80);
            char** envp = env.toEnvArray();

            assertEqual(findEnvVar(envp, "REQUEST_METHOD"), "POST", "REQUEST_METHOD should be POST");
            assertEqual(findEnvVar(envp, "CONTENT_TYPE"), "application/x-www-form-urlencoded", "CONTENT_TYPE should match");
            assertEqual(findEnvVar(envp, "CONTENT_LENGTH"), "28", "CONTENT_LENGTH should be 28");
            assertEqual(findEnvVar(envp, "QUERY_STRING"), "", "QUERY_STRING should be empty for POST without query");

            CgiEnvironment::freeEnvArray(envp);
        }
};

class TestEnvHttpHeaders : public TestCase {
    public:
        TestEnvHttpHeaders() : TestCase("CgiEnvironment: HTTP headers converted to HTTP_* vars") {}

        void run() {
            HttpRequest req;
            req.setMethod("GET");
            req.setUri("/test");
            req.setVersion("HTTP/1.1");
            req.setHeader("Host", "myhost.com");
            req.setHeader("Accept", "text/html");
            req.setHeader("User-Agent", "TestBot/1.0");

            CgiEnvironment env(req, "/test", "myhost.com", 3000);
            char** envp = env.toEnvArray();

            assertEqual(findEnvVar(envp, "HTTP_HOST"), "myhost.com", "HTTP_HOST should exist");
            assertEqual(findEnvVar(envp, "HTTP_ACCEPT"), "text/html", "HTTP_ACCEPT should exist");
            assertEqual(findEnvVar(envp, "HTTP_USER_AGENT"), "TestBot/1.0", "HTTP_USER_AGENT with underscore");

            CgiEnvironment::freeEnvArray(envp);
        }
};

class TestEnvNoQueryString : public TestCase {
    public:
        TestEnvNoQueryString() : TestCase("CgiEnvironment: URI without query string") {}

        void run() {
            HttpRequest req;
            req.setMethod("GET");
            req.setUri("/index.html");
            req.setVersion("HTTP/1.1");
            req.setHeader("Host", "localhost");

            CgiEnvironment env(req, "/index.html", "localhost", 80);
            char** envp = env.toEnvArray();

            assertEqual(findEnvVar(envp, "QUERY_STRING"), "", "QUERY_STRING should be empty");
            assertEqual(findEnvVar(envp, "PATH_INFO"), "/index.html", "PATH_INFO should be full URI");

            CgiEnvironment::freeEnvArray(envp);
        }
};

class TestEnvFreeNull : public TestCase {
    public:
        TestEnvFreeNull() : TestCase("CgiEnvironment: freeEnvArray(NULL) doesn't crash") {}

        void run() {
            CgiEnvironment::freeEnvArray(NULL);
            assertTrue(true, "freeEnvArray(NULL) should not crash");
        }
};

// ========== CgiResponse Tests ==========

class TestResponseBasicParse : public TestCase {
    public:
        TestResponseBasicParse() : TestCase("CgiResponse: basic header+body parse") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Content-Type: text/html\r\n\r\n<h1>Hello</h1>";
            assertTrue(resp.parse(raw), "parse should return true");
            assertEqual(resp.getBody(), "<h1>Hello</h1>", "Body should match");
            assertEqual(resp.getHeader("Content-Type"), "text/html", "Content-Type header should match");
            assertIntEqual(resp.getStatusCode(), 200, "Default status should be 200");
        }
};

class TestResponseWithStatus : public TestCase {
    public:
        TestResponseWithStatus() : TestCase("CgiResponse: Status header sets status code") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Status: 302 Found\r\nLocation: /redirect\r\n\r\nRedirecting...";
            assertTrue(resp.parse(raw), "parse should return true");
            assertIntEqual(resp.getStatusCode(), 302, "Status code should be 302");
            assertEqual(resp.getHeader("Location"), "/redirect", "Location header should match");
            assertEqual(resp.getHeader("Status"), "", "Status header should be removed after parsing");
        }
};

class TestResponseLFOnly : public TestCase {
    public:
        TestResponseLFOnly() : TestCase("CgiResponse: LF-only line endings") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Content-Type: text/plain\n\nHello World";
            assertTrue(resp.parse(raw), "parse should handle LF-only");
            assertEqual(resp.getBody(), "Hello World", "Body should match");
            assertEqual(resp.getHeader("Content-Type"), "text/plain", "Header should match");
        }
};

class TestResponseNoSeparator : public TestCase {
    public:
        TestResponseNoSeparator() : TestCase("CgiResponse: no header/body separator returns false") {}

        void run() {
            CgiResponse resp;
            std::string raw = "just some text without separator";
            assertFalse(resp.parse(raw), "parse should return false without separator");
        }
};

class TestResponseInvalidStatus : public TestCase {
    public:
        TestResponseInvalidStatus() : TestCase("CgiResponse: invalid Status falls back to 500") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Status: xyz\r\nContent-Type: text/html\r\n\r\nerror";
            assertTrue(resp.parse(raw), "parse should return true");
            assertIntEqual(resp.getStatusCode(), 500, "Invalid status should fallback to 500");
        }
};

class TestResponseStatus999 : public TestCase {
    public:
        TestResponseStatus999() : TestCase("CgiResponse: out-of-range status (999) falls back to 500") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Status: 999\r\n\r\nbad";
            assertTrue(resp.parse(raw), "parse should return true");
            assertIntEqual(resp.getStatusCode(), 500, "Status 999 should fallback to 500");
        }
};

class TestResponseMultipleHeaders : public TestCase {
    public:
        TestResponseMultipleHeaders() : TestCase("CgiResponse: multiple headers parsed") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Content-Type: text/html\r\nX-Custom: value1\r\nSet-Cookie: id=abc\r\n\r\nbody";
            assertTrue(resp.parse(raw), "parse should return true");
            assertEqual(resp.getHeader("Content-Type"), "text/html", "Content-Type should match");
            assertEqual(resp.getHeader("X-Custom"), "value1", "X-Custom should match");
            assertEqual(resp.getHeader("Set-Cookie"), "id=abc", "Set-Cookie should match");
        }
};

class TestResponseEmptyBody : public TestCase {
    public:
        TestResponseEmptyBody() : TestCase("CgiResponse: empty body after headers") {}

        void run() {
            CgiResponse resp;
            std::string raw = "Content-Type: text/html\r\n\r\n";
            assertTrue(resp.parse(raw), "parse should return true");
            assertEqual(resp.getBody(), "", "Body should be empty");
        }
};

class TestResponseParseResets : public TestCase {
    public:
        TestResponseParseResets() : TestCase("CgiResponse: second parse resets previous state") {}

        void run() {
            CgiResponse resp;
            std::string raw1 = "Status: 404\r\nX-First: yes\r\n\r\nfirst body";
            resp.parse(raw1);
            assertIntEqual(resp.getStatusCode(), 404, "First parse status should be 404");

            std::string raw2 = "Content-Type: text/plain\r\n\r\nsecond body";
            resp.parse(raw2);
            assertIntEqual(resp.getStatusCode(), 200, "Second parse should reset to 200");
            assertEqual(resp.getBody(), "second body", "Body should be from second parse");
            assertEqual(resp.getHeader("X-First"), "", "Old headers should be gone");
        }
};

int main() {
    std::cout << "\n=== CGI Unit Tests ===" << std::endl;

    std::cout << "\n--- CgiEnvironment ---" << std::endl;
    TestCase *envTests[] = {
        new TestEnvBasicGetRequest(),
        new TestEnvPostRequest(),
        new TestEnvHttpHeaders(),
        new TestEnvNoQueryString(),
        new TestEnvFreeNull()
    };

    std::cout << "\n--- CgiResponse ---" << std::endl;
    TestCase *respTests[] = {
        new TestResponseBasicParse(),
        new TestResponseWithStatus(),
        new TestResponseLFOnly(),
        new TestResponseNoSeparator(),
        new TestResponseInvalidStatus(),
        new TestResponseStatus999(),
        new TestResponseMultipleHeaders(),
        new TestResponseEmptyBody(),
        new TestResponseParseResets()
    };

    int totalCount = 0;
    int passedCount = 0;

    int envCount = sizeof(envTests) / sizeof(envTests[0]);
    for (int i = 0; i < envCount; ++i) {
        envTests[i]->run();
        envTests[i]->printResult();
        if (envTests[i]->isPassed())
            passedCount++;
        totalCount++;
        delete envTests[i];
    }

    int respCount = sizeof(respTests) / sizeof(respTests[0]);
    for (int i = 0; i < respCount; ++i) {
        respTests[i]->run();
        respTests[i]->printResult();
        if (respTests[i]->isPassed())
            passedCount++;
        totalCount++;
        delete respTests[i];
    }

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passedCount << "/" << totalCount << std::endl;

    if (passedCount == totalCount) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}
