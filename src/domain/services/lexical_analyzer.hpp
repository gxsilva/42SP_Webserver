/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexical_analyzer.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 19:42:33 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 20:25:24 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXICAL_ANALYZER_HPP
#define LEXICAL_ANALYZER_HPP

#include <string>

#include "LexerResult.hpp"

class Lexer
{
	public:
		Lexer();
		~Lexer();

		// LexerResult analyze(const std::string& input);

		DISABLE_COPY(Lexer);
};

Lexer::Lexer() {}
Lexer::~Lexer() {}

#endif /* LEXICAL_ANALYZER_HPP */
