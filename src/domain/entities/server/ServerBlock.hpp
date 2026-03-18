/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:12:02 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 18:43:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP

#include <map>
#include <string>
#include <vector>

#include "LocationBlock.hpp"

struct ServerBlock
{
		int			port;
		std::string host;
		std::string serverName;

		std::string				   root;
		std::vector< std::string > index;
		size_t					   clientMaxBodySize;

		std::map< int, std::string > errorPages;

		std::vector< LocationBlock > locations;
};

#endif /* SERVERBLOCK_HPP */
