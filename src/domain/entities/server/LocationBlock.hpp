/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:12:36 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 05:21:30 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBLOCK_HPP
#define LOCATIONBLOCK_HPP

#include <map>
#include <set>
#include <string>

struct LocationBlock
{
		std::string path;

		std::string				root;
		std::set< std::string > allowedMethods;
		bool					autoindex;

		// std::string					 uploadPath;
		std::map< int, std::string > errorPages;
};

#endif /* LOCATIONBLOCK_HPP */
