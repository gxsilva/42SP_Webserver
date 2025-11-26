/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 18:19:13 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/11/26 20:42:13 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "./utils/Logger.hpp"

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	std::cout << "Hello world\n";

	Logger& Logger = Logger::getInstance();

	Logger.setFilterLevel(WARN);
	Logger.enableFileLogging();

	Logger.log("Starting the application...");
	Logger.log("User logged in", INFO);
	Logger.log("Disk space running low", WARN);
	Logger.log("Unable to connect to database", ERROR);
	Logger.log("Shutting down the application...");

	Logger.log("Initializing configuration...", DEBUG);
	Logger.log("Configuration loaded successfully", INFO);
	Logger.log("Network connection established", INFO);
	Logger.log("Memory usage exceeds threshold", WARN);
	Logger.log("Critical error: System overheating", ERROR);
	Logger.log("Application terminated unexpectedly", ERROR);
	Logger.log("Restarting application...", DEBUG);
	Logger.log("Application restarted successfully", INFO);
}
