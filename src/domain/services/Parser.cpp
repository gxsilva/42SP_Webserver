/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 23:30:48 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/09 19:12:25 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include "../entities/SourceLocation.hpp"
#include "../entities/Token.hpp"
#include "../value_objects/TokenType.hpp"

#include "../errors/CompilerError.hpp"
#include "../errors/ErrorList.hpp"

#include "../../infrastructure/common/ASTResult.hpp"

#include "../entities/ast/base/ASTNode.hpp"
#include "../entities/ast/node/ASTBlock.hpp"
#include "../entities/ast/node/ASTDirective.hpp"
#include "../entities/ast/node/ASTRoot.hpp"
#include "../value_objects/ASTValueType.hpp"

// ------------------------ OCCF ------------------------ //
Parser::Parser(std::vector< Token >* tokens) : _tokens(tokens), _current(0), _astRoot() {}

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

void Parser::_addError(const CompilerError& error) { _errors.addError(error); }

// ------------------------ PARSE METHODS ------------------------ //

ASTValue* Parser::parseValue()
{
	if (!_isValueToken(_peek().type))
	{
		_addError(CompilerError::expectedValueError(
			"Expected a value (indentationifier, number, string, or path)", _peek().location));
		return (NULL);
	}
	const Token		   valueToken = _advance();
	const ASTValueType valueType  = _convertTokenTypeToAstValue(valueToken.type);
	return new ASTValue(valueType, valueToken.value, valueToken.location);
}

// ------------------------  ------------------------ //
ASTNode* Parser::parseStatement()
{
	if (!_check(WORD))
	{
		_addError(
			CompilerError::expectedNameError("Expected directive or block name", _peek().location));
		_synchronize();
		return (NULL);
	}

	const Token			 indentationifierToken = _advance();
	const std::string	 directiveName		   = indentationifierToken.value;
	const SourceLocation loc				   = indentationifierToken.location;

	std::vector< ASTValue* > parameters;

	while (_isValueToken(_peek().type))
	{
		ASTValue* valueNode = parseValue();
		if (valueNode != NULL)
			parameters.push_back(valueNode);
		else
			_synchronize(); //?
	}

	// Agora, o próximo token deve ser ou um LBRACE (indicando um bloco) ou um SEMICOLON (indicando
	// uma diretiva)
	if (_check(LBRACE))
	{
		ASTBlock* blockNode = new ASTBlock(directiveName, loc);

		for (size_t i = 0; i < parameters.size(); ++i)
			blockNode->addParameter(parameters[i]);

		_advance(); // Consome o LBRACE

		while (!_check(RBRACE) && !_isAtEnd())
		{
			ASTNode* child = parseStatement();
			if (child != NULL)
				blockNode->addChild(child);
			else
				_synchronize();
		}

		if (_check(RBRACE))
			_advance(); // Consome o RBRACE
		else
		{
			const std::string	expectedTokens = "'}' to close block";
			const CompilerError error =
				CompilerError::expectedRightBraceError(expectedTokens, _peek().location);
			_addError(error);
			_synchronize();
		}

		return blockNode;
	}
	else if (!_check(SEMICOLON))
	{
		const std::string	expectedTokens = "'{' for block or ';' for directive";
		const CompilerError error =
			CompilerError::unepxectedTokenError(expectedTokens, _peek().location);
		_addError(error);

		_synchronize();
		return (NULL);
	}
	ASTDirective* directiveNode = new ASTDirective(directiveName, loc);

	for (size_t i = 0; i < parameters.size(); ++i)
		directiveNode->addValue(parameters[i]);

	_advance(); // Consome o SEMICOLON
	return directiveNode;
}

// ------------------------  ------------------------ //

ASTRoot* Parser::parseConfig()
{
	ASTRoot* root = new ASTRoot();
	while (!_isAtEnd())
	{
		ASTNode* statement = parseStatement();
		if (statement != NULL)
			root->addStatement(statement);
		else
			_synchronize();
	}
	return root;
}

// ------------------------ MAIN METHODS ------------------------ //
ASTResult Parser::parser()
{
	_astRoot = parseConfig();
	if (hasErrors())
	{
		delete _astRoot;
		_astRoot = NULL;
		return ASTResult(getErrors());
	}
	return ASTResult(_astRoot);
}

// ------------------------ UTILS ------------------------ //

bool Parser::_isValueToken(TokenType type)
{
	return type == WORD || type == NUMBER || type == STRING || type == PATH;
}

ASTValueType Parser::_convertTokenTypeToAstValue(TokenType type)
{
	switch (type)
	{
	case WORD:
		return AST_VALUE_indentationIFIER;
	case NUMBER:
		return AST_VALUE_NUMBER;
	case STRING:
		return AST_VALUE_STRING;
	case PATH:
		return AST_VALUE_PATH;
	default:
		return AST_VALUE_UNDEFINED;
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

bool Parser::_isAtEnd() const { return _peek().type == EOF_TOKEN; }

bool Parser::_check(TokenType expected) const
{
	if (_isAtEnd())
		return false;
	return _peek().type == expected;
}

bool Parser::hasErrors() const { return _errors.hasErrors(); }

// que bagunça
