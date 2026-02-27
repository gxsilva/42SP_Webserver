/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 19:42:33 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 22:40:01 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXICAL_ANALYZER_HPP
#define LEXICAL_ANALYZER_HPP

#include <string>

#include "../../../includes/macros.hpp"

class Lexer
{
	private:
		std::string _content;
		std::string _filePath;

	public:
		Lexer(const std::string& fileContent, const std::string& path);
		~Lexer();

		DISABLE_COPY(Lexer);
};

#endif /* LEXICAL_ANALYZER_HPP */
