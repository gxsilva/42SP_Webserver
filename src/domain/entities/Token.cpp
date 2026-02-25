/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 21:07:39 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 00:24:52 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

#include "../value_objects/TokenType.hpp"

#include "SourceLocation.hpp"
#include "Token.hpp"

// ------------------------ TOKEN IMP ------------------------ //

// ------------------------ OCCF ------------------------ //
Token::Token() : type(UNKNOWN) {}

Token::Token(TokenType t, const std::string& v, const SourceLocation& loc)
	: type(t), value(v), location(loc)
{
}

std::string Token::toString() const
{
	std::ostringstream ss;
	const char*		   typeStr = Token::_tokenTypeToString(type);

	ss << "Token(" << typeStr << ", \"" << value << "\", " << location.toString() << ")";
	return ss.str();
}

// ------------------------ METHODS ------------------------ //

const char* Token::_tokenTypeToString(TokenType type)
{
	if (type == WORD)
		return "WORD";
	if (type == NUMBER)
		return "NUMBER";
	if (type == STRING)
		return "STRING";
	if (type == PATH)
		return "PATH";
	if (type == LBRACE)
		return "LBRACE";
	if (type == RBRACE)
		return "RBRACE";
	if (type == SEMICOLON)
		return "SEMICOLON";
	if (type == SLASH)
		return "SLASH";
	if (type == NEWLINE)
		return "NEWLINE";
	if (type == EOF_TOKEN)
		return "EOF_TOKEN";
	if (type == UNKNOWN)
		return "UNKNOWN";
	return "INVALID_TOKEN_TYPE";
}
