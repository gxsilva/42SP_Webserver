/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompileSourceFile.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 16:37:11 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 16:37:56 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CompileSourceFile.hpp"

#include "../../domain/errors/CompilerError.hpp"
#include "../../domain/services/Lexer.hpp"
#include "../../infrastructure/io/FileReader.hpp"
#include "../../infrastructure/io/FileValidator.hpp"

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
		LexerResult result(*validateErr);
		delete validateErr;
		return result;
	}

	std::string fileContent;
	bool		readSuccess = FileReader::readFile(filepath, fileContent);
	if (!readSuccess)
		return LexerResult(CompilerError::ioError(filepath));
	Lexer* lexer = new Lexer(fileContent, filepath);
	return LexerResult(lexer);
}

// This will return TokenResult in the future, but for now it will return a
//   LexerResult since we haven't implemented the tokenization process yet k k k
LexerResult CompileSourceFile::execute(const std::string& filePath)
{
	return CompileSourceFile::loadSourceFile(filePath);
}
