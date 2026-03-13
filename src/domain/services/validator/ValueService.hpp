/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValueService.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 02:30:08 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 02:30:18 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALUESERVICE_HPP
#define VALUESERVICE_HPP

#include "../../ports/ISemanticRule.hpp"

class ValueService : public ISemanticRule
{
	public:
		virtual ~ValueService() {}

		void apply(const ASTNode& node, const std::string& context, ErrorList& errors) override;
};

#endif /* VALUESERVICE_HPP */
