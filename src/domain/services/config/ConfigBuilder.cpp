/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 05:32:37 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 18:52:24 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBuilder.hpp"

#include <cstdlib>

#include "../../../domain/entities/ast/node/ASTBlock.hpp"
#include "../../../domain/entities/ast/node/ASTDirective.hpp"
#include "../../../domain/entities/ast/node/ASTRoot.hpp"
#include "../../../domain/entities/ast/node/ASTValue.hpp"

// ------------------------ LIFECYCLE ------------------------ //

ConfigBuilder::ConfigBuilder() {}
ConfigBuilder::~ConfigBuilder() {}

// ------------------------ PRIVATE HELPERS ------------------------ //

static size_t _parseSize(const std::string& s)
{
	if (s.empty())
		return 0;
	char*  end;
	size_t val = static_cast< size_t >(std::strtoul(s.c_str(), &end, 10));
	if (*end == 'm' || *end == 'M')
		val *= 1024 * 1024;
	else if (*end == 'k' || *end == 'K')
		val *= 1024;
	return val;
}

std::string ConfigBuilder::_directive(const ASTNode& block, const std::string& name) const
{
	const ASTBlock&				   b		= static_cast< const ASTBlock& >(block);
	const std::vector< ASTNode* >& children = b.getChildren();

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (children[i]->getType() != AST_NODETYPE_DIRECTIVE)
			continue;
		const ASTDirective&				d	 = static_cast< const ASTDirective& >(*children[i]);
		const std::vector< ASTValue* >& vals = d.getValues();
		if (d.getName() == name && !vals.empty())
			return vals[0]->getValue();
	}
	return "";
}

std::vector< std::string > ConfigBuilder::_directiveArgs(const ASTNode& block, const std::string& name) const
{
	std::vector< std::string >	   result;
	const ASTBlock&				   b		= static_cast< const ASTBlock& >(block);
	const std::vector< ASTNode* >& children = b.getChildren();

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (children[i]->getType() != AST_NODETYPE_DIRECTIVE)
			continue;
		const ASTDirective& d = static_cast< const ASTDirective& >(*children[i]);
		if (d.getName() != name)
			continue;
		const std::vector< ASTValue* >& vals = d.getValues();
		for (size_t j = 0; j < vals.size(); ++j)
			result.push_back(vals[j]->getValue());
		break;
	}
	return result;
}

// ------------------------ BUILDERS ------------------------ //

LocationBlock ConfigBuilder::_buildLocation(const ASTNode& node) const
{
	const ASTBlock&				   b		= static_cast< const ASTBlock& >(node);
	const std::vector< ASTNode* >& children = b.getChildren();
	LocationBlock				   loc;

	const std::vector< ASTValue* >& params = b.getParameters();
	if (!params.empty())
		loc.path = params[0]->getValue();

	loc.root	  = _directive(node, "root");
	loc.autoindex = (_directive(node, "autoindex") == "on");
	loc.redirectUri = "";
	loc.redirectCode = 302;

	std::vector< std::string > methods = _directiveArgs(node, "allow_methods");
	for (size_t i = 0; i < methods.size(); ++i)
		loc.allowedMethods.insert(methods[i]);

	std::vector< std::string > redirectArgs = _directiveArgs(node, "return");
	if (!redirectArgs.empty())
	{
		if (redirectArgs.size() == 1)
		{
			loc.redirectUri = redirectArgs[0];
		}
		else
		{
			char* end = NULL;
			long parsedCode = std::strtol(redirectArgs[0].c_str(), &end, 10);
			if (end != NULL && *end == '\0' && parsedCode >= 300 && parsedCode < 400)
				loc.redirectCode = static_cast< int >(parsedCode);
			loc.redirectUri = redirectArgs[1];
		}
	}

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (children[i]->getType() != AST_NODETYPE_DIRECTIVE)
			continue;
		const ASTDirective& d = static_cast< const ASTDirective& >(*children[i]);
		if (d.getName() != "error_page")
			continue;
		const std::vector< ASTValue* >& vals = d.getValues();
		if (vals.size() < 2)
			continue;
		int code			 = std::atoi(vals[0]->getValue().c_str());
		loc.errorPages[code] = vals[1]->getValue();
	}

	return loc;
}

