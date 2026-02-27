/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 22:08:23 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 22:15:01 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <fstream>
#include <string>

struct FileReader
{
		static bool readFile(const std::string& filepath);
};

bool FileReader::readFile(const std::string& filepath)
{
	std::string	  content;
	std::ifstream file(filepath.c_str());
	if (file.good())
	{
		std::string line;
		while (std::getline(file, line))
		{
			content += line + '\n';
		}
		file.close();
		return true;
	}
	return false;
}

#endif /* FILEREADER_HPP */
