/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTBlock.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:06:24 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/09 19:12:25 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTBlock.hpp"
#include "ASTValue.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "../../../entities/SourceLocation.hpp"
#include "../../../value_objects/ASTNodeType.hpp"
#include "../base/ASTNode.hpp"

// ------------------------ OCCD ------------------------ //
ASTBlock::ASTBlock(const std::string& name, const SourceLocation& loc) : ASTNode(AST_NODETYPE_BLOCK, loc), name_(name)
{
}

ASTBlock::~ASTBlock()
{
	for (size_t i = 0; i < parameters_.size(); ++i)
	{
		delete parameters_[i];
	}
	for (size_t i = 0; i < children_.size(); ++i)
	{
		delete children_[i];
	}
}

// ------------------------ METHODS ------------------------ //
const std::string& ASTBlock::getName() const { return name_; }

const std::vector< ASTValue* >& ASTBlock::getParameters() const { return parameters_; }

const std::vector< ASTNode* >& ASTBlock::getChildren() const { return children_; }

std::string ASTBlock::toString(int indentation) const
{
	std::ostringstream oss;
	const std::string  indent = ASTNode::indentString(indentation);
	oss << indent << "ASTBlock Name: " << name_ << "\n";
	oss << indent << "ASTBlock Parameters:" << "\n";
	for (size_t i = 0; i < parameters_.size(); ++i)
	{
		oss << parameters_[i]->toString(indentation + 1) + "\n";
	}
	oss << indent << "ASTBlock Children:\n";
	for (size_t i = 0; i < children_.size(); ++i)
	{
		oss << " - " << children_[i]->toString(indentation + 1) + "\n";
	}
	return oss.str();
}

void ASTBlock::addParameter(ASTValue* param) { parameters_.push_back(param); }

void ASTBlock::addChild(ASTNode* child) { children_.push_back(child); }
