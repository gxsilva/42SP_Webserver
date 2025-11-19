/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cadete <cadete@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 02:21:54 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/11/19 18:31:35 by cadete           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

#include "colors.hpp"

enum LogLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR
};

class Logger
{
	private:
		std::string	  _logFileName;
		std::ofstream _fdOutput;
		bool		  _fileLoggingEnabled;

		Logger();
		~Logger();
		Logger(const Logger& logger);
		Logger& operator=(const Logger& logger);

		std::string _levelToColor(LogLevel lvl);
		std::string _levelToString(LogLevel lvl);

		std::string _currentTime();

		std::string _genUniqueLogFileName(void) const;

	public:
		static Logger& getInstance();
		void		   log(const std::string& msg, LogLevel lvl = INFO);
		void		   enableFileLogging();
};

#endif /* LOGGER_HPP */