ServerBlock ConfigBuilder::_buildServer(const ASTNode& node) const
{
	const ASTBlock&				   b		= static_cast< const ASTBlock& >(node);
	const std::vector< ASTNode* >& children = b.getChildren();
	ServerBlock					   srv;

	std::string portStr = _directive(node, "listen");
	srv.port			= portStr.empty() ? 80 : std::atoi(portStr.c_str());
	srv.host			= _directive(node, "server_name");
	srv.serverName		= _directive(node, "server_name");
	srv.root			= _directive(node, "root");
	srv.index			= _directiveArgs(node, "index");

	std::string cmbsStr	  = _directive(node, "client_max_body_size");
	srv.clientMaxBodySize = _parseSize(cmbsStr);

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (children[i]->getType() == AST_NODETYPE_DIRECTIVE)
		{
			const ASTDirective& d = static_cast< const ASTDirective& >(*children[i]);
			if (d.getName() != "error_page")
				continue;
			const std::vector< ASTValue* >& vals = d.getValues();
			if (vals.size() < 2)
				continue;
			int code			 = std::atoi(vals[0]->getValue().c_str());
			srv.errorPages[code] = vals[1]->getValue();
		}
		else if (children[i]->getType() == AST_NODETYPE_BLOCK)
		{
			const ASTBlock& child = static_cast< const ASTBlock& >(*children[i]);
			if (child.getName() == "location")
				srv.locations.push_back(_buildLocation(*children[i]));
		}
	}

	return srv;
}

HttpBlock ConfigBuilder::_buildHttp(const ASTNode& node) const
{
	const ASTBlock&				   b		= static_cast< const ASTBlock& >(node);
	const std::vector< ASTNode* >& children = b.getChildren();
	HttpBlock					   http;

	std::string cmbsStr	   = _directive(node, "client_max_body_size");
	http.clientMaxBodySize = _parseSize(cmbsStr);

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (children[i]->getType() == AST_NODETYPE_DIRECTIVE)
		{
			const ASTDirective& d = static_cast< const ASTDirective& >(*children[i]);
			if (d.getName() != "error_page")
				continue;
			const std::vector< ASTValue* >& vals = d.getValues();
			if (vals.size() < 2)
				continue;
			int code			  = std::atoi(vals[0]->getValue().c_str());
			http.errorPages[code] = vals[1]->getValue();
		}
		else if (children[i]->getType() == AST_NODETYPE_BLOCK)
		{
			const ASTBlock& child = static_cast< const ASTBlock& >(*children[i]);
			if (child.getName() == "server")
			{
				http.server = _buildServer(*children[i]);
				http.host	= http.server.host;
			}
		}
	}

	return http;
}

// ------------------------ PUBLIC ------------------------ //

HttpBlock* ConfigBuilder::build(const ASTNode* ast)
{
	if (!ast)
		return NULL;

	const ASTRoot&				   root	 = static_cast< const ASTRoot& >(*ast);
	const std::vector< ASTNode* >& stmts = root.getStatements();

	for (size_t i = 0; i < stmts.size(); ++i)
	{
		if (stmts[i]->getType() != AST_NODETYPE_BLOCK)
			continue;
		const ASTBlock& block = static_cast< const ASTBlock& >(*stmts[i]);

		if (block.getName() == "http")
		{
			HttpBlock* result = new HttpBlock();
			*result			  = _buildHttp(*stmts[i]);
			return result;
		}
		if (block.getName() == "server")
		{
			HttpBlock* result		  = new HttpBlock();
			result->server			  = _buildServer(*stmts[i]);
			result->host			  = result->server.host;
			result->clientMaxBodySize = result->server.clientMaxBodySize;
			result->errorPages		  = result->server.errorPages;
			return result;
		}
	}

	return NULL;
}
