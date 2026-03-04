/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTBlock.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:06:24 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:37:12 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTBlock.hpp"
#include "ASTValue.hpp"

#include "../../../value_objects/ASTNodeType.hpp"
#include "../../../value_objects/ASTValueType.hpp"

// ------------------------ OCCD ------------------------ //
ASTBlock::ASTBlock(const std::string& name, const SourceLocation& loc)
	: ASTNode(AST_NODETYPE_BLOCK, loc), name_(name)
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

const std::vector<ASTValue*>& ASTBlock::getParameters() const { return parameters_; }

const std::vector<ASTNode*>& ASTBlock::getChildren() const { return children_; }

std::string ASTBlock::toString() const
{
	std::string result = "Block: " + name_ + "\n";
	result += "Parameters:\n";
	for (size_t i = 0; i < parameters_.size(); ++i)
	{
		result += "  - " + parameters_[i]->toString() + "\n";
	}
	result += "Children:\n";
	for (size_t i = 0; i < children_.size(); ++i)
	{
		result += "  - " + children_[i]->toString() + "\n";
	}
	return result;
}

void ASTBlock::addParameter(ASTValue* param) { parameters_.push_back(param); }

void ASTBlock::addChild(ASTNode* child) { children_.push_back(child); }
