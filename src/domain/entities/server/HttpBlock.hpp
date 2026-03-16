/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:06:17 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:32:19 by lsilva-x         ###   ########.fr       */
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
		std::map< int, std::string > errorPages;

		ServerBlock server;
		// std::vector<ServerBlock>        servers;
};

#endif /* HTTPBLOCK_HPP */
