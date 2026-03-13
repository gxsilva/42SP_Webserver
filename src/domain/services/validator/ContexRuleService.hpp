/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContexRuleServiceService.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 23:41:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:46:39 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ContexRuleServiceSERVICE_HPP
#define ContexRuleServiceSERVICE_HPP

#include "../../ports/ISemanticRule.hpp"
#include "../../value_objects/RuleTable.hpp"

class ContextRule : public ISemanticRule
{
	private:
		const RuleTable& _table;

	public:
		ContextRule(const RuleTable& table);
		virtual ~ContextRule();

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* ContexRuleServiceSERVICE_HPP */
