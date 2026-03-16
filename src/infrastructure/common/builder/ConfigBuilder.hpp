/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:24:24 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 05:32:58 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGBUILDER_HPP
#define CONFIGBUILDER_HPP

#include <string>
#include <vector>

#include "../../../domain/entities/ast/base/ASTNode.hpp"
#include "../../../domain/entities/server/HttpBlock.hpp"
#include "../../../domain/entities/server/LocationBlock.hpp"
#include "../../../domain/entities/server/ServerBlock.hpp"

class ConfigBuilder
{
	private:
		HttpBlock	  _buildHttp(const ASTNode& node) const;
		ServerBlock	  _buildServer(const ASTNode& node) const;
		LocationBlock _buildLocation(const ASTNode& node) const;

		std::string _directive(const ASTNode& block, const std::string& name) const;

		std::vector< std::string > _directiveArgs(const ASTNode& block, const std::string& name) const;

	public:
		ConfigBuilder();
		~ConfigBuilder();

		HttpBlock* build(const ASTNode* ast);
};

#endif /* CONFIGBUILDER_HPP */
