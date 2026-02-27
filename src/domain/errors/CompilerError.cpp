/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompilerError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:29:41 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 22:51:00 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CompilerError.hpp"

#include <cstddef> // for size_t
#include <sstream>
#include <string>

#include "../value_objects/ErrorCode.hpp"
#include "../value_objects/ErrorSeverity.hpp"

// ------------------------ OCCF ------------------------ //

CompilerError::CompilerError()
	: code(ERROR_UNKNOWN), severity(SEVERITY_UNKNOWN), has_location(false), has_hint(false)
{
}

CompilerError::~CompilerError() {}

// ------------------------ METHODS ------------------------ //

void CompilerError::addNote(const std::string& note) { notes.push_back(note); }

void CompilerError::addHint(const std::string& h)
{
	this->has_hint = true;
	this->hint	   = h;
}

std::string CompilerError::_errorCodeToString(ErrorCode code)
{
	switch (code)
	{
	case ERROR_FILE_NOT_FOUND:
		return "ERROR_FILE_NOT_FOUND";
	case ERROR_PERMISSION_DENIED:
		return "ERROR_PERMISSION_DENIED";
	case ERROR_IO_ERROR:
		return "ERROR_IO_ERROR";
	case ERROR_UNKNOWN:
	default:
		return "Error not registered in the system.";
	}
}

std::string CompilerError::format() const
{
	std::stringstream ss;

	ss << "[" << _errorCodeToString(code) << "] " << message;

	if (has_location)
	{
		ss << " at " + location.toString();
	}
	for (size_t i = 0; i < notes.size(); ++i)
	{
		ss << "\n  Note: " + notes.at(i);
	}
	if (has_hint)
	{
		ss << "\n  Hint: " + hint;
	}
	return ss.str();
}
// ------------------------ FACTORY METHOD ------------------------ //

CompilerError CompilerError::fileNotFoundError(const std::string& filename)
{
	CompilerError err;
	err.code		 = ERROR_FILE_NOT_FOUND;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "File not found: " + filename;
	err.has_location = false;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::permissionDeniedError(const std::string& filename)
{
	CompilerError err;
	err.code		 = ERROR_PERMISSION_DENIED;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "no read permission on the file: " + filename;
	err.has_location = false;
	err.has_hint	 = true;
	err.hint =
		"Check the read permissions on the file and ensure that the compiler has access to it.";
	return err;
}

CompilerError CompilerError::emptyPathError(const std::string& path)
{
	CompilerError err;
	err.code		 = ERROR_FILE_NOT_FOUND;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "File path is empty: " + path;
	err.has_location = false;
	err.has_hint	 = true;
	err.hint		 = "Provide a valid file path to the compiler.";
	return err;
}

CompilerError CompilerError::ioError(const std::string& filename)
{
	CompilerError err;
	err.code		 = ERROR_IO_ERROR;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "I/O error while accessing the file: " + filename;
	err.has_location = false;
	err.has_hint	 = true;
	err.hint		 = "Check if the file is accessible and not locked by another process.";
	return err;
}
