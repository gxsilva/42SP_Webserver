/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DependencyRuleService.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 01:15:09 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:15:11 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DependencyRuleService.hpp"

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
		if (node.getType() == ASTNodeType::AST_NODETYPE_BLOCK)
		{
			const ASTBlock& blockNode = static_cast<const ASTBlock&>(node);
			name					  = blockNode.getName();
			return true;
		}
		if (node.getType() == ASTNodeType::AST_NODETYPE_DIRECTIVE)
		{
			const ASTDirective& directiveNode = static_cast<const ASTDirective&>(node);
			name							  = directiveNode.getName();
			return true;
		}
		return false;
	}

	void collectChildren(const ASTNode& node, std::vector<ASTNode*>& children)
	{
		if (node.getType() == ASTNodeType::AST_NODETYPE_BLOCK)
		{
			const ASTBlock&				 blockNode	   = static_cast<const ASTBlock&>(node);
			const std::vector<ASTNode*>& blockChildren = blockNode.getChildren();
			children.insert(children.end(), blockChildren.begin(), blockChildren.end());
			return;
		}
		if (node.getType() == ASTNodeType::AST_NODETYPE_ROOT)
		{
			const ASTRoot&				rootNode	 = static_cast<const ASTRoot&>(node);
			const std::vector<ASTNode*> rootChildren = rootNode.getStatements();
			children.insert(children.end(), rootChildren.begin(), rootChildren.end());
		}
	}

	std::string resolveContextName(const ASTNode& node, const std::string& fallback)
	{
		if (node.getType() == ASTNodeType::AST_NODETYPE_BLOCK)
		{
			const ASTBlock& blockNode = static_cast<const ASTBlock&>(node);
			return blockNode.getName();
		}
		if (node.getType() == ASTNodeType::AST_NODETYPE_ROOT)
			return "root";
		return fallback;
	}
} // namespace

DependencyRuleService::DependencyRuleService(const RuleTable& table) : _table(table) {}

DependencyRuleService::~DependencyRuleService() {}

void DependencyRuleService::apply(const ASTNode& node, const std::string& context,
								  ErrorList& errors)
{
	std::vector<ASTNode*>				  children;
	std::map<std::string, SourceLocation> locations;
	const std::string					  contextName = resolveContextName(node, context);

	collectChildren(node, children);
	for (size_t i = 0; i < children.size(); ++i)
	{
		std::string childName;
		if (!getNodeName(*children[i], childName))
			continue;
		if (locations.find(childName) == locations.end())
			locations[childName] = children[i]->getLocation();
	}
	for (std::map<std::string, SourceLocation>::const_iterator it = locations.begin();
		 it != locations.end(); ++it)
	{
		std::set<std::string> requirements;
		if (!this->_table.getRequirements(it->first, requirements))
			continue;
		for (std::set<std::string>::const_iterator reqIt = requirements.begin();
			 reqIt != requirements.end(); ++reqIt)
		{
			if (locations.find(*reqIt) != locations.end())
				continue;
			errors.addError(CompilerError::directiveDependencyError(it->first, *reqIt, contextName,
																	it->second));
		}
	}
}
