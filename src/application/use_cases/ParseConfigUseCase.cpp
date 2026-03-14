/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfigUseCase.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:09:39 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:39 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfigUseCase.hpp"

#include <string>
#include <vector>

#include "../../domain/entities/Token.hpp"
#include "../../domain/errors/ErrorList.hpp"
#include "../../domain/services/Parser.hpp"
#include "../../infrastructure/common/ASTResult.hpp"
#include "../../infrastructure/common/TokenResult.hpp"
#include "../ports/ILogger.hpp"
#include "CompileSourceFile.hpp"

// ------------------------ OCCF ------------------------ //
ParseConfigUseCase::ParseConfigUseCase() {}

ParseConfigUseCase::~ParseConfigUseCase() {}

// ------------------------ METHODS ------------------------ //

ASTResult ParseConfigUseCase::execute(const std::string& filePath, ILogger* logger)
{
	TokenResult tokenRes = CompileSourceFile::execute(filePath, logger);
	if (tokenRes.isErr())
		return ASTResult(tokenRes.error());

	std::vector<Token>* tokens = tokenRes.unwrap();
	ASTResult			astRes	= Parser(tokens).parser();
	delete tokens;

	if (astRes.isErr())
	{
		logger->log("Failed to parse config file: " + filePath, ERROR);
		return ASTResult(astRes.error());
	}
	logger->log("Successfully parsed config file: " + filePath, INFO);
	return ASTResult(astRes.release());
}
