/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTNode.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:20:10 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/05 22:30:45 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTNode.hpp"
#include "../../SourceLocation.hpp"

// ------------------------ OCCF ------------------------ //

ASTNode::ASTNode(ASTNodeType type, const SourceLocation& location)
	: _type(type), _location(location)
{
}
ASTNode::~ASTNode() {}

// ------------------------ METHODS ------------------------ //

ASTNodeType ASTNode::getType() const { return _type; }

SourceLocation ASTNode::getLocation() const { return _location; }

// ------------------------ STATIC METHODS ------------------------ //

std::string ASTNode::indentString(int ident)
{
	std::string indent;

	for (int i = 0; i < ident; ++i)
		indent += "  ";
	return indent;
}
