/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RuleRegistry.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:52:31 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:50:43 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RULEREGISTRY_HPP
#define RULEREGISTRY_HPP

#include "../../../domain/value_objects/config/RuleTable.hpp"

class RuleRegistry
{
	public:
		RuleRegistry();
		~RuleRegistry();

		const RuleTable& getContextTable() const;
		const RuleTable& getCardinalityTable() const;
		const RuleTable& getConflictTable() const;
		const RuleTable& getDependencyTable() const;

	private:
		RuleTable _context;
		RuleTable _cardinality;
		RuleTable _conflict;
		RuleTable _dependency;

		void _registerContextRuleServices();
		void _registerCardinalityRules();
		void _registerConflictRules();
		void _registerDependencyRules();
};

#endif /* RULEREGISTRY_HPP */
