/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerResult.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 19:40:45 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 04:21:01 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXERRESULT_HPP
#define LEXERRESULT_HPP

#include "ResultBase.hpp"

#include "../../../includes/macros.hpp"
#include "../../domain/errors/CompilerError.hpp"
#include "../../domain/errors/ErrorList.hpp"

class Lexer;

class LexerResult : public ResultBase
{
	private:
		Lexer*	  _lexer;
		ErrorList _errorList;

	public:
		LexerResult(Lexer* lexer);

		LexerResult(const CompilerError& err);
		LexerResult(const ErrorList& error);
		virtual ~LexerResult();

		Lexer*			 unwrap();
		const ErrorList& error() const;

		// DISABLE_COPY(LexerResult);
};

#endif /* LEXERRESULT_HPP */
