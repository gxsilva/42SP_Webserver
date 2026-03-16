/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValueRuleService.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 02:30:08 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 02:30:18 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ValueRuleService_HPP
#define ValueRuleService_HPP

#include "../../ports/ISemanticRule.hpp"

class ValueRuleService : public ISemanticRule
{
	public:
		virtual ~ValueRuleService() {}

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors);
};

#endif /* ValueRuleService_HPP */
