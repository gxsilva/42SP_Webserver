/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TokenResult.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 00:51:55 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:08:03 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENRESULT_HPP
#define TOKENRESULT_HPP

#include <vector>

#include "ResultBase.hpp"

#include "../../domain/errors/ErrorList.hpp"

#include "../../../includes/macros.hpp"

class Token;

class TokenResult : public ResultBase
{
	private:
		std::vector<Token>* _tokens;
		ErrorList			_errorList;

	public:
		TokenResult(std::vector<Token>* toks);
		TokenResult(const ErrorList& error);
		~TokenResult();

		std::vector<Token>* unwrap();
		const ErrorList&	error() const;

		// DISABLE_COPY(TokenResult);
};

#endif /* TOKENRESULT_HPP */
