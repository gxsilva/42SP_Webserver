/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   old_mainv2.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 16:06:40 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "../../domain/errors/common/ErrorList.hpp"

#include "../../application/use_cases/CompileSourceFile.hpp"
#include "../../domain/entities/config/Token.hpp"
#include "../../domain/entities/server/HttpBlock.hpp"
#include "../../domain/services/config/Parser.hpp"

#include "../../infrastructure/common/config/ASTResult.hpp"
#include "../../infrastructure/common/config/TokenResult.hpp"
#include "../../infrastructure/common/config/ValidatorResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include "../../infrastructure/common/config/RuleRegistry.hpp"

#include "../../domain/services/config/Validator.hpp"

#include "../../domain/services/validator/CardinalityRuleService.hpp"
#include "../../domain/services/validator/ConflictRuleService.hpp"
#include "../../domain/services/validator/ContextRuleService.hpp"
#include "../../domain/services/validator/DependencyRuleService.hpp"
#include "../../domain/services/validator/ValueRuleService.hpp"
#include "../../infrastructure/io/config/SemanticAnalyzer.hpp"

#include "../../domain/services/config/ConfigBuilder.hpp"

#include <vector>

class Debugger
{
	public:
		static void logTokens(const std::vector< Token >& tokens)
		{
			std::cout << "---- Token list (" << tokens.size() << " tokens) ----\n";

			for (std::vector< Token >::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
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

		static void logHttpBlock(const HttpBlock* block, int indent = 0)
		{
			if (!block)
				return;

			std::string prefix(indent * 2, ' ');
			std::cout << prefix << "---- HttpBlock ----\n";
			std::cout << prefix << "clientMaxBodySize: " << block->clientMaxBodySize << '\n';
			std::cout << prefix << "errorPages: ";
			for (std::map< int, std::string >::const_iterator it = block->errorPages.begin();
				 it != block->errorPages.end(); ++it)
			{
				std::cout << "[" << it->first << ": " << it->second << "] ";
			}
			std::cout << '\n';

			std::cout << prefix << "server:\n";
			logServerBlock(&(block->server), indent + 1);

			std::cout << prefix << "---- End of HttpBlock ----\n";
		}

		static void logLocationBlock(const LocationBlock* block, int indent = 0)
		{
			if (!block)
				return;

			std::string prefix(indent * 2, ' ');
			std::cout << prefix << "---- LocationBlock ----\n";
			std::cout << prefix << "path: " << block->path << '\n';
			std::cout << prefix << "root: " << block->root << '\n';
			std::cout << prefix << "allowedMethods: ";
			for (std::set< std::string >::const_iterator it = block->allowedMethods.begin();
				 it != block->allowedMethods.end(); ++it)
			{
				std::cout << *it << " ";
			}
			std::cout << '\n';
			std::cout << prefix << "autoindex: " << (block->autoindex ? "true" : "false") << '\n';
			std::cout << prefix << "errorPages: ";
			for (std::map< int, std::string >::const_iterator it = block->errorPages.begin();
				 it != block->errorPages.end(); ++it)
			{
				std::cout << "[" << it->first << ": " << it->second << "] ";
			}
			std::cout << '\n';
			std::cout << prefix << "---- End of LocationBlock ----\n";
		}

		static void logServerBlock(const ServerBlock* block, int indent = 0)
		{
			if (!block)
				return;

			std::string prefix(indent * 2, ' ');
			std::cout << prefix << "---- ServerBlock ----\n";
			std::cout << prefix << "port: " << block->port << '\n';
			std::cout << prefix << "serverName: " << block->serverName << '\n';
			std::cout << prefix << "root: " << block->root << '\n';
			std::cout << prefix << "index: ";
			for (std::vector< std::string >::const_iterator it = block->index.begin(); it != block->index.end(); ++it)
			{
				std::cout << *it << " ";
			}
			std::cout << '\n';
			std::cout << prefix << "clientMaxBodySize: " << block->clientMaxBodySize << '\n';
			std::cout << prefix << "errorPages: ";
			for (std::map< int, std::string >::const_iterator it = block->errorPages.begin();
				 it != block->errorPages.end(); ++it)
			{
				std::cout << "[" << it->first << ": " << it->second << "] ";
			}
			std::cout << '\n';
			std::cout << prefix << "locations:\n";
			for (std::vector< LocationBlock >::const_iterator it = block->locations.begin();
				 it != block->locations.end(); ++it)
			{
				logLocationBlock(&(*it), indent + 1);
			}
			std::cout << prefix << "---- End of ServerBlock ----\n";
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

	std::vector< Token >* tokens = res.unwrap();

	// ------------------------ DEVELOPMENT ------------------------ //
	/// std::vector<Token>* tokens -> use_case -> parser -> ASTNode* astRoot -> validate AST ->
	/// generate config object -> class Config

	// Debugger::logTokens(*tokens);
	ASTResult astRes = Parser(tokens).parser();

	if (astRes.isErr())
	{
		const ErrorList& errors = astRes.error();
		errors.formatAllErrors();
		delete tokens;
		logger.log("Failed to parse tokens from source file: " + std::string(argv[1]), ERROR);
		return (1);
	}
	logger.log("Successfully parsed tokens from source file: " + std::string(argv[1]), INFO);
	ASTNode* astRoot = astRes.unwrap();
	delete tokens;
	// Debugger::logAST(astRoot);

	RuleRegistry		   ruleRegistry;
	ContextRuleService	   contextRule(ruleRegistry.getContextTable());
	CardinalityRuleService cardinalityRule(ruleRegistry.getCardinalityTable(), ruleRegistry.getContextTable());
	ConflictRuleService	   conflictRule(ruleRegistry.getConflictTable());
	DependencyRuleService  dependencyRule(ruleRegistry.getDependencyTable());
	ValueRuleService	   valueRule;

	std::vector< ISemanticRule* > rules;
	rules.push_back(&contextRule);
	rules.push_back(&cardinalityRule);
	rules.push_back(&conflictRule);
	rules.push_back(&dependencyRule);
	rules.push_back(&valueRule);

	SemanticAnalyzer analyzer(rules);
	Validator		 validator(analyzer);
	ValidatorResult	 validationResult = validator.validate(*astRoot);

	if (validationResult.isErr())
	{
		const ErrorList& errors = validationResult.error();
		errors.formatAllErrors();
		delete astRoot;
		logger.log("Validation failed for AST from source file: " + std::string(argv[1]), ERROR);
		return (1);
	}
	logger.log("Successfully validated AST from source file: " + std::string(argv[1]), INFO);

	HttpBlock* config = ConfigBuilder().build(astRoot);

	//config->server.port;
	//ip na config

	logger.log("Successfully built configuration from AST for source file: " + std::string(argv[1]), INFO);
	// Debugger::logHttpBlock(config);
	delete config;
	delete astRoot;

	return (0);
}
