/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:23:52 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:36:04 by lsilva-x         ###   ########.fr       */
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
	ERROR_UNRECOGNIZED_CHARACTER
};

#endif /* ERRORCODE_HPP */
