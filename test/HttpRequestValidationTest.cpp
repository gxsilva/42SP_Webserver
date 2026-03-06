#include <iostream>
#include <cstring>
#include <cstdio>
#include "../src/domain/entities/HttpRequest.hpp"
#include "../src/infrastructure/io/HttpRequestParser.hpp"
#include "../src/domain/services/HttpRequestValidator.hpp"

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
        
        void assertFalse(bool condition, const char *msg) {
            if (condition) {
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

class TestValidGetRequest : public TestCase {
    public:
        TestValidGetRequest() : TestCase("Valid GET request passes validation") {}
        
        void run() {
            std::string raw = "GET /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertEqual(error, "", "Valid GET request should produce no error");
        }
};

class TestValidPostRequest : public TestCase {
    public:
        TestValidPostRequest() : TestCase("Valid POST request with body passes validation") {}
        
        void run() {
            std::string body = "data=value";
            std::string raw = "POST /api/submit HTTP/1.0\r\nHost: example.com\r\nContent-Length: 10\r\n\r\n" + body;
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertEqual(error, "", "Valid POST request should produce no error");
        }
};

class TestValidDeleteRequest : public TestCase {
    public:
        TestValidDeleteRequest() : TestCase("Valid DELETE request passes validation") {}
        
        void run() {
            std::string raw = "DELETE /resource/123 HTTP/1.0\r\nHost: api.example.com\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertEqual(error, "", "Valid DELETE request should produce no error");
        }
};

class TestInvalidMethod : public TestCase {
    public:
        TestInvalidMethod() : TestCase("Invalid HTTP method is rejected") {}
        
        void run() {
            std::string raw = "PUT /resource HTTP/1.0\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "Invalid method should produce error");
            assertTrue(error.find("Invalid HTTP method") != std::string::npos, "Error should mention invalid method");
        }
};

class TestInvalidVersion : public TestCase {
    public:
        TestInvalidVersion() : TestCase("HTTP/1.1 version is rejected") {}
        
        void run() {
            std::string raw = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "HTTP/1.1 should produce error");
            assertTrue(error.find("Invalid HTTP version") != std::string::npos, "Error should mention invalid version");
        }
};

class TestMissingHostHeader : public TestCase {
    public:
        TestMissingHostHeader() : TestCase("Missing Host header is rejected") {}
        
        void run() {
            std::string raw = "GET / HTTP/1.0\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "Missing Host header should produce error");
            assertTrue(error.find("Host") != std::string::npos, "Error should mention Host header");
        }
};

class TestEmptyUri : public TestCase {
    public:
        TestEmptyUri() : TestCase("Empty URI is rejected") {}
        
        void run() {
            std::string raw = "GET  HTTP/1.0\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "Empty URI should produce error");
            assertTrue(error.find("URI") != std::string::npos || error.find("Invalid") != std::string::npos, 
                    "Error should mention URI");
        }
};

class TestInvalidContentLengthNegative : public TestCase {
    public:
        TestInvalidContentLengthNegative() : TestCase("Negative Content-Length is rejected") {}
        
        void run() {
            std::string raw = "POST / HTTP/1.0\r\nHost: localhost\r\nContent-Length: -10\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "Negative Content-Length should produce error");
            assertTrue(error.find("Content-Length") != std::string::npos, "Error should mention Content-Length");
        }
};

class TestInvalidContentLengthNonNumeric : public TestCase {
    public:
        TestInvalidContentLengthNonNumeric() : TestCase("Non-numeric Content-Length is rejected") {}
        
        void run() {
            std::string raw = "POST / HTTP/1.0\r\nHost: localhost\r\nContent-Length: abc\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertTrue(!error.empty(), "Non-numeric Content-Length should produce error");
            assertTrue(error.find("Content-Length") != std::string::npos, "Error should mention Content-Length");
        }
};

class TestValidContentLengthZero : public TestCase {
    public:
        TestValidContentLengthZero() : TestCase("Zero Content-Length is valid") {}
        
        void run() {
            std::string raw = "POST / HTTP/1.0\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertEqual(error, "", "Zero Content-Length should be valid");
        }
};

class TestOptionalContentLength : public TestCase {
    public:
        TestOptionalContentLength() : TestCase("Missing Content-Length is optional and valid") {}
        
        void run() {
            std::string raw = "POST / HTTP/1.0\r\nHost: localhost\r\n\r\n";
            HttpRequestParser parser;
            HttpRequest req = parser.parse(raw);
            HttpRequestValidator validator;
            
            std::string error = validator.validate(req);
            assertEqual(error, "", "Missing Content-Length should be optional");
        }
};

int main() {
    std::cout << "\n=== HttpRequest Validation Tests ===" << std::endl;
    
    TestCase *tests[] = {
        new TestValidGetRequest(),
        new TestValidPostRequest(),
        new TestValidDeleteRequest(),
        new TestInvalidMethod(),
        new TestInvalidVersion(),
        new TestMissingHostHeader(),
        new TestEmptyUri(),
        new TestInvalidContentLengthNegative(),
        new TestInvalidContentLengthNonNumeric(),
        new TestValidContentLengthZero(),
        new TestOptionalContentLength()
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
