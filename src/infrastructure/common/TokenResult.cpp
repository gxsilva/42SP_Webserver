/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenResult.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 01:10:43 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 01:41:10 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef> // for NULL
#include <string>
#include <vector>

#include "TokenResult.hpp"

#include "ResultBase.hpp"

#include "../../domain/entities/Token.hpp"
#include "../../domain/errors/ErrorList.hpp"

TokenResult::TokenResult(std::vector<Token>* toks) : ResultBase(true), _tokens(toks) {}

TokenResult::TokenResult(const ErrorList& error)
	: ResultBase(false), _tokens(NULL), _errorList(error)
{
}

TokenResult::~TokenResult()
{
	if (_is_ok && _tokens != NULL)
		delete _tokens;
}

std::vector<Token>* TokenResult::unwrap()
{
	if (isErr())
		throw std::runtime_error("Attempted to unwrap an error TokenResult");

	// transferencia de ownership do ponteiro para o chamador
	std::vector<Token>* tmp = _tokens;
	_tokens = NULL; // evita que o destrutor delete o ponteiro após a transferência
	return tmp;
}

const ErrorList& TokenResult::error() const
{
	if (isOk())
		throw std::runtime_error("Attempted to access error of an ok TokenResult");
	return _errorList;
}
