/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTRoot.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:21:32 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 21:21:36 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTRoot.hpp"

ASTRoot::ASTRoot() : ASTNode(AST_NODETYPE_ROOT, SourceLocation()) {}

ASTRoot::~ASTRoot()
{
	for (size_t i = 0; i < _statements.size(); ++i)
		delete _statements[i];
}

std::vector<ASTNode*> ASTRoot::getStatements() const { return _statements; }

void ASTRoot::addStatement(ASTNode* statement) { _statements.push_back(statement); }
