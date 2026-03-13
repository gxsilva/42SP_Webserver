/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RuleTable.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:50:49 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/12 22:51:21 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RuleTable.hpp"

#include <map>
#include <set>
#include <string>

// ------------------------ OCCF ------------------------ //

RuleTable::RuleTable() {}
RuleTable::~RuleTable() {}

// ------------------------ CONTEXT METHODS ------------------------ //

void RuleTable::addContext(const std::string& ruleName, const std::set<std::string>& contexts)
{
	// Replace
	this->_contextMap[ruleName] = contexts;
	// Append (? não sei se é necessário)
	//  this->_contextMap[ruleName].insert(contexts.begin(), contexts.end());
}

bool RuleTable::isAllowedInContext(const std::string& ruleName,
								   const std::string& currentContext) const
{
	std::map<std::string, std::set<std::string>>::const_iterator it =
		this->_contextMap.find(ruleName);
	if (it == this->_contextMap.end())
		return false;

	const std::set<std::string>& allowedContexts = it->second;
	for (std::set<std::string>::const_iterator ctxIt = allowedContexts.begin();
		 ctxIt != allowedContexts.end(); ++ctxIt)
	{
		if (currentContext == *ctxIt)
			return true;
	}
	return false;
}

// ------------------------ CARDINALITY METHODS ------------------------ //
void RuleTable::setCardinality(const std::string& ruleName, int min, int max)
{
	this->_cardinalityMap[ruleName] = std::make_pair(min, max);
}

bool RuleTable::checkCardinality(const std::string& ruleName, int count) const
{
	std::map<std::string, std::pair<int, int>>::const_iterator it =
		this->_cardinalityMap.find(ruleName);
	if (it == this->_cardinalityMap.end())
		return false;

	const std::pair<int, int>& cardinality = it->second;
	return (count >= cardinality.first && count <= cardinality.second);
}

// ------------------------ CONFLICT METHODS ------------------------ //
void RuleTable::addConflict(const std::string& ruleName, const std::set<std::string>& conflicts)
{
	this->_conflictMap[ruleName] = conflicts;
}

bool RuleTable::hasConflict(const std::string& ruleName, const std::string& currentContext) const
{
	std::map<std::string, std::set<std::string>>::const_iterator it =
		this->_conflictMap.find(ruleName);
	if (it == this->_conflictMap.end())
		return false;

	const std::set<std::string>& conflictRules = it->second;
	for (std::set<std::string>::const_iterator conflictIt = conflictRules.begin();
		 conflictIt != conflictRules.end(); ++conflictIt)
	{
		if (currentContext == *conflictIt)
			return true;
	}
	return false;
}

// ------------------------ REQUIREMENT METHODS ------------------------ //
void RuleTable::addRequirement(const std::string&			ruleName,
							   const std::set<std::string>& requirements)
{
	this->_requiresMap[ruleName] = requirements;
}

bool RuleTable::checkRequirements(const std::string& ruleName,
								  const std::string& currentContext) const
{
	std::map<std::string, std::set<std::string>>::const_iterator it =
		this->_requiresMap.find(ruleName);
	if (it == this->_requiresMap.end())
		return true;

	const std::set<std::string>& requiredRules = it->second;
	for (std::set<std::string>::const_iterator reqIt = requiredRules.begin();
		 reqIt != requiredRules.end(); ++reqIt)
	{
		if (currentContext == *reqIt)
			return true;
	}
	return false;
}
