/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerResult.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 20:04:31 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 22:48:26 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LexerResult.hpp"
#include "../../domain/services/Lexer.hpp"

// ------------------------ LexerResult ------------------------ //

// ------------------------ OCCF ------------------------ //
LexerResult::LexerResult(Lexer* lexer) : ResultBase(true), _lexer(lexer) {}
LexerResult::LexerResult(ErrorList error) : ResultBase(false), _errorList(error) {}
LexerResult::LexerResult(CompilerError err) : ResultBase(false) { _errorList.addError(err); }

LexerResult::~LexerResult()
{
	if (_is_ok && _lexer != NULL)
		delete _lexer;
}

// ------------------------ METHODS ------------------------ //

Lexer* LexerResult::unwrap()
{
	if (isErr())
		throw std::runtime_error("Attempted to unwrap an Err value from LexerResult");
	return _lexer;
}

ErrorList const& LexerResult::error() const
{
	if (isOk())
		throw std::runtime_error("Attempted to access error of an Ok value from LexerResult");
	return _errorList;
}
