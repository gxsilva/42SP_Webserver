#ifndef PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
#define PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/services/request/HttpRequestValidator.hpp"
#include "../../infrastructure/common/Result.hpp"
#include "../../infrastructure/io/request/HttpRequestParser.hpp"
#include <string>

class ParseAndValidateHttpRequestUseCase
{
	private:
		HttpRequestParser	 _parser;
		HttpRequestValidator _validator;

	public:
		ParseAndValidateHttpRequestUseCase();
		~ParseAndValidateHttpRequestUseCase();

		Result< HttpRequest > execute(const std::string& rawRequest);
};

#endif // PARSEANDVALIDATEHTTPREQUESTUSECASE_HPP
