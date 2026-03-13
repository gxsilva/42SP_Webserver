/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompilerError.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:19:10 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 01:14:13 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPILERERROR_HPP
#define COMPILERERROR_HPP

#include <vector>

#include "../entities/SourceLocation.hpp"
#include "../value_objects/ErrorCode.hpp"
#include "../value_objects/ErrorSeverity.hpp"

// OBS: Static Factory Method

class CompilerError
{
	private:
		static std::string _errorCodeToString(ErrorCode code);

		ErrorCode	  code;
		ErrorSeverity severity;

		std::string message;

		SourceLocation location;
		bool		   has_location;

		std::vector< std::string > notes;

		bool		has_hint;
		std::string hint;

	public:
		CompilerError();
		~CompilerError();

		// ------------------------ METHODS ------------------------ //
		void addNote(const std::string& note);
		void addHint(const std::string& h);

		std::string format() const;
		// ------------------------ FACTORY METHODS ------------------------ //
		// File-related errors
		static CompilerError emptyPathError(const std::string& path);
		static CompilerError fileNotFoundError(const std::string& filename);
		static CompilerError permissionDeniedError(const std::string& filename);
		static CompilerError ioError(const std::string& filename);
		static CompilerError unterminatedStringError(const SourceLocation& location);
		static CompilerError unregonizedCharacterError(const SourceLocation& location, char c);
		static CompilerError notARegularFileError(const std::string& filename);
		static CompilerError invalidBinaryFileError(const std::string& filename);

		// Parsing-related errors
		static CompilerError expectedNameError(const std::string&	 message,
											   const SourceLocation& location);
		static CompilerError expectedValueError(const std::string&	  message,
												const SourceLocation& location);
		static CompilerError expectedRightBraceError(const std::string&	   message,
													 const SourceLocation& location);
		static CompilerError unepxectedTokenError(const std::string&	message,
												  const SourceLocation& location);

		// Semantic-related errors
		static CompilerError directiveNotAllowedInContextError(const std::string&	 directive,
															   const std::string&	 context,
															   const SourceLocation& location);
		static CompilerError directiveCardinalityError(const std::string& directive,
													   const std::string& context, int min, int max,
													   int count, const SourceLocation& location);
		static CompilerError directiveConflictError(const std::string&	  directive,
													const std::string&	  conflictingDirective,
													const std::string&	  context,
													const SourceLocation& location);
		static CompilerError directiveDependencyError(const std::string&	directive,
													  const std::string&	requiredDirective,
													  const std::string&	context,
													  const SourceLocation& location);
		static CompilerError directiveInvalidValueError(const std::string&	  directive,
														const std::string&	  value,
														const std::string&	  reason,
														const SourceLocation& location);
};

#endif /* COMPILERERROR_HPP */
