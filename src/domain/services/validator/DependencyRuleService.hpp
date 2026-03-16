/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DependencyRuleService.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 01:15:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:15:15 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEPENDENCYRULESERVICE_HPP
#define DEPENDENCYRULESERVICE_HPP

#include "../../ports/ISemanticRule.hpp"
#include "../../value_objects/RuleTable.hpp"

class DependencyRuleService : public ISemanticRule
{
	private:
		const RuleTable& _table;

	public:
		DependencyRuleService(const RuleTable& table);
		virtual ~DependencyRuleService();

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* DEPENDENCYRULESERVICE_HPP */
