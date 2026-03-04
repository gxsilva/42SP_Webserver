/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 23:30:48 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:30:31 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "../../infrastructure/common/ASTResult.hpp"
#include "../entities/ast/node/ASTDirective.hpp"
#include "../value_objects/ASTValueType.hpp"

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

void Parser::_addError(const CompilerError& error) { _errors.addError(error); }

// ------------------------ PARSE METHODS ------------------------ //

ASTValue* Parser::parseValue()
{
	if (!_isValueToken(_peek().type))
	{
		_addError(CompilerError::expectedValueError(
			"Expected a value (identifier, number, string, or path)", _peek().location));
		return (NULL);
	}
	Token		 valueToken = _advance();
	ASTValueType valueType	= _convertTokenTypeToAstValue(valueToken.type);
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

	Token		   identifierToken = _advance();
	std::string	   directiveName   = identifierToken.value;
	SourceLocation loc			   = identifierToken.location;

	std::vector<ASTValue*> parameters;

	while (_isValueToken(_peek().type))
	{
		ASTValue* valueNode = parseValue();
		if (valueNode)
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
			if (child)
				blockNode->addChild(child);
			else
				_synchronize();
		}

		if (_check(RBRACE))
			_advance(); // Consome o RBRACE
		else
		{
			std::string	  expectedTokens = "'}' to close block";
			CompilerError error =
				CompilerError::expectedRightBraceError(expectedTokens, _peek().location);
			_addError(error);
			_synchronize();
		}

		return blockNode;
	}
	else if (_check(SEMICOLON))
	{
		ASTDirective* directiveNode = new ASTDirective(directiveName, loc);

		for (size_t i = 0; i < parameters.size(); ++i)
			directiveNode->addValue(parameters[i]);

		_advance(); // Consome o SEMICOLON
		return directiveNode;
	}
	else
	{
		std::string	  expectedTokens = "'{' for block or ';' for directive";
		CompilerError error = CompilerError::unepxectedTokenError(expectedTokens, _peek().location);
		_addError(error);

		_synchronize();
		return (NULL);
	}
}

// ------------------------  ------------------------ //

ASTRoot* Parser::parseConfig()
{
	ASTRoot* root = new ASTRoot();
	while (!_isAtEnd())
	{
		ASTNode* statement = parseStatement();
		if (statement)
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

bool Parser::_isValueToken(TokenType type) const
{
	return type == WORD || type == NUMBER || type == STRING || type == PATH;
}

ASTValueType Parser::_convertTokenTypeToAstValue(TokenType type) const
{
	switch (type)
	{
	case WORD:
		return AST_VALUE_IDENTIFIER;
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
