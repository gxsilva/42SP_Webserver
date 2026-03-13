/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SemanticAnalyzer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 03:00:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 03:11:12 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SemanticAnalyzer.hpp"

#include <string>
#include <vector>

#include "../../domain/entities/ast/node/ASTBlock.hpp"
#include "../../domain/entities/ast/node/ASTRoot.hpp"
#include "../../domain/value_objects/ASTNodeType.hpp"

// ------------------------ OCCF ------------------------ //

SemanticAnalyzer::SemanticAnalyzer(const std::vector< ISemanticRule* >& rules) : _rules(rules) {}

SemanticAnalyzer::~SemanticAnalyzer() {}

// ------------------------ PRIVATE ------------------------ //

void SemanticAnalyzer::_visit(const ASTNode& node, const std::string& context, ErrorList& errors) const
{
	for (size_t i = 0; i < _rules.size(); ++i)
		_rules[i]->apply(node, context, errors);

	if (node.getType() == AST_NODETYPE_BLOCK)
	{
		const ASTBlock&				   block	= static_cast< const ASTBlock& >(node);
		const std::vector< ASTNode* >& children = block.getChildren();

		for (size_t i = 0; i < children.size(); ++i)
			_visit(*children[i], block.getName(), errors);
	}
	else if (node.getType() == AST_NODETYPE_ROOT)
	{
		const ASTRoot&				  root		 = static_cast< const ASTRoot& >(node);
		const std::vector< ASTNode* > statements = root.getStatements();

		for (size_t i = 0; i < statements.size(); ++i)
			_visit(*statements[i], "root", errors);
	}
}

// ------------------------ PUBLIC ------------------------ //

void SemanticAnalyzer::analyze(const ASTNode& root, ErrorList& errors) const { _visit(root, "", errors); }
