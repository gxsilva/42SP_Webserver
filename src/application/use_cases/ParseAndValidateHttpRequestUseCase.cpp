#include "ParseAndValidateHttpRequestUseCase.hpp"

ParseAndValidateHttpRequestUseCase::ParseAndValidateHttpRequestUseCase() : _parser(), _validator() {}

ParseAndValidateHttpRequestUseCase::~ParseAndValidateHttpRequestUseCase() {}

Result< HttpRequest > ParseAndValidateHttpRequestUseCase::execute(const std::string& rawRequest)
{
	HttpRequest request = _parser.parse(rawRequest);

	std::string validationError = _validator.validate(request);

	if (validationError.empty())
		return Result< HttpRequest >(request);
	else
		return Result< HttpRequest >(validationError);
}
