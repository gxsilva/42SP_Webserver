/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CardinalityRuleService.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 01:14:49 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 02:53:36 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CardinalityRuleService.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../entities/ast/node/ASTBlock.hpp"
#include "../../entities/ast/node/ASTDirective.hpp"
#include "../../entities/ast/node/ASTRoot.hpp"
#include "../../errors/CompilerError.hpp"
#include "../../value_objects/ASTNodeType.hpp"

namespace
{
	bool getNodeName(const ASTNode& node, std::string& name)
	{
		if (node.getType() == AST_NODETYPE_BLOCK)
		{
			const ASTBlock& blockNode = static_cast< const ASTBlock& >(node);
			name					  = blockNode.getName();
			return true;
		}
		if (node.getType() == AST_NODETYPE_DIRECTIVE)
		{
			const ASTDirective& directiveNode = static_cast< const ASTDirective& >(node);
			name							  = directiveNode.getName();
			return true;
		}
		return false;
	}

	void collectChildren(const ASTNode& node, std::vector< ASTNode* >& children)
	{
		if (node.getType() == AST_NODETYPE_BLOCK)
		{
			const ASTBlock&				   blockNode	 = static_cast< const ASTBlock& >(node);
			const std::vector< ASTNode* >& blockChildren = blockNode.getChildren();
			children.insert(children.end(), blockChildren.begin(), blockChildren.end());
			return;
		}
		if (node.getType() == AST_NODETYPE_ROOT)
		{
			const ASTRoot&				  rootNode	   = static_cast< const ASTRoot& >(node);
			const std::vector< ASTNode* > rootChildren = rootNode.getStatements();
			children.insert(children.end(), rootChildren.begin(), rootChildren.end());
		}
	}

	std::string resolveContextName(const ASTNode& node, const std::string& fallback)
	{
		if (node.getType() == AST_NODETYPE_BLOCK)
		{
			const ASTBlock& blockNode = static_cast< const ASTBlock& >(node);
			return blockNode.getName();
		}
		if (node.getType() == AST_NODETYPE_ROOT)
			return "root";
		return fallback;
	}
} // namespace

CardinalityRuleService::CardinalityRuleService(const RuleTable& table) : _table(table) {}

CardinalityRuleService::~CardinalityRuleService() {}

void CardinalityRuleService::apply(const ASTNode& node, const std::string& context,
								   ErrorList& errors)
{
	std::vector< ASTNode* >					children;
	std::map< std::string, int >			counts;
	std::map< std::string, SourceLocation > firstLocations;
	std::set< std::string >					ruleNames;
	const std::string						contextName = resolveContextName(node, context);

	collectChildren(node, children);
	ruleNames = this->_table.getCardinalityRuleNames();
	for (size_t i = 0; i < children.size(); ++i)
	{
		std::string childName;
		if (!getNodeName(*children[i], childName))
			continue;
		++counts[childName];
		if (firstLocations.find(childName) == firstLocations.end())
			firstLocations[childName] = children[i]->getLocation();
	}
	for (std::set< std::string >::const_iterator it = ruleNames.begin(); it != ruleNames.end();
		 ++it)
	{
		std::map< std::string, int >::const_iterator countIt  = counts.find(*it);
		int											 count	  = 0;
		int											 min	  = 0;
		int											 max	  = 0;
		SourceLocation								 location = node.getLocation();

		if (countIt != counts.end())
			count = countIt->second;
		if (!this->_table.getCardinality(*it, min, max))
			continue;
		if (firstLocations.find(*it) != firstLocations.end())
			location = firstLocations[*it];
		if (!this->_table.checkCardinality(*it, count))
		{
			errors.addError(CompilerError::directiveCardinalityError(*it, contextName, min, max,
																	 count, location));
		}
	}
}
