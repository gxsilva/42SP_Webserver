/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorList.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 23:15:30 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 00:22:21 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>

#include "CompilerError.hpp"
#include "ErrorList.hpp"

// ------------------------ ErroList implementation ------------------------ //

// ------------------------ OCCF ------------------------ //
ErrorList::ErrorList() {}

ErrorList::~ErrorList() {}

// ------------------------ METHODS ------------------------ //
void ErrorList::addError(const CompilerError& error) { errors.push_back(error); }

const std::vector<CompilerError>& ErrorList::getErrors() const { return errors; }

bool ErrorList::hasErrors() const { return !errors.empty(); }

int ErrorList::size() const { return static_cast<int>(errors.size()); }

void ErrorList::formatAllErrors() const
{
	for (int i = 0; i < size(); ++i)
	{
		std::cout << errors.at(i).format() << '\n';
	}
}
