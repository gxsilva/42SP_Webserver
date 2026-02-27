/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 03:29:10 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
// #include "domain/entities/Token.hpp"
#include "../../domain/errors/CompilerError.hpp"
#include "../../domain/errors/ErrorList.hpp"

#include "../../application/use_cases/CompileSourceFile.hpp"
#include "../../domain/entities/Token.hpp"
#include "../../infrastructure/common/TokenResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include <vector>

static void logTokens(const std::vector<Token>& tokens)
{
	std::cout << "---- Token list (" << tokens.size() << " tokens) ----\n";

	for (std::vector<Token>::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		const Token& token = *it;

		std::cout << token.toString() << '\n';
	}

	std::cout << "---- End of tokens ----\n";
}

int main()
{
	LexerResult res = CompileSourceFile::execute("config.conf");
	Logger		logger;

	logger.enableFileLogging();

	if (res.isErr())
	{
		const ErrorList& errors = res.error();
		errors.formatAllErrors();
	}
	else
		logger.log("Source file loaded successfully. Lexer is ready for tokenization.", INFO);
	// Momento de adicionar um logger? se pá

	Lexer* lexer = res.unwrap();
	lexer->internalTest();
	TokenResult tokenRes = lexer->tokenize();
	if (tokenRes.isErr())
	{
		const ErrorList& errors = tokenRes.error();
		errors.formatAllErrors();
	}
	else
		logger.log("Tokenization successful. Tokens are ready for parsing.", INFO);
	std::vector<Token>* tokens = tokenRes.unwrap();
	std::stringstream	ss;
	ss << "Tokens generated: " << tokens->size();
	logger.log(ss.str().c_str(), INFO);
	logTokens(*tokens);
	return (0);
}

int LexerResultTest()
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
	return (42); // sp
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
