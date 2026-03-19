#include "ParseAndValidateHttpRequestUseCase.hpp"

ParseAndValidateHttpRequestUseCase::ParseAndValidateHttpRequestUseCase() : _parser(), _validator() {}

ParseAndValidateHttpRequestUseCase::~ParseAndValidateHttpRequestUseCase() {}

Result< HttpRequest > ParseAndValidateHttpRequestUseCase::execute(const std::string& rawRequest)
{
	return execute(rawRequest, 0);
}

Result< HttpRequest > ParseAndValidateHttpRequestUseCase::execute(const std::string& rawRequest,
	size_t maxBodySize)
{
	HttpRequest request = _parser.parse(rawRequest);

	_lastIssue = _validator.validateDetailed(request, maxBodySize);

	if (!_lastIssue.hasError())
		return Result< HttpRequest >(request);

	return Result< HttpRequest >(_lastIssue.getMessage());
}

const HttpRequestValidationIssue& ParseAndValidateHttpRequestUseCase::getLastIssue() const { return _lastIssue; }
