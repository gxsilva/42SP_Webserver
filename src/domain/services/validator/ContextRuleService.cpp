/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContextRuleServiceService.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 23:41:47 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:46:50 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ContextRuleService.hpp"

#include "../../entities/ast/node/ASTBlock.hpp"
#include "../../entities/ast/node/ASTDirective.hpp"
#include "../../value_objects/ASTNodeType.hpp"

ContextRuleService::ContextRuleService(const RuleTable& table) : _table(table) {}

ContextRuleService::~ContextRuleService() {}

void ContextRuleService::apply(const ASTNode& node, const std::string& context, ErrorList& errors)
{
	if (node.getType() == AST_NODETYPE_BLOCK)
	{
		ASTBlock& blockNode = static_cast< ASTBlock& >(const_cast< ASTNode& >(node));
		if (!this->_table.isAllowedInContext(blockNode.getName(), context))
		{
			errors.addError(CompilerError::directiveNotAllowedInContextError(blockNode.getName(), context,
																			 blockNode.getLocation()));
		}
	}
	else if (node.getType() == AST_NODETYPE_DIRECTIVE)
	{
		ASTDirective& directiveNode = static_cast< ASTDirective& >(const_cast< ASTNode& >(node));
		if (!this->_table.isAllowedInContext(directiveNode.getName(), context))
		{
			errors.addError(CompilerError::directiveNotAllowedInContextError(directiveNode.getName(), context,
																			 directiveNode.getLocation()));
		}
	}
	else
		return;
}
