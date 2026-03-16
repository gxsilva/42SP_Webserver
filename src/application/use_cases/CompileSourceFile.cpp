/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompileSourceFile.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 16:37:11 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:56:51 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <string>

#include "../../domain/errors/common/CompilerError.hpp"
#include "../../domain/errors/common/ErrorList.hpp"
#include "../../domain/services/config/Lexer.hpp"
#include "../../infrastructure/common/config/LexerResult.hpp"
#include "../../infrastructure/common/config/TokenResult.hpp"
#include "../../infrastructure/io/config/FileReader.hpp"
#include "../../infrastructure/io/config/FileValidator.hpp"
#include "../ports/ILogger.hpp"
#include "CompileSourceFile.hpp"

// ------------------------ COMPILE_SOURCE_FILE.CPP ------------------------ //

// ------------------------ OCCF ------------------------ //
CompileSourceFile::CompileSourceFile() {}
CompileSourceFile::~CompileSourceFile() {}

// ------------------------ METHODS ------------------------ //

LexerResult CompileSourceFile::loadSourceFile(const std::string& filepath)
{
	CompilerError* validateErr = FileValidator::validateFile(filepath);
	if (validateErr != NULL)
	{
		const LexerResult result(*validateErr);
		delete validateErr;
		return result;
	}

	std::string fileContent;
	const bool	readSuccess = FileReader::readFile(filepath, fileContent);
	if (!readSuccess)
		return LexerResult(CompilerError::ioError(filepath));
	Lexer* lexer = new Lexer(fileContent, filepath);
	return LexerResult(lexer);
}

TokenResult CompileSourceFile::execute(const std::string& filePath, ILogger* logger)
{
	LexerResult lexerRes = CompileSourceFile::loadSourceFile(filePath);
	if (lexerRes.isErr())
	{
		const ErrorList& errors = lexerRes.error();
		logger->log("Failed to load source file: " + filePath, ERROR);
		return TokenResult(errors);
	}
	Lexer* lexer = lexerRes.unwrap();
	logger->log("Successfully loaded source file: " + filePath, INFO);
	const TokenResult tokenRes = lexer->tokenize();
	//! DEBUG lexer->internalTest();
	if (tokenRes.isErr())
	{
		const ErrorList& errors = tokenRes.error();
		logger->log("Failed to tokenize source file: " + filePath, ERROR);
		return TokenResult(errors);
	}
	logger->log("Successfully tokenized source file: " + filePath, INFO);
	return (tokenRes);
}
