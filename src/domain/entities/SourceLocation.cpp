/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SourceLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:48:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/24 21:28:32 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SourceLocation.hpp"

#include <sstream>

// ------------------------ Source Location Implementation ------------------------ //

// ------------------------ OCCF ------------------------ //
SourceLocation::SourceLocation(void) : line(0), column(0), length(0) {}

SourceLocation::SourceLocation(const std::string& fname, size_t l, size_t c, size_t len)
	: filename(fname), line(l), column(c), length(len)
{
}

// ------------------------ METHODS ------------------------ //
std::string SourceLocation::toString() const
{
	std::ostringstream ss;

	ss << this->filename << ":" << this->line << ":" << this->column;
	return ss.str();
}
