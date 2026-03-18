/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigResult.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:06:00 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 20:33:44 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigResult.hpp"

ConfigResult::ConfigResult(HttpBlock* config) : ResultBase(true), _config(config) {}

ConfigResult::ConfigResult(const CompilerError& err) : ResultBase(false), _config(NULL) { _errorList.addError(err); }

ConfigResult::ConfigResult(const ErrorList& err) : ResultBase(false), _config(NULL), _errorList(err) {}

ConfigResult::~ConfigResult()
{
	// if (_config)
	// 	delete _config;
}

HttpBlock* ConfigResult::unwrap()
{
	if (!isOk())
		return NULL;
	return _config;
}

const ErrorList& ConfigResult::error() const { return _errorList; }
