/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 03:11:50 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 03:35:59 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

// ----------------------- ANSI COLOR CODES ----------------------- //
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define GREY "\033[90m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"

// ------------------------ OCCF ------------------------ //

Logger::Logger() : _filterLevel(DEBUG), _fileLoggingEnabled(false) {}

Logger::~Logger()
{
	if (_fdOutput.is_open())
		_fdOutput.close();
}

// ------------------------ PRIVATE HELPERS ------------------------ //

std::string Logger::_levelToString(LogLevel lvl) const
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
	default:
		return "NONE";
	}
}

std::string Logger::_levelToColor(LogLevel lvl) const
{
	switch (lvl)
	{
	case DEBUG:
		return GREY;
	case INFO:
		return CYAN;
	case WARN:
		return YELLOW;
	case ERROR:
		return RED;
	default:
		return MAGENTA;
	}
}

std::string Logger::_currentTime() const
{
	std::time_t now = std::time(NULL);
	char		buf[20];
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	return std::string(buf);
}

std::string Logger::_genUniqueLogFileName() const
{
	std::time_t now = std::time(NULL);
	char		buf[32];
	std::strftime(buf, sizeof(buf), "log_%Y%m%d_%H%M%S.txt", std::localtime(&now));
	return std::string(buf);
}

bool Logger::_tryOpen(const std::string& path)
{
	_fdOutput.open(path.c_str(), std::ios::out | std::ios::app);
	if (_fdOutput.is_open())
	{
		_logFileName = path;
		return true;
	}
	return false;
}

// ------------------------ PUBLIC INTERFACE ------------------------ //

void Logger::log(const std::string& msg, LogLevel lvl)
{
	if (lvl < _filterLevel)
		return;

	std::string timestamp = _currentTime();
	std::string label	  = _levelToString(lvl);

	// Console output with color
	std::cout << BOLD << _levelToColor(lvl) << "[" << timestamp << "] " << "[" << label << "] "
			  << RESET << msg << "\n";

	// File output (plain, no ANSI codes)
	if (_fileLoggingEnabled && _fdOutput.is_open())
	{
		_fdOutput << "[" << timestamp << "] " << "[" << label << "] " << msg << "\n";
		_fdOutput.flush();
	}
}

void Logger::enableFileLogging()
{
	if (_fileLoggingEnabled)
		return;

	const std::string name	  = _genUniqueLogFileName();
	const std::string primary = "log/" + name;

	if (!_tryOpen(primary) && !_tryOpen(name))
	{
		log("Logger: could not open log file in 'log/' or './' — file logging disabled", WARN);
		return;
	}

	_fileLoggingEnabled = true;

	std::string location = _logFileName;
	log("Logger: file logging enabled, writing to '" + location + "'", INFO);
}

void Logger::setFilterLevel(LogLevel lvl) { _filterLevel = lvl; }
