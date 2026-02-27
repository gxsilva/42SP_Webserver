/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 23:04:05 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 04:28:14 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileReader.hpp"

#include <fstream>
#include <string>

bool FileReader::readFile(const std::string& filepath, std::string& content)
{
	std::ifstream file(filepath.c_str());
	if (file.good())
	{
		std::string line;
		while (static_cast<bool>(std::getline(file, line)))
		{
			content += line + '\n';
		}
		file.close();
		return true;
	}
	return false;
}
