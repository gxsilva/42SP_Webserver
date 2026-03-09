/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:24:34 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/09 19:12:25 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "../entities/Token.hpp"
#include "../errors/ErrorList.hpp"
#include "../value_objects/TokenType.hpp"

#include "../entities/ast/base/ASTNode.hpp"
#include "../entities/ast/node/ASTBlock.hpp"
#include "../entities/ast/node/ASTRoot.hpp"
#include "../entities/ast/node/ASTValue.hpp"

#include "../../infrastructure/common/ASTResult.hpp"

#include <vector>

/*
AST_NODETYPE_VALUE = 0
AST_NODETYPE_DIRECTIVE,
AST_NODETYPE_BLOCK,
AST_NODETYPE_ROOT,


VALUE -> ASTValueType(STRING, indentationIFIER, NUMBER, PATH
DIRECTIVE -> indentationIFIER + VALUE
CONFIG -> DIRECTIVE | BLOCK
BLOCKK -> indentationIFIER + "{" + CONFIG + "}"
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
		   ASTValueType = VALUE_indentationIFIER
		   std::string value = server
   std::vector<ASTNode*> children
*/

class Parser
{
	private:
		std::vector<Token>* _tokens;
		size_t				_current;
		ErrorList			_errors;
		ASTRoot*			_astRoot;

		Token _peek() const;
		Token _advance();

		bool _match(TokenType expected);
		bool _isAtEnd() const;
		bool _check(TokenType expected) const;

		void _synchronize();

		void _addError(const CompilerError& error);

		ASTValue* parseValue();
		ASTNode*  parseStatement();
		ASTRoot*  parseConfig();

		static bool			_isValueToken(TokenType type);
		static ASTValueType _convertTokenTypeToAstValue(TokenType type);

	public:
		Parser(std::vector<Token>* tokens);
		~Parser();

		ASTResult parser();

		ErrorList getErrors() const;
		bool	  hasErrors() const;
};

#endif /* PARSER_HPP */
