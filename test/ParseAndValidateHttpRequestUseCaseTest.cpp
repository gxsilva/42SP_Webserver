#include "../src/application/use_cases/ParseAndValidateHttpRequestUseCase.hpp"
#include <cassert>
#include <iostream>
#include <string>

void testValidGetRequest()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string						   rawRequest = "GET /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n";

	Result< HttpRequest > result = useCase.execute(rawRequest);
	assert(result.isOk());
	assert(result.getValue().getMethod() == "GET");
	assert(result.getValue().getUri() == "/index.html");
	assert(result.getValue().getVersion() == "HTTP/1.0");
	std::cout << "✓ testValidGetRequest passed\n";
}

void testValidPostRequest()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string rawRequest = "POST /api/data HTTP/1.0\r\nHost: api.example.com\r\nContent-Length: 5\r\n\r\nhello";

	Result< HttpRequest > result = useCase.execute(rawRequest);
	assert(result.isOk());
	assert(result.getValue().getMethod() == "POST");
	assert(result.getValue().getUri() == "/api/data");
	std::cout << "✓ testValidPostRequest passed\n";
}

void testMissingHostHeader()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string						   rawRequest = "GET /index.html HTTP/1.0\r\n\r\n";

	Result< HttpRequest > result = useCase.execute(rawRequest);
	assert(result.isErr());
	assert(!result.getError().empty());
	std::cout << "✓ testMissingHostHeader passed\n";
}

void testInvalidMethod()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string						   rawRequest = "PUT /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n";

	Result< HttpRequest > result = useCase.execute(rawRequest);
	assert(result.isErr());
	std::cout << "✓ testInvalidMethod passed\n";
}

void testHttp11Version()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string						   rawRequest = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";

	Result< HttpRequest > result = useCase.execute(rawRequest);
	assert(result.isOk());
	assert(result.getValue().getVersion() == "HTTP/1.1");
	std::cout << "✓ testHttp11Version passed\n";
}

void testContentLengthMismatchReturns413()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string rawRequest = "POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nabc";

	Result< HttpRequest > result = useCase.execute(rawRequest, 1024);
	assert(result.isErr());
	assert(useCase.getLastIssue().hasError());
	assert(useCase.getLastIssue().getStatusCode() == 413);
	std::cout << "✓ testContentLengthMismatchReturns413 passed\n";
}

void testClientMaxBodySizeReturns413()
{
	ParseAndValidateHttpRequestUseCase useCase;
	std::string rawRequest = "POST /upload HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\nhello";

	Result< HttpRequest > result = useCase.execute(rawRequest, 3);
	assert(result.isErr());
	assert(useCase.getLastIssue().hasError());
	assert(useCase.getLastIssue().getStatusCode() == 413);
	std::cout << "✓ testClientMaxBodySizeReturns413 passed\n";
}

int main()
{
	std::cout << "\n=== ParseAndValidateHttpRequestUseCase Tests ===\n\n";

	testValidGetRequest();
	testValidPostRequest();
	testMissingHostHeader();
	testInvalidMethod();
	testHttp11Version();
	testContentLengthMismatchReturns413();
	testClientMaxBodySizeReturns413();

	std::cout << "\n✓ All 7 tests passed!\n\n";
	return 0;
}
