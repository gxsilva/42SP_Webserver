/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTDirective.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:52:04 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/09 19:12:25 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTDirective.hpp"
#include "ASTValue.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "../../../entities/SourceLocation.hpp"
#include "../../../value_objects/ASTNodeType.hpp"
#include "../base/ASTNode.hpp"

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

std::string ASTDirective::toString(int indentation) const
{
	std::ostringstream oss;
	const std::string  ind		= ASTNode::indentString(indentation);
	const std::string  indChild = ASTNode::indentString(indentation + 1);

	oss << ind << "+- Directive: \"" << _name << "\"\n";
	oss << ind << "|  Location: " << getLocation().toString() << "\n";

	if (_values.empty())
	{
		oss << ind << "\\- Values: (none)";
		return oss.str();
	}

	oss << ind << "\\- Values:\n";

	for (size_t i = 0; i < _values.size(); ++i)
	{
		const bool isLast = (i + 1 == _values.size());
		oss << indChild;
		oss << "+- ";
		oss << _values[i]->toString(indentation + 2);
		if (!isLast)
			oss << "\n";
	}
	return oss.str();
}

void ASTDirective::addValue(ASTValue* value) { _values.push_back(value); }
