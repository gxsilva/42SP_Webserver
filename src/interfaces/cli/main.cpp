/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/05 23:27:06 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
#include "../../domain/errors/ErrorList.hpp"

#include "../../application/use_cases/CompileSourceFile.hpp"
#include "../../domain/entities/Token.hpp"
#include "../../domain/services/Parser.hpp"
#include "../../infrastructure/common/ASTResult.hpp"
#include "../../infrastructure/common/TokenResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include <vector>

class Debugger
{
	public:
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

		static void logAST(const ASTNode* node, int indent = 0)
		{
			if (!node)
				return;
			std::cout << "---- AST Node ----\n";

			std::cout << std::string(indent * 2, ' ') << node->toString(indent);

			std::cout << "---- End of AST Node ----\n";
		}
};

int main(int argc, const char** argv)
{
	Logger logger;
	logger.enableFileLogging();

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>\n";
		logger.log("No configuration file provided. Exiting.", ERROR);
		return (1);
	}

	TokenResult res = CompileSourceFile::execute(argv[1], &logger);

	if (res.isErr())
	{
		const ErrorList& errors = res.error();
		errors.formatAllErrors();
		logger.log("Failed to compile source file: " + std::string(argv[1]), ERROR);
		return (1);
	}

	std::vector<Token>* tokens = res.unwrap();

	// ------------------------ DEVELOPMENT ------------------------ //
	/// std::vector<Token>* tokens -> use_case -> parser -> ASTNode* astRoot -> validate AST ->
	/// generate config object -> class Config

	// Debugger::logTokens(*tokens);
	ASTResult astRes = Parser(tokens).parser();

	if (astRes.isErr())
	{
		const ErrorList& errors = astRes.error();
		errors.formatAllErrors();
		logger.log("Failed to parse tokens from source file: " + std::string(argv[1]), ERROR);
		return (1);
	}
	logger.log("Successfully parsed tokens from source file: " + std::string(argv[1]), INFO);
	ASTNode* astRoot = astRes.unwrap();
	delete tokens;
	// Debugger::logAST(astRoot);
	(void)astRoot;
	// delete astRoot;
	return (0);
}
