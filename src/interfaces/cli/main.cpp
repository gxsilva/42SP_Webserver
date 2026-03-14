/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/06 00:11:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
#include "../../domain/errors/ErrorList.hpp"

#include "../../application/use_cases/ParseConfigUseCase.hpp"
#include "../../domain/entities/Token.hpp"
#include "../../domain/entities/ast/base/ASTNode.hpp"
#include "../../infrastructure/common/ASTResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

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

	ASTResult astRes = ParseConfigUseCase::execute(argv[1], &logger);

	if (astRes.isErr())
	{
		const ErrorList& errors = astRes.error();
		errors.formatAllErrors();
		logger.log("Failed to compile config file: " + std::string(argv[1]), ERROR);
		return (1);
	}
	ASTNode* astRoot = astRes.release();
	// Debugger::logAST(astRoot);
	delete astRoot;
	return (0);
}
