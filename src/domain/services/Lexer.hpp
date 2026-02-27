/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 19:42:33 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:40:21 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXICAL_ANALYZER_HPP
#define LEXICAL_ANALYZER_HPP

#include <string>
#include <vector>

#include "../../../includes/macros.hpp"
#include "../../infrastructure/common/TokenResult.hpp"
#include "../entities/SourceLocation.hpp"

#include "../entities/Token.hpp"
#include "../errors/ErrorList.hpp"

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
		bool isDigit(char c) const;
		bool isAlpha(char c) const;
		bool isPathChar(char c) const;

		// ----------------------- SCANNERS --------------------------- //
		Token scanWord();
		Token scanNumber();
		Token scanString();
		Token scanPath();
		Token scanSingle(TokenType type);
		Token scanUnknown();

		// ----------------------- HELPERS ---------------------------- //
		SourceLocation currentLocation() const;
		Token makeToken(TokenType type, SourceLocation location, const std::string& value) const;
		void  addError(const CompilerError& error);

	public:
		Lexer(const std::string& fileContent, const std::string& path);
		~Lexer();

		TokenResult tokenize();
		void		internalTest(); // Método para testes internos

		DISABLE_COPY(Lexer);
};

#endif
