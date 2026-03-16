/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 22:08:23 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 02:32:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <fstream>
#include <string>

struct FileReader
{
		static bool readFile(const std::string& filepath, std::string& content);
};

#endif /* FILEREADER_HPP */
