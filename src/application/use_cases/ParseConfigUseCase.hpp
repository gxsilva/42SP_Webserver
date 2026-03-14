/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfigUseCase.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:09:39 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:39 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSECONFIGUSECASE_HPP
#define PARSECONFIGUSECASE_HPP

#include <string>

#include "../../../includes/macros.hpp"
#include "../../infrastructure/common/ASTResult.hpp"
#include "../ports/ILogger.hpp"

/*
	Full config compilation pipeline:
	  file -> FileValidator -> FileReader -> Lexer -> Tokens -> Parser -> AST
*/

class ParseConfigUseCase
{
	private:
		ParseConfigUseCase();
		~ParseConfigUseCase();

		DISABLE_COPY(ParseConfigUseCase);

	public:
		static ASTResult execute(const std::string& filePath, ILogger* logger);
};

#endif /* PARSECONFIGUSECASE_HPP */
