/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompilerError.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:29:41 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:14:13 by lsilva-x         ###   ########.fr       */
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
	case ERROR_UNKNOWN:
		return "ERROR_UNKNOWN";
	case ERROR_FILE_NOT_FOUND:
		return "ERROR_FILE_NOT_FOUND";
	case ERROR_PERMISSION_DENIED:
		return "ERROR_PERMISSION_DENIED";
	case ERROR_IO_ERROR:
		return "ERROR_IO_ERROR";
	case ERROR_UNTERMINATED_STRING_LITERAL:
		return "ERROR_UNTERMINATED_STRING_LITERAL";
	case ERROR_UNRECOGNIZED_CHARACTER:
		return "ERROR_UNRECOGNIZED_CHARACTER";
	case ERROR_EXPECTED_NAME:
		return "ERROR_EXPECTED_NAME";
	case ERROR_EXPECTED_VALUE:
		return "ERROR_EXPECTED_VALUE";
	case ERROR_EXPECTED_LEFT_BRACE:
		return "ERROR_EXPECTED_LEFT_BRACE";
	case ERROR_UNEXPECTED_TOKEN:
		return "ERROR_UNEXPECTED_TOKEN";
	case ERROR_DIRECTIVE_NOT_ALLOWED_IN_CONTEXT:
		return "ERROR_DIRECTIVE_NOT_ALLOWED_IN_CONTEXT";
	case ERROR_DIRECTIVE_CARDINALITY_VIOLATION:
		return "ERROR_DIRECTIVE_CARDINALITY_VIOLATION";
	case ERROR_DIRECTIVE_CONFLICT:
		return "ERROR_DIRECTIVE_CONFLICT";
	case ERROR_DIRECTIVE_DEPENDENCY_MISSING:
		return "ERROR_DIRECTIVE_DEPENDENCY_MISSING";
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

CompilerError CompilerError::unterminatedStringError(const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_UNTERMINATED_STRING_LITERAL;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = "Unterminated string literal";
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::unregonizedCharacterError(const SourceLocation& location, char c)
{
	CompilerError err;
	err.code		 = ERROR_UNRECOGNIZED_CHARACTER;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = "Unrecognized character: '" + std::string(1, c) + "'";
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::notARegularFileError(const std::string& filename)
{
	CompilerError err;
	err.code		 = ERROR_FILE_NOT_FOUND;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "Not a regular file: " + filename;
	err.has_location = false;
	err.has_hint	 = true;
	err.hint = "Ensure that the path points to a regular file and not a directory or special file.";
	return err;
}

CompilerError CompilerError::invalidBinaryFileError(const std::string& filename)
{
	CompilerError err;
	err.code		 = ERROR_IO_ERROR;
	err.severity	 = SEVERITY_FATAL;
	err.message		 = "Invalid binary file: " + filename;
	err.has_location = false;
	err.has_hint	 = true;
	err.hint = "The file appears to be a binary file. Ensure that you are providing a valid source "
			   "code file.";
	return err;
}

CompilerError CompilerError::expectedNameError(const std::string&	 message,
											   const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_EXPECTED_NAME;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = message;
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::expectedValueError(const std::string&	  message,
												const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_EXPECTED_VALUE;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = message;
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::expectedRightBraceError(const std::string&	   message,
													 const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_EXPECTED_LEFT_BRACE;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = message;
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::unepxectedTokenError(const std::string&	message,
												  const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_UNEXPECTED_TOKEN;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = message;
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::directiveNotAllowedInContextError(const std::string&	 directive,
															   const std::string&	 context,
															   const SourceLocation& location)
{
	CompilerError err;
	err.code		 = ERROR_DIRECTIVE_NOT_ALLOWED_IN_CONTEXT;
	err.severity	 = SEVERITY_ERROR;
	err.message		 = "Directive '" + directive + "' is not allowed in context '" + context + "'";
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::directiveCardinalityError(const std::string& directive,
													   const std::string& context, int min, int max,
													   int count, const SourceLocation& location)
{
	CompilerError	  err;
	std::stringstream ss;

	err.code	 = ERROR_DIRECTIVE_CARDINALITY_VIOLATION;
	err.severity = SEVERITY_ERROR;
	ss << "Directive '" << directive << "' appears " << count << " time(s) in context '" << context
	   << "'";
	if (min >= 0 && max >= 0)
		ss << ", expected between " << min << " and " << max;
	else if (min >= 0)
		ss << ", expected at least " << min;
	else if (max >= 0)
		ss << ", expected at most " << max;
	err.message		 = ss.str();
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::directiveConflictError(const std::string&	  directive,
													const std::string&	  conflictingDirective,
													const std::string&	  context,
													const SourceLocation& location)
{
	CompilerError err;
	err.code	 = ERROR_DIRECTIVE_CONFLICT;
	err.severity = SEVERITY_ERROR;
	err.message	 = "Directive '" + directive + "' conflicts with '" + conflictingDirective +
		"' in context '" + context + "'";
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}

CompilerError CompilerError::directiveDependencyError(const std::string&	directive,
													  const std::string&	requiredDirective,
													  const std::string&	context,
													  const SourceLocation& location)
{
	CompilerError err;
	err.code	 = ERROR_DIRECTIVE_DEPENDENCY_MISSING;
	err.severity = SEVERITY_ERROR;
	err.message	 = "Directive '" + directive + "' requires '" + requiredDirective +
		"' in context '" + context + "'";
	err.has_location = true;
	err.location	 = location;
	err.has_hint	 = false;
	return err;
}
