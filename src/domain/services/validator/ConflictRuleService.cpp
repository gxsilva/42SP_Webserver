/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConflictRuleService.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 01:14:55 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 02:53:45 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConflictRuleService.hpp"

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../../entities/ast/node/ASTBlock.hpp"
#include "../../entities/ast/node/ASTDirective.hpp"
#include "../../entities/ast/node/ASTRoot.hpp"
#include "../../errors/CompilerError.hpp"
#include "../../value_objects/ASTNodeType.hpp"

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

ConflictRuleService::ConflictRuleService(const RuleTable& table) : _table(table) {}

ConflictRuleService::~ConflictRuleService() {}

void ConflictRuleService::apply(const ASTNode& node, const std::string& context, ErrorList& errors)
{
	if (node.getType() != AST_NODETYPE_BLOCK && node.getType() != AST_NODETYPE_ROOT)
		return;

	std::vector< ASTNode* >							  children;
	std::map< std::string, SourceLocation >			  locations;
	std::set< std::pair< std::string, std::string > > emitted;
	const std::string contextName = resolveContextName(node, context);

	collectChildren(node, children);
	for (size_t i = 0; i < children.size(); ++i)
	{
		std::string childName;
		if (!getNodeName(*children[i], childName))
			continue;
		if (locations.find(childName) == locations.end())
			locations[childName] = children[i]->getLocation();
	}
	for (std::map< std::string, SourceLocation >::const_iterator it = locations.begin();
		 it != locations.end(); ++it)
	{
		std::set< std::string > conflicts;
		if (!this->_table.getConflicts(it->first, conflicts))
			continue;
		for (std::set< std::string >::const_iterator conflictIt = conflicts.begin();
			 conflictIt != conflicts.end(); ++conflictIt)
		{
			std::pair< std::string, std::string > key(it->first, *conflictIt);
			if (locations.find(*conflictIt) == locations.end())
				continue;
			if (emitted.find(key) != emitted.end())
				continue;
			emitted.insert(key);
			errors.addError(CompilerError::directiveConflictError(it->first, *conflictIt,
																  contextName, it->second));
		}
	}
}
