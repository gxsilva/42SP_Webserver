/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorList.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 23:13:36 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/24 23:33:47 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORLIST_HPP
#define ERRORLIST_HPP

#include <vector>

#include "CompilerError.hpp"

class ErrorList
{
	public:
		ErrorList();
		~ErrorList();

		void addError(const CompilerError& error);
		bool hasErrors() const;

		const std::vector<CompilerError>& getErrors() const;
		int								  size() const;

		void formatAllErrors() const;

	private:
		std::vector<CompilerError> errors;
};

#endif /* ERRORLIST_HPP */
