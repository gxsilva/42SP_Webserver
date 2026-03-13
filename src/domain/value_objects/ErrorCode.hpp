/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:23:52 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:14:12 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORCODE_HPP
#define ERRORCODE_HPP

enum ErrorCode
{
	ERROR_UNKNOWN = 0,
	ERROR_FILE_NOT_FOUND,
	ERROR_PERMISSION_DENIED,
	ERROR_IO_ERROR,
	ERROR_UNTERMINATED_STRING_LITERAL,
	ERROR_UNRECOGNIZED_CHARACTER,
	ERROR_EXPECTED_NAME,
	ERROR_EXPECTED_VALUE,
	ERROR_EXPECTED_LEFT_BRACE,
	ERROR_UNEXPECTED_TOKEN,
	ERROR_DIRECTIVE_NOT_ALLOWED_IN_CONTEXT,
	ERROR_DIRECTIVE_CARDINALITY_VIOLATION,
	ERROR_DIRECTIVE_CONFLICT,
	ERROR_DIRECTIVE_DEPENDENCY_MISSING
};

#endif /* ERRORCODE_HPP */
