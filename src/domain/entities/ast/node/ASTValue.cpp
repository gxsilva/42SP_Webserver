/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTValue.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:43:55 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/05 22:10:55 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTValue.hpp"

#include <sstream>

ASTValue::ASTValue(ASTValueType vtype, const std::string& value, const SourceLocation& loc)
	: ASTNode(AST_NODETYPE_VALUE, loc), _valueType(vtype), _value(value)
{
}

ASTValue::~ASTValue() {}

std::string ASTValue::getValue() const { return _value; }

std::string ASTValue::toString(int ident) const
{
	std::ostringstream oss;

	oss << ASTNode::indentString(ident);
	oss << "ASTValue(" << _valueType << ", \"" << _value << "\") at " << getLocation().toString();
	return oss.str();
}
