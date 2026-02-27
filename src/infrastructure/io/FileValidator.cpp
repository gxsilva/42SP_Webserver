/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 23:04:22 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 04:29:27 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <fstream>
#include <ios>
#include <string>
#include <unistd.h> // for access()

#include "FileValidator.hpp"

#include "../../domain/errors/CompilerError.hpp"

CompilerError* FileValidator::validateFile(const std::string& filePath)
{
	CompilerError* permissionErr = FileValidator::validateExists(filePath);
	if (permissionErr != NULL)
		return permissionErr;
	return FileValidator::validateReadPermission(filePath);
}

CompilerError* FileValidator::validateExists(const std::string& filepath)
{
	if (access(filepath.c_str(), F_OK) != 0)
		return new CompilerError(CompilerError::fileNotFoundError(filepath));
	return NULL;
}

CompilerError* FileValidator::validateReadPermission(const std::string& filepath)
{
	std::ifstream file(filepath.c_str(), std::ios::in);
	if (!file.good())
		return new CompilerError(CompilerError::permissionDeniedError(filepath));
	file.close();
	return NULL;
}
