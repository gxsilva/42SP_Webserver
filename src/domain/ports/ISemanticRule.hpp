/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ISemanticRule.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 23:27:53 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/12 23:35:42 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISEMANTICRULE_HPP
#define ISEMANTICRULE_HPP

#include "../entities/ast/base/ASTNode.hpp"
#include "../errors/ErrorList.hpp"

class ISemanticRule
{
	public:
		virtual ~ISemanticRule() = default;

		virtual void apply(const ASTNode& node, const std::string& context, ErrorList& errors) = 0;
};

#endif /* ISEMANTICRULE_HPP */
