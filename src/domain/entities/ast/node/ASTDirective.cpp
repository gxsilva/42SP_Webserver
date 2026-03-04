/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTDirective.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:52:04 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 20:57:27 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTDirective.hpp"
#include "ASTValue.hpp"
#include "sstream"

ASTDirective::ASTDirective(const std::string& name, const SourceLocation& loc)
	: ASTNode(AST_NODETYPE_DIRECTIVE, loc), _name(name)
{
}

ASTDirective::~ASTDirective()
{
	for (size_t i = 0; i < _values.size(); ++i)
	{
		delete _values[i];
	}
}

const std::string& ASTDirective::getName() const { return _name; }

const std::vector<ASTValue*>& ASTDirective::getValues() const { return _values; }

std::string ASTDirective::toString() const
{
	std::ostringstream oss;

	oss << "ASTDirective(\"" << _name << "\", values=[";

	for (size_t i = 0; i < _values.size(); ++i)
	{
		oss << _values[i]->toString();
		if (i < _values.size() - 1)
		{
			oss << ", ";
		}
	}
	oss << "]) at " << getLocation().toString();
	return oss.str();
}

void ASTDirective::addValue(ASTValue* value) { _values.push_back(value); }
