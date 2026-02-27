/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 21:08:40 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 22:55:20 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEVALIDATOR_HPP
#define FILEVALIDATOR_HPP

#include <fstream>
#include <string>
#include <unistd.h> // for access()

#include "../../../includes/macros.hpp"
#include "../../domain/errors/CompilerError.hpp"

struct FileValidator
{
		static CompilerError* validateFile(const std::string& filePath);
		static CompilerError* validateReadPermission(const std::string& filepath);
		static CompilerError* validateExists(const std::string& filepath);
};

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

#endif /* FILEVALIDATOR_HPP */
