/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 23:30:48 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 23:31:29 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

bool Parser::_isValueToken(TokenType type) const
{
	return type == WORD || type == NUMBER || type == STRING || type == PATH;
}

ASTValueType _convertTokenTypeToAstValue(TokenType type) const;

ASTNode* Parser::parseStatement()
{
	if (!_check(WORD))
	{
		addError(
			CompilerError::expectedNameError("Expected directive or block name", _peek().location));
		_synchronize();
		return (NULL);
	}

	Token		   identifierToken = _advance();
	std::string	   directiveName   = identifierToken.value;
	SourceLocation loc			   = identifierToken.location;

	std::vector<ASTValue*> parameters;

	while (_isValueToken(_peek().type))
	{
		Token		 valueToken = _advance();
		ASTValueType valueType;

		switch (valueToken.type)
		{
		case WORD:
			valueType = ASTValueType::VALUE_IDENTIFIER;
			break;
		case NUMBER:
			valueType = ASTValueType::VALUE_NUMBER;
			break;
		case STRING:
			valueType = ASTValueType::VALUE_STRING;
			break;
		case PATH:
			valueType = ASTValueType::VALUE_PATH;
			break;
		default:
			addError(
				CompilerError::unregonizedCharacterError(valueToken.location, valueToken.value[0]));
			continue;
		}

		parameters.push_back(new ASTValue(valueType, valueToken.value, valueToken.location));
	}

	if (_check(LBRACE))
	{
		return parseBlock(directiveName, loc);
	}
	else
	{
		return parseDirective(directiveName, loc);
	}
}

void Parser::_synchronize()
{
	while (!_isAtEnd())
	{
		if (_peek().type == SEMICOLON || _peek().type == RBRACE || _peek().type == WORD)
		{
			_advance();
			return;
		}
		_advance();
	}
}

ASTRoot* Parser::parseConfig()
{
	ASTRoot* root = new ASTRoot();
	while (!_isAtEnd())
	{
		ASTNode* statement = parseStatement();
	}
}

ASTValue* Parser::parseValue()
{
	if (!_isValueToken(_peek().type))
	{
		addError(CompilerError::expectedValueError(
			"Expected a value (identifier, number, string, or path)", _peek().location));
		return (NULL);
	}
	Token		 valueToken = _advance();
	ASTValueType valueType	= _convertTokenTypeToAstValue(valueToken.type);
}

bool Parser::_isAtEnd() const { return _peek().type == EOF_TOKEN; }

bool Parser::_check(TokenType expected) const
{
	if (_isAtEnd())
		return false;
	return _peek().type == expected;
}

// ------------------------ OCCF ------------------------ //
Parser::Parser(std::vector<Token>* tokens) : _tokens(tokens), _current(0) {}

Parser::~Parser() {}

Token Parser::_peek() const
{
	if (_current < _tokens->size())
		return _tokens->at(_current);
	return Token();
}

// ------------------------ NAVIGATION ------------------------ //

Token Parser::_advance()
{
	if (_current < _tokens->size())
		return _tokens->at(_current++);
	return Token();
}

bool Parser::_match(TokenType expected)
{
	if (_peek().type == expected)
	{
		_advance();
		return true;
	}
	return false;
}

// ------------------------ ERROR METHODS ------------------------ //
ErrorList Parser::getErrors() const { return _errors; }

void Parser::addError(const CompilerError& error) { _errors.addError(error); }

// ------------------------ PARSE METHODS ------------------------ //

// ------------------------ MAIN METHODS ------------------------ //
void Parser::parse()
{
	while (_peek().type != EOF_TOKEN)
	{
		Token currentToken = _peek();
		std::cout << "Parsing token: " << currentToken.toString() << std::endl;
		_advance();
	}
}

// que bagunça
