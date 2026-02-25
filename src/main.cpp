/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/24 21:30:33 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/header.hpp"

#include <iostream>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
#include "domain/entities/Token.hpp"

int main()
{
	// std::string fileName = "config.conf";
	// SourceLocation loc(fileName, 1, 1, 6);
	// std::cout << loc.toString() << std::endl;
	Token t(WORD, "server", SourceLocation("config.conf", 1, 1, 6));
	std::cout << t.toString() << std::endl;

	return 0;
}
