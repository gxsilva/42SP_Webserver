/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:24:34 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 21:39:34 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "../entities/Token.hpp"
#include "../errors/ErrorList.hpp"
#include <vector>

/*
AST_NODETYPE_VALUE = 0
AST_NODETYPE_DIRECTIVE,
AST_NODETYPE_BLOCK,
AST_NODETYPE_ROOT,


VALUE -> ASTValueType(STRING, IDENTIFIER, NUMBER, PATH
DIRECTIVE -> IDENTIFIER + VALUE
CONFIG -> DIRECTIVE | BLOCK
BLOCKK -> IDENTIFIER + "{" + CONFIG + "}"
CONFIG -> DIRECTIVE | BLOCK

AST_ROOT
  AST_DIRECTIVE
	std::string name = client_max_body_size
	std::vector<AST_VALUE*> values
	  values[0] = AST_VALUE
		 ASTValueType = VALUE_NUMBER
		 std::string value = 80
ASTBlock
   std::string name = server
   std::vector<ASTValue*> parameters
	 values[0] = AST_VALUE
		   ASTValueType = VALUE_IDENTIFIER
		   std::string value = server
   std::vector<ASTNode*> children
*/

class Parser
{
	private:
		std::vector<Token>* _tokens;
		size_t				_current;
		ErrorList			_errors;

		Token _peek() const;
		Token _advance();

	public:
		Parser(std::vector<Token>* tokens);
		~Parser();

		void parse();
};

Parser::Parser(std::vector<Token>* tokens) : _tokens(tokens), _current(0) {}

Parser::~Parser() {}

Token Parser::_peek() const
{
	if (_current < _tokens->size())
		return _tokens->at(_current);
	return Token();
}

Token Parser::_advance()
{
	if (_current < _tokens->size())
		return _tokens->at(_current++);
	return Token();
}

void Parser::parse()
{
	while (_peek().type != EOF_TOKEN)
	{
		Token currentToken = _peek();
		std::cout << "Parsing token: " << currentToken.toString() << std::endl;
		_advance();
	}
}

#endif /* PARSER_HPP */
