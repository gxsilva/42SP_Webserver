/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:06:17 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 18:43:50 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPBLOCK_HPP
#define HTTPBLOCK_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ServerBlock.hpp"

struct HttpBlock
{
		size_t						 clientMaxBodySize;
		std::string					 host;
		std::map< int, std::string > errorPages;

		ServerBlock server;
		// std::vector<ServerBlock>        servers;
};

#endif /* HTTPBLOCK_HPP */
