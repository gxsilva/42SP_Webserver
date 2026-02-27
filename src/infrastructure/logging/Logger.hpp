/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 03:11:50 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 03:32:36 by lsilva-x         ###   ########.fr       */
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

		std::string _levelToColor(LogLevel lvl) const;
		std::string _levelToString(LogLevel lvl) const;
		std::string _currentTime() const;
		std::string _genUniqueLogFileName() const;
		bool		_tryOpen(const std::string& path);

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
