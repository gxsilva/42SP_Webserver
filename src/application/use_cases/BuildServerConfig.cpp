/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuildServerConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:02:48 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 20:33:46 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BuildServerConfig.hpp"

#include "./CompileSourceFile.hpp"

#include "../../domain/services/config/ConfigBuilder.hpp"
#include "../../domain/services/config/Parser.hpp"

#include "../../domain/services/config/Validator.hpp"
#include "../../domain/services/validator/CardinalityRuleService.hpp"
#include "../../domain/services/validator/ConflictRuleService.hpp"
#include "../../domain/services/validator/ContextRuleService.hpp"
#include "../../domain/services/validator/DependencyRuleService.hpp"
#include "../../domain/services/validator/ValueRuleService.hpp"
#include "../../infrastructure/common/config/RuleRegistry.hpp"
#include "../../infrastructure/common/config/TokenResult.hpp"

ConfigResult BuildServerConfig::execute(const std::string& filePath, Logger* logger)
{

	TokenResult tokenRes = CompileSourceFile::execute(filePath, logger);
	if (tokenRes.isErr())
		return ConfigResult(tokenRes.error());

	std::vector< Token >* tokens = tokenRes.unwrap();

	ASTResult astRes = Parser(tokens).parser();
	delete tokens;

	if (astRes.isErr())
	{
		logger->log("Failed to parse tokens from: " + filePath, ERROR);
		return ConfigResult(astRes.error());
	}

	logger->log("Successfully parsed: " + filePath, INFO);
	ASTNode* astRoot = astRes.unwrap();

	RuleRegistry		   registry;
	ContextRuleService	   contextRule(registry.getContextTable());
	CardinalityRuleService cardinalityRule(registry.getCardinalityTable(), registry.getContextTable());
	ConflictRuleService	   conflictRule(registry.getConflictTable());
	DependencyRuleService  dependencyRule(registry.getDependencyTable());
	ValueRuleService	   valueRule;

	std::vector< ISemanticRule* > rules;
	rules.push_back(&contextRule);
	rules.push_back(&cardinalityRule);
	rules.push_back(&conflictRule);
	rules.push_back(&dependencyRule);
	rules.push_back(&valueRule);

	SemanticAnalyzer analyzer(rules);
	Validator		 validator(analyzer);
	ValidatorResult	 validRes = validator.validate(*astRoot);

	if (validRes.isErr())
	{
		logger->log("Validation failed for: " + filePath, ERROR);
		delete astRoot;
		return ConfigResult(validRes.error());
	}

	logger->log("Successfully validated: " + filePath, INFO);

	HttpBlock* server = ConfigBuilder().build(astRoot);
	delete astRoot;

	logger->log("Successfully built config for: " + filePath, INFO);
	return ConfigResult(server);
}
