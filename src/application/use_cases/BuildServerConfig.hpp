/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BuildServerConfig.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:01:02 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 20:33:45 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILDSERVERCONFIG_HPP
#define BUILDSERVERCONFIG_HPP

#include "../../infrastructure/common/config/ConfigResult.hpp"
#include "../../infrastructure/logging/Logger.hpp"

class BuildServerConfig
{
	public:
		static ConfigResult execute(const std::string& filePath, Logger* logger);
};

#endif /* BUILDSERVERCONFIG_HPP */
