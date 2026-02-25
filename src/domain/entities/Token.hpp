#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>

#include "../value_objects/TokenType.hpp"
#include "SourceLocation.hpp"

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
