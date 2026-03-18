/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 15:59:27 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 21:52:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>

#include "../../infrastructure/common/config/ConfigResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include "../../application/use_cases/BuildServerConfig.hpp"

#include "../../domain/entities/server/HttpBlock.hpp"

// ---------------------- DEVELOPMENT ---------------------- //
#include "../../application/network/server.hpp"
#include "../../domain/network/ipAddr.hpp"
#include "../../domain/network/port.hpp"

// --------------------------------------------------------- //

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

	HttpBlock* server = result.unwrap();

	// ---------------------- DEVELOPMENT ---------------------- //
	Port   port(server->server.port);
	IpAddr ip(server->server.host);

	Server webServer(port, ip, server);

	if (!webServer.isValid())
	{
		std::stringstream ss;
		ss << "Failed to initialize server on " << ip.getValue() << ":" << port.getValue();
		logger.log(ss.str(), ERROR);
		return 1;
	}
	logger.log("Server initialized successfully on " + ip.getValue() + ":" + port.toString(), INFO);
	webServer.displayServerStatus();
	webServer.run();
	// delete server;
	// --------------------------------------------------------- //
	return 0;
}
