/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContextRuleServiceService.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 23:41:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:46:39 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Contexttuleeervicee_HPP
#define Contexttuleeervicee_HPP

#include "../../ports/ISemanticRule.hpp"
#include "../../value_objects/config/RuleTable.hpp"

class ContextRuleService : public ISemanticRule
{
	private:
		const RuleTable& _table;

	public:
		ContextRuleService(const RuleTable& table);
		virtual ~ContextRuleService();

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* Contexttuleeervicee_HPP */
