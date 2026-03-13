/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RuleTable.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:01:01 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/12 22:56:25 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RULETABLE_HPP
#define RULETABLE_HPP

#include <map>
#include <set>
#include <string>

class RuleTable
{
	private:
		std::map<std::string, std::set<std::string>> _contextMap;
		std::map<std::string, std::pair<int, int>>	 _cardinalityMap;
		std::map<std::string, std::set<std::string>> _conflictMap;
		std::map<std::string, std::set<std::string>> _requiresMap;

	public:
		RuleTable();
		~RuleTable();

		// CONTEXT
		void addContext(const std::string& ruleName, const std::set<std::string>& contexts);
		bool isAllowedInContext(const std::string& ruleName,
								const std::string& currentContext) const;

		// CARDINALITY
		void setCardinality(const std::string& ruleName, int min, int max);
		bool checkCardinality(const std::string& ruleName, int count) const;

		// CONFLICTS
		void addConflict(const std::string& ruleName, const std::set<std::string>& conflicts);
		bool hasConflict(const std::string& ruleName, const std::string& currentContext) const;

		// REQUIRES
		void addRequirement(const std::string& ruleName, const std::set<std::string>& requirements);
		bool checkRequirements(const std::string& ruleName,
							   const std::string& currentContext) const;
};

#endif /* RULETABLE_HPP */
