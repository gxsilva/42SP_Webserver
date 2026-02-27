/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 23:06:00 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
// #include "domain/entities/Token.hpp"
#include "../../domain/errors/CompilerError.hpp"
#include "../../domain/errors/ErrorList.hpp"

#include "../../application/use_cases/CompileSourceFile.hpp"
#include "../../domain/entities/Token.hpp"
#include "../../infrastructure/common/TokenResult.hpp"

#include <vector>

int baseTest();

int main()
{
	LexerResult res = CompileSourceFile::execute("config.conf");

	if (res.isErr())
	{
		const ErrorList& errors = res.error();
		// std::cout << "Failed to load source file. Errors:\n";
		errors.formatAllErrors();
	}
	else
		std::cout << "Source file loaded successfully. Lexer is ready for tokenization.\n";
	// return baseTest();
	Lexer* lexer = res.unwrap();
	(void)lexer;
	return (0);
}

int baseTest()
{
	// std::string fileName = "config.conf";
	// SourceLocation loc(fileName, 1, 1, 6);
	// std::cout << loc.toString() << '\n';
	// Token t(WORD, "server", SourceLocation("config.conf", 1, 1, 6));
	// std::cout << t.toString() << '\n';
	// ------------------------ ------------------------ //

	CompilerError err1 = CompilerError::fileNotFoundError("config.conf");
	CompilerError::fileNotFoundError("config.conf");
	err1.addNote("Make sure the file exists and is in the correct directory.");
	err1.addHint("Check the file path and permissions.");
	const std::string err1_format = err1.format();
	std::cout << "err1 format:\n" << err1_format << '\n';

	const CompilerError err2		= CompilerError::permissionDeniedError("config.conf");
	const std::string	err2_format = err2.format();
	std::cout << "err2 format:\n" << err2_format << '\n';

	ErrorList errorList;
	errorList.addError(err1);
	errorList.addError(err2);

	errorList.formatAllErrors();
	const std::vector<CompilerError> errors = errorList.getErrors();
	std::cout << "Retrieved " << errors.size() << " errors from the error list." << '\n';

	std::cout << "\n\nTotal errors: " << errorList.size() << '\n';

	const ErrorList emptyErrorList;
	std::cout << "[Empty error list] has errors? " << (emptyErrorList.hasErrors() ? "Yes" : "No")
			  << '\n';

	return 0;
}
