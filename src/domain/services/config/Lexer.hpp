/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 19:42:33 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:44:47 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXICAL_ANALYZER_HPP
#define LEXICAL_ANALYZER_HPP

#include <string>
#include <vector>

#include "../../../../includes/macros.hpp"
#include "../../../infrastructure/common/config/TokenResult.hpp"
#include "../../entities/common/SourceLocation.hpp"

#include "../../entities/config/Token.hpp"
#include "../../errors/common/ErrorList.hpp"

class Lexer
{
	private:
		std::string _content;
		std::string _filePath;
		size_t		_pos;
		size_t		_line;
		size_t		_column;
		ErrorList	_errorList;

		// ------------------------ NAVIGATION ------------------------ //
		char peek() const;
		char advance();
		bool isAtEnd() const;
		void skipWhitespace();
		void skipComment();

		// ---------------------- CLASSIFICATION ---------------------- //
		static bool isDigit(char c);
		static bool isAlpha(char c);
		static bool isPathChar(char c);

		// ----------------------- SCANNERS --------------------------- //
		Token scanWord();
		Token scanNumber();
		Token scanString();
		Token scanPath();
		Token scanSingle(TokenType type);
		Token scanUnknown();

		// ----------------------- HELPERS ---------------------------- //
		SourceLocation currentLocation() const;
		void		   addError(const CompilerError& error);

		static Token makeToken(TokenType type, const SourceLocation& location, const std::string& value);

	public:
		Lexer(const std::string& fileContent, const std::string& path);
		~Lexer();

		TokenResult tokenize();
		void		internalTest(); // Método para testes internos

		DISABLE_COPY(Lexer);
};

#endif
