/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CompilerError.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 22:19:10 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 00:35:42 by lsilva-x         ###   ########.fr       */
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

		std::vector<std::string> notes;

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
		static CompilerError fileNotFound(const std::string& filename);
		static CompilerError permissionDenied(const std::string& filename);
};

#endif /* COMPILERERROR_HPP */
