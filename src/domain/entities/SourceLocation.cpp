/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SourceLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:48:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 00:25:37 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SourceLocation.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

// ------------------------ Source Location Implementation ------------------------ //

// ------------------------ OCCF ------------------------ //
SourceLocation::SourceLocation(void) : line(0), column(0), length(0) {}

SourceLocation::SourceLocation(const std::string& file_name, size_t line_number,
							   size_t column_number, size_t token_length)
	: filename(file_name), line(line_number), column(column_number), length(token_length)
{
}

// ------------------------ METHODS ------------------------ //
std::string SourceLocation::toString() const
{
	std::ostringstream ss;

	ss << this->filename << ":" << this->line << ":" << this->column;
	return ss.str();
}
