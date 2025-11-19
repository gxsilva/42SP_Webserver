/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cadete <cadete@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 02:56:47 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/11/19 18:34:20 by cadete           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/utils/Logger.hpp"

// ------------------------ Logger Implementation ----------------------- //

//------------------------ OCCF ------------------------ //
Logger::Logger() : _fileLoggingEnabled(false){};

Logger::~Logger()
{
	if (_fileLoggingEnabled)
		_fdOutput.close();
};

Logger::Logger(const Logger& logger) : _fileLoggingEnabled(logger._fileLoggingEnabled){};

Logger& Logger::operator=(const Logger& logger)
{
	(void)logger;
	return (*this);
};

//------------------------ METHODS ------------------------ //

Logger& Logger::getInstance()
{
	static Logger instance;

	return (instance);
}

std::string Logger::_levelToColor(LogLevel lvl)
{
	switch (lvl)
	{
	case DEBUG:
		return CYAN;
	case INFO:
		return GREEN;
	case WARN:
		return YELLOW;
	case ERROR:
		return RED;
	}
	return "\033[0m";
}

std::string Logger::_levelToString(LogLevel lvl)
{
	switch (lvl)
	{
	case DEBUG:
		return "DEBUG";
	case INFO:
		return "INFO";
	case WARN:
		return "WARN";
	case ERROR:
		return "ERROR";
	}
	return "UNKNOWN";
}

std::string Logger::_currentTime()
{
	std::time_t now = std::time(0);
	char		buf[20];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return std::string(buf);
}

void Logger::log(const std::string& msg, LogLevel lvl)
{
	std::string color	 = _levelToColor(lvl);
	std::string levelStr = _levelToString(lvl);
	std::string timeStr	 = _currentTime();

	if (lvl == ERROR)
	{
		if (_fileLoggingEnabled && _fdOutput.is_open())
			_fdOutput << "[" + timeStr + "] [" + levelStr + "] " + msg << std::endl;
		std::cerr << color + "[" + timeStr + "] [" + levelStr + "] " + msg + RESET << std::endl;
	}
	else
	{
		if (_fileLoggingEnabled && _fdOutput.is_open())
			_fdOutput << "[" + timeStr + "] [" + levelStr + "] " + msg << std::endl;
		std::cout << color + "[" + timeStr + "] [" + levelStr + "] " + msg + RESET << std::endl;
	}
}

std::string Logger::_genUniqueLogFileName(void) const
{
	std::time_t now = std::time(0);
	char		buf[20];
	std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&now));
	return "log_" + std::string(buf) + ".txt";
}

void Logger::enableFileLogging()
{
	_logFileName = _genUniqueLogFileName();
	_fdOutput.open(_logFileName.c_str(), std::ios::out | std::ios::app);
	if (!_fdOutput.is_open())
	{
		std::cerr << RED << "Failed to open log file: " << _logFileName << RESET << std::endl;
		return;
	}
	_fileLoggingEnabled = true;
}