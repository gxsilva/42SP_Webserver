/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTValue.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:20:42 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:29:08 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTVALUE_HPP
#define ASTVALUE_HPP

#include "../../../value_objects/config/ASTValueType.hpp"
#include "../base/ASTNode.hpp"

class ASTValue : public ASTNode
{
	private:
		ASTValueType _valueType;
		std::string	 _value;

	public:
		ASTValue(ASTValueType vtype, const std::string& value, const SourceLocation& loc);
		virtual ~ASTValue();

		std::string getValue() const;

		std::string toString(int indentation) const;
};

#endif /* ASTVALUE_HPP */
