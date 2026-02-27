/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompileSourceFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 20:47:16 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:32:22 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPILE_SOURCE_FILE_HPP
#define COMPILE_SOURCE_FILE_HPP

#include <string>

#include "../../domain/errors/CompilerError.hpp"
#include "../../domain/services/Lexer.hpp"
#include "../../infrastructure/common/LexerResult.hpp"
#include "../../infrastructure/io/FileReader.hpp"
#include "../../infrastructure/io/FileValidator.hpp"

#include "../../../includes/macros.hpp"

/*
	orquestão inical dessa bomba
	CompileSourceFile::execute(filePath) -> CompileSourceFile::loadSourceFile
	LexerResult = CompileSourceFile::loadSourceFile [FileValidator::validateFile(),
   FileReader::readFile()]; LexerResult.unwrap() -> Lexer* -> Lexer.tokenize() -> TokenResult
*/

class CompileSourceFile
{
	private:
		CompileSourceFile();
		~CompileSourceFile();

		static LexerResult loadSourceFile(const std::string& filepath);

		DISABLE_COPY(CompileSourceFile);

	public:
		static LexerResult execute(const std::string& filePath);
};

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

#endif /* COMPILE_SOURCE_FILE_HPP */
