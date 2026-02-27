/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 22:36:54 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:40:21 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

#include <cstddef> // for size_t
#include <string>

#include "../entities/SourceLocation.hpp"
#include "../entities/Token.hpp"
#include "../errors/CompilerError.hpp"
#include "../value_objects/TokenType.hpp"

#include "../../infrastructure/common/TokenResult.hpp"

/*
C++ 98 não permite usar Enum dentro de classes, então o TokenType é um enum global
Enum TokenType -> WORD, NUMBER
*/

// ------------------------ LEXER IMPLEMENTATION ------------------------ //

// ------------------------ OCCF ------------------------ //
Lexer::Lexer(const std::string& fileContent, const std::string& path)
	: _content(fileContent), _filePath(path), _pos(0), _line(1), _column(1)
{
}

Lexer::~Lexer() {}

// ------------------------ METHODS ------------------------ //

char Lexer::peek() const
{
	if (isAtEnd())
		return '\0';
	return _content[_pos];
}

// ------------------------ NAVIGATION ------------------------ //

char Lexer::advance()
{
	if (!isAtEnd())
	{
		char c = _content[_pos++];
		if (c == '\n')
		{
			_line++;
			_column = 0;
		}
		else
			_column++;
		return c;
	}
	return ('\0');
}
bool Lexer::isAtEnd() const { return _pos >= _content.size(); }

void Lexer::skipWhitespace()
{
	while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n'))
		advance();
}

void Lexer::skipComment()
{
	if (peek() == '#')
	{
		while (!isAtEnd() && peek() != '\n')
			advance();
	}
}

// ---------------------- CLASSIFICATION ---------------------- //

bool Lexer::isDigit(char c) const { return c >= '0' && c <= '9'; }

bool Lexer::isAlpha(char c) const
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/*
Example of valid paths:
- /tours
- ./docs
- -images
- _assets
*/
bool Lexer::isPathChar(char c) const
{
	return isAlpha(c) || isDigit(c) || c == '/' || c == '.' || c == '-' || c == '_';
}

// ----------------------- SCANNERS --------------------------- //
Token Lexer::scanWord()
{
	size_t		   start = _pos;
	SourceLocation loc	 = currentLocation();

	while (!isAtEnd() && (isAlpha(peek()) || isDigit(peek()) || peek() == '_'))
		advance();
	std::string value = _content.substr(start, _pos - start);
	return makeToken(WORD, loc, value);
}

Token Lexer::scanNumber()
{
	size_t		   start = _pos;
	SourceLocation loc	 = currentLocation();

	while (!isAtEnd() && isDigit(peek()))
		advance();
	std::string value = _content.substr(start, _pos - start);
	return makeToken(NUMBER, loc, value);
}

/*
- "hello world"
advance() ^ 2 -> pula o primeiro e o segundo " e começa a ler o conteúdo

Case 1
- "unterminated string

Case 2
- "valid string"

Case 3
- "terminated string with newline
					"
*/

Token Lexer::scanString()
{
	advance();
	size_t		   start = _pos;
	SourceLocation loc	 = currentLocation();

	while (!isAtEnd() && peek() != '"')
	{
		if (peek() == '\n')
			addError(CompilerError::unterminatedStringError(currentLocation()));
		advance();
	}
	if (isAtEnd())
	{
		addError(CompilerError::unterminatedStringError(currentLocation()));
		return makeToken(UNKNOWN, loc, "");
	}
	std::string value = _content.substr(start, _pos - start);
	advance(); // Skip closing quote
	return makeToken(STRING, loc, value);
}

Token Lexer::scanPath()
{
	size_t		   start = _pos;
	SourceLocation loc	 = currentLocation();

	while (!isAtEnd() && isPathChar(peek()))
		advance();
	std::string value = _content.substr(start, _pos - start);
	return makeToken(PATH, loc, value);
}

Token Lexer::scanSingle(TokenType type)
{
	SourceLocation loc = currentLocation();
	std::string	   value(1, peek());
	advance();
	return makeToken(type, loc, value);
}

Token Lexer::scanUnknown()
{
	SourceLocation loc = currentLocation();
	char		   c   = peek();
	addError(CompilerError::unregonizedCharacterError(loc, c));
	advance();
	return makeToken(UNKNOWN, loc, std::string(1, c));
}

// ----------------------- HELPERS ---------------------------- //
Token Lexer::makeToken(TokenType type, SourceLocation location, const std::string& value) const
{
	return Token(type, value, location);
}

void Lexer::addError(const CompilerError& error) { _errorList.addError(error); }

// ---------------------- PUBLIC METHODS ---------------------- //
TokenResult Lexer::tokenize()
{
	std::vector<Token> tokens;

	while (!isAtEnd())
	{
		skipWhitespace();
		skipComment();

		if (isAtEnd())
			break;

		char c = peek();
		if (c == '{')
			tokens.push_back(scanSingle(LBRACE));
		else if (c == '}')
			tokens.push_back(scanSingle(RBRACE));
		else if (c == ';')
			tokens.push_back(scanSingle(SEMICOLON));
		else if (isAlpha(c))
			tokens.push_back(scanWord());
		else if (isDigit(c))
			tokens.push_back(scanNumber());
		else if (c == '"')
			tokens.push_back(scanString());
		else if (isPathChar(c))
			tokens.push_back(scanPath());
		else
			tokens.push_back(scanUnknown());
	}
	if (!tokens.empty())
		tokens.push_back(makeToken(EOF_TOKEN, currentLocation(), ""));
	if (_errorList.hasErrors())
		return TokenResult(_errorList);

	return TokenResult(new std::vector<Token>(tokens));
}

// ------------------------ arrumar algum dia ------------------------ //
SourceLocation Lexer::currentLocation() const
{
	return SourceLocation(_filePath, _line, _column, 1);
}

void Lexer::internalTest()
{
	std::cout << "Lexer internal state:" << std::endl;
	std::cout << "  _filePath : " << _filePath << std::endl;
	std::cout << "  _content  : " << _content.size() << std::endl;
	std::cout << "  _pos      : " << _pos << std::endl;
	std::cout << "  _line     : " << _line << std::endl;
	std::cout << "  _column   : " << _column << std::endl;
	std::cout << "  _errorList size: " << _errorList.size() << std::endl;
}
