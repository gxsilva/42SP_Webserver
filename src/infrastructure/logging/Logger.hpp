/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 03:11:50 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 04:31:44 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

#include "../../application/ports/ILogger.hpp"

class Logger : public ILogger
{
	private:
		std::string	  _logFileName;
		std::ofstream _fdOutput;
		LogLevel	  _filterLevel;
		bool		  _fileLoggingEnabled;

		bool _tryOpen(const std::string& path);

		static std::string _levelToString(LogLevel lvl);
		static std::string _levelToColor(LogLevel lvl);
		static std::string _currentTime();
		static std::string _genUniqueLogFileName();

		Logger(const Logger&);
		Logger& operator=(const Logger&);

	public:
		Logger();
		~Logger();

		void log(const std::string& msg, LogLevel lvl = INFO);
		void enableFileLogging();
		void setFilterLevel(LogLevel lvl);
};

#endif /* LOGGER_HPP */
