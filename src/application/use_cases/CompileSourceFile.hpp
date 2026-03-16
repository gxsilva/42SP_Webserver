/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompileSourceFile.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 20:47:16 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:45:23 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPILE_SOURCE_FILE_HPP
#define COMPILE_SOURCE_FILE_HPP

#include <string>

#include "../../../includes/macros.hpp"
#include "../../infrastructure/common/config/LexerResult.hpp"
#include "../../infrastructure/common/config/TokenResult.hpp"
#include "../ports/ILogger.hpp"

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
		static TokenResult execute(const std::string& filePath, ILogger* logger);
};

#endif /* COMPILE_SOURCE_FILE_HPP */
