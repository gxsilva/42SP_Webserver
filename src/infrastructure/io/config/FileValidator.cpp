/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 23:04:22 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 05:05:40 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <fstream>
#include <ios>
#include <string>
#include <sys/stat.h> // for stat()
#include <unistd.h>	  // for access()

#include "FileValidator.hpp"

#include "../../../domain/errors/common/CompilerError.hpp"

CompilerError* FileValidator::validateFile(const std::string& filePath)
{
	CompilerError* err = FileValidator::validateExists(filePath);
	if (err != NULL)
		return err;

	err = FileValidator::validateIsRegularFile(filePath);
	if (err != NULL)
		return err;

	err = FileValidator::validateBinaryFile(filePath);
	if (err != NULL)
		return err;

	return FileValidator::validateReadPermission(filePath);
}

CompilerError* FileValidator::validateExists(const std::string& filepath)
{
	if (access(filepath.c_str(), F_OK) != 0)
		return new CompilerError(CompilerError::fileNotFoundError(filepath));
	return NULL;
}

CompilerError* FileValidator::validateIsRegularFile(const std::string& filepath)
{
	struct stat fileStat = {};

	if (stat(filepath.c_str(), &fileStat) != 0)
		return new CompilerError(CompilerError::fileNotFoundError(filepath));
	if (!S_ISREG(fileStat.st_mode))
		return new CompilerError(CompilerError::notARegularFileError(filepath));

	return NULL;
}

CompilerError* FileValidator::validateBinaryFile(const std::string& filepath)
{

	std::ifstream file(filepath.c_str(), std::ios::in | std::ios::binary);
	if (!file.good())
		return new CompilerError(CompilerError::permissionDeniedError(filepath));

	char buffer[FileValidator::sampleSize];
	file.read(buffer, static_cast< std::streamsize >(FileValidator::sampleSize));
	const std::streamsize bytesRead = file.gcount();
	file.close();

	std::size_t i = 0;
	while (i < static_cast< std::size_t >(bytesRead))
	{
		if (buffer[i] == '\0')
			return new CompilerError(CompilerError::invalidBinaryFileError(filepath));
		++i;
	}
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
