/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ILogger.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 03:09:09 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/27 03:12:53 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include <string>

enum LogLevel
{
	DEBUG = 0,
	INFO,
	WARN,
	ERROR,
	NONE
};

class ILogger
{
	public:
		virtual ~ILogger() {}

		virtual void log(const std::string& msg, LogLevel lvl) = 0;
		virtual void enableFileLogging()					   = 0;
		virtual void setFilterLevel(LogLevel lvl)			   = 0;
};

#endif /* ILOGGER_HPP */
