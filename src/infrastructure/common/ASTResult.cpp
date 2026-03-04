/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTResult.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 00:27:49 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:27:49 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASTResult.hpp"

ASTResult::ASTResult(const ErrorList& error) : ResultBase(false), _astRoot(NULL), _errorList(error)
{
}

ASTResult::ASTResult(ASTNode* astRoot) : ResultBase(true), _astRoot(astRoot) {}

ASTResult::~ASTResult()
{
	if (_is_ok && _astRoot != NULL)
		delete _astRoot;
}

ASTNode* ASTResult::unwrap()
{
	if (isErr())
		throw std::runtime_error("Attempted to unwrap an Err value from ASTResult");
	return _astRoot;
}

const ErrorList& ASTResult::error() const
{
	if (isOk())
		throw std::runtime_error("Attempted to access error of an Ok value from ASTResult");
	return _errorList;
}
