/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CardinalityRuleService.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 00:00:00 by lsilva-x	       #+#    #+#             */
/*   Updated: 2026/03/13 01:14:27 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CARDINALITYRULESERVICE_HPP
#define CARDINALITYRULESERVICE_HPP

#include "../../ports/ISemanticRule.hpp"
#include "../../value_objects/RuleTable.hpp"

class CardinalityRuleService : public ISemanticRule
{
	private:
		const RuleTable& _table;
		const RuleTable& _contextTable;

	public:
		CardinalityRuleService(const RuleTable& table, const RuleTable& contextTable);
		virtual ~CardinalityRuleService();

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* CARDINALITYRULESERVICE_HPP */
