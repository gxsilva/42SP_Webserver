/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 15:59:27 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/19 20:08:26 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include <sstream>

#include "../../application/network/server.hpp"
#include "../../domain/entities/server/HttpBlock.hpp"
#include "../../infrastructure/common/config/ConfigResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

#include "../../application/use_cases/BuildServerConfig.hpp"

#include "../../domain/network/ipAddr.hpp"
#include "../../domain/network/port.hpp"

namespace
{
	void handleTerminationSignal(int) { Server::requestStop(); }
} // namespace

int main(int argc, const char** argv)
{
	std::signal(SIGINT, handleTerminationSignal);
	std::signal(SIGTERM, handleTerminationSignal);

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

	HttpBlock*				   httpConfig	 = result.unwrap();
	std::vector< ServerBlock > serverConfigs = httpConfig->servers;
	if (serverConfigs.empty())
		serverConfigs.push_back(httpConfig->server);

	int			configuredPort = serverConfigs[0].port;
	std::string configuredHost = serverConfigs[0].host;

	if (configuredHost.empty())
		configuredHost = "127.0.0.1";

	try
	{
		Port   port(configuredPort);
		IpAddr ipAddr(configuredHost);

		Server server(serverConfigs);
		if (!server.isValid())
		{
			logger.log("Server initialization failed.", ERROR);
			delete httpConfig;
			return 1;
		}

		logger.log("Server initialized successfully on " + ipAddr.getValue() + ":" + port.toString(), INFO);
		server.displayServerStatus(serverConfigs);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Startup error: " << e.what() << std::endl;
		logger.log(std::string("Startup error: ") + e.what(), ERROR);
		delete httpConfig;
		return 1;
	}

	delete httpConfig;
	return 0;
}
