/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConflictRuleService.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 01:15:03 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:53:44 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFLICTRULESERVICE_HPP
#define CONFLICTRULESERVICE_HPP

#include "../../ports/ISemanticRule.hpp"
#include "../../value_objects/config/RuleTable.hpp"

class ConflictRuleService : public ISemanticRule
{
	private:
		const RuleTable& _table;

	public:
		ConflictRuleService(const RuleTable& table);
		virtual ~ConflictRuleService();

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* CONFLICTRULESERVICE_HPP */
