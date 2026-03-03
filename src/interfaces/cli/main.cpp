/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 17:29:11 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

// ------------------------ TMP IMPORT TO TEST ------------------------ //
#include "../../domain/errors/ErrorList.hpp"

#include "../../application/use_cases/CompileSourceFile.hpp"
#include "../../domain/entities/Token.hpp"
#include "../../infrastructure/common/TokenResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include <vector>

int main(int argc, const char** argv)
{
	Logger logger;
	logger.enableFileLogging();

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>\n";
		logger.log("No configuration file provided. Exiting.", ERROR);
		return (1);
	}

	TokenResult res = CompileSourceFile::execute(argv[1], &logger);

	if (res.isErr())
	{
		const ErrorList& errors = res.error();
		errors.formatAllErrors();
		logger.log("Failed to compile source file: " + std::string(argv[1]), ERROR);
		return (1);
	}

	std::vector<Token>* tokens = res.unwrap();
	(void)tokens;

	return (0);
}
