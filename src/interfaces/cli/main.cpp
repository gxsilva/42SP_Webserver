/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 15:59:27 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 16:00:13 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "../../infrastructure/logging/Logger.hpp"

int main(int argc, const char** argv)
{
	Logger logger;
	logger.enableFileLogging();

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>\n";
		logger.log("No configuration file provided. Exiting.", ERROR);
		return 1;
	}

	ConfigResult result = BuildServerConfig::execute(argv[1], &logger);
	if (result.isErr())
	{
		result.error().formatAllErrors();
		logger.log("Failed to build config: " + std::string(argv[1]), ERROR);
		return 1;
	}

	Config* config = result.unwrap();
	// usa config...
	delete config;
	return 0;
}
