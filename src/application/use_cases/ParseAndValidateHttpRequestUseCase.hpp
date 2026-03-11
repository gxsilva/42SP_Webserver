#ifndef PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
#define PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/services/HttpRequestValidator.hpp"
#include "../../infrastructure/common/Result.hpp"
#include "../../infrastructure/io/HttpRequestParser.hpp"
#include <string>

class ParseAndValidateHttpRequestUseCase
{
	private:
		HttpRequestParser	 _parser;
		HttpRequestValidator _validator;

	public:
		ParseAndValidateHttpRequestUseCase();
		~ParseAndValidateHttpRequestUseCase();

		Result<HttpRequest> execute(const std::string& rawRequest);
};

#endif // PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
