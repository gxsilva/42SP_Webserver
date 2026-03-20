#ifndef PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
#define PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/errors/request/HttpRequestValidationIssue.hpp"
#include "../../domain/services/request/HttpRequestValidator.hpp"
#include "../../infrastructure/common/Result.hpp"
#include "../../infrastructure/io/request/HttpRequestParser.hpp"
#include <string>

class ParseAndValidateHttpRequestUseCase
{
	private:
		HttpRequestParser		   _parser;
		HttpRequestValidator	   _validator;
		HttpRequestValidationIssue _lastIssue;

	public:
		ParseAndValidateHttpRequestUseCase();
		~ParseAndValidateHttpRequestUseCase();

		Result< HttpRequest >			  execute(const std::string& rawRequest);
		Result< HttpRequest >			  execute(const std::string& rawRequest, size_t maxBodySize);
		const HttpRequestValidationIssue& getLastIssue() const;
};

#endif // PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
