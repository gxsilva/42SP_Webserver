/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Token.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 04:30:39 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:31:54 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>

#include "../common/SourceLocation.hpp"

#include "../../value_objects/config/TokenType.hpp"

class Token
{
	private:
		static const char* _tokenTypeToString(TokenType type);

	public:
		TokenType	   type;
		std::string	   value;
		SourceLocation location;

		Token();
		Token(TokenType t, const std::string& v, const SourceLocation& loc);

		std::string toString() const;
};

#endif /* TOKEN_HPP */
