/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SourceLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:48:10 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/24 21:53:05 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOURCELOCATION_HPP
#define SOURCELOCATION_HPP

#include <iostream>

class SourceLocation
{
	public:
		std::string filename;
		size_t		line;
		size_t		column;
		size_t		length;

		SourceLocation(void);
		SourceLocation(const std::string& fname, size_t l, size_t c, size_t len);

		std::string toString() const;
};

#endif /* SOURCELOCATION_HPP */
