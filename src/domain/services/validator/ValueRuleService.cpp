/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValueRuleService.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 03:50:00 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 02:43:40 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValueRuleService.hpp"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include "../../entities/ast/node/ASTBlock.hpp"
#include "../../entities/ast/node/ASTDirective.hpp"
#include "../../entities/ast/node/ASTValue.hpp"
#include "../../errors/common/CompilerError.hpp"
#include "../../value_objects/config/ASTNodeType.hpp"

namespace
{
	bool isDigitsOnly(const std::string& value)
	{
		if (value.empty())
			return false;
		for (size_t i = 0; i < value.size(); ++i)
		{
			if (!std::isdigit(static_cast< unsigned char >(value[i])))
				return false;
		}
		return true;
	}

	bool isValidPort(const std::string& value)
	{
		if (!isDigitsOnly(value))
			return false;

		std::stringstream ss(value);
		long			  port = 0;

		ss >> port;
		if (ss.fail())
			return false;
		return port >= 1 && port <= 65535;
	}

	bool isValidIpv4(const std::string& value)
	{
		std::stringstream ss(value);
		std::string		  token;
		int				  parts = 0;

		while (std::getline(ss, token, '.'))
		{
			std::stringstream partStream(token);
			int				  octet = -1;
			char			  remainder;

			if (token.empty())
				return false;
			if (token.size() > 1 && token[0] == '0')
				return false;
			partStream >> octet;
			if (partStream.fail())
				return false;
			if (partStream >> remainder)
				return false;
			if (octet < 0 || octet > 255)
				return false;
			++parts;
		}
		return parts == 4;
	}

	bool isPathLike(const std::string& value) { return !value.empty(); }

	bool isLocationPath(const std::string& value) { return !value.empty() && value[0] == '/'; }

	bool isValidServerName(const std::string& value)
	{
		if (value.empty())
			return false;
		for (size_t i = 0; i < value.size(); ++i)
		{
			const char c = value[i];
			if (std::isalnum(static_cast< unsigned char >(c)))
				continue;
			if (c == '-' || c == '_' || c == '.' || c == '*')
				continue;
			return false;
		}
		return true;
	}

	bool isValidBodySize(const std::string& value)
	{
		if (value.empty())
			return false;
		if (isDigitsOnly(value))
			return true;
		if (value.size() < 2)
			return false;
		const char suffix = value[value.size() - 1];
		if (suffix != 'k' && suffix != 'K' && suffix != 'm' && suffix != 'M' && suffix != 'g' && suffix != 'G')
			return false;
		return isDigitsOnly(value.substr(0, value.size() - 1));
	}

	bool isValidStatusCode(const std::string& value)
	{
		if (!isDigitsOnly(value))
			return false;
		if (value.size() != 3)
			return false;
		const int code = std::atoi(value.c_str());
		return code >= 300 && code <= 599;
	}

	bool isValidMethod(const std::string& value)
	{
		static const char* allowedMethods[] = {"GET", "POST", "DELETE", "PUT", "HEAD"};
		for (size_t i = 0; i < sizeof(allowedMethods) / sizeof(allowedMethods[0]); ++i)
		{
			if (value == allowedMethods[i])
				return true;
		}
		return false;
	}

	bool isValidCgiExtension(const std::string& value)
	{
		if (value.size() < 2)
			return false;
		if (value[0] != '.')
			return false;
		for (size_t i = 1; i < value.size(); ++i)
		{
			if (!std::isalnum(static_cast< unsigned char >(value[i])) && value[i] != '_')
				return false;
		}
		return true;
	}

	std::string getFirstValueOrEmpty(const std::vector< ASTValue* >& values)
	{
		if (values.empty())
			return "";
		return values[0]->getValue();
	}

	SourceLocation getFirstValueLocationOrDirective(const ASTDirective& directive)
	{
		const std::vector< ASTValue* >& values = directive.getValues();
		if (values.empty())
			return directive.getLocation();
		return values[0]->getLocation();
	}

	void addInvalidValueError(ErrorList& errors, const ASTDirective& directive, const std::string& value,
							  const std::string& reason)
	{
		errors.addError(CompilerError::directiveInvalidValueError(directive.getName(), value, reason,
																  getFirstValueLocationOrDirective(directive)));
	}
} // namespace

void ValueRuleService::apply(const ASTNode& node, const std::string& context, ErrorList& errors)
{
	(void)context;
	if (node.getType() == AST_NODETYPE_BLOCK)
	{
		const ASTBlock& block = static_cast< const ASTBlock& >(node);
		if (block.getName() != "location")
			return;

		const std::vector< ASTValue* >& params = block.getParameters();
		if (params.size() != 1)
		{
			errors.addError(CompilerError::directiveInvalidValueError(
				"location", "", "location block requires exactly one URL parameter", block.getLocation()));
			return;
		}
		if (!isLocationPath(params[0]->getValue()))
		{
			errors.addError(CompilerError::directiveInvalidValueError(
				"location", params[0]->getValue(), "location URL must start with '/'", params[0]->getLocation()));
		}
		return;
	}

	if (node.getType() != AST_NODETYPE_DIRECTIVE)
		return;

	const ASTDirective&				directive = static_cast< const ASTDirective& >(node);
	const std::string&				name	  = directive.getName();
	const std::vector< ASTValue* >& values	  = directive.getValues();

	if (name == "listen")
	{
		if (values.size() != 1 || !isValidPort(getFirstValueOrEmpty(values)))
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "listen expects one TCP port between 1 and 65535");
	}
	else if (name == "host")
	{
		if (values.size() != 1)
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "host expects one IPv4 address or 'localhost'");
		else
		{
			const std::string host = values[0]->getValue();
			if (host != "localhost" && !isValidIpv4(host))
				addInvalidValueError(errors, directive, host, "host must be a valid IPv4 address or 'localhost'");
		}
	}
	else if (name == "server_name")
	{
		if (values.size() != 1 || !isValidServerName(getFirstValueOrEmpty(values)))
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "server_name expects one valid hostname token");
	}
	else if (name == "client_max_body_size")
	{
		if (values.size() != 1 || !isValidBodySize(getFirstValueOrEmpty(values)))
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "client_max_body_size expects digits with optional k/m/g suffix");
	}
	else if (name == "root" || name == "index" || name == "upload_path")
	{
		if (values.size() != 1 || !isPathLike(getFirstValueOrEmpty(values)))
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values), name + " expects one non-empty path");
	}
	else if (name == "error_page")
	{
		if (values.size() < 2)
		{
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "error_page expects one or more status codes and a path");
			return;
		}
		for (size_t i = 0; i + 1 < values.size(); ++i)
		{
			if (!isValidStatusCode(values[i]->getValue()))
			{
				addInvalidValueError(errors, directive, values[i]->getValue(),
									 "error_page status code must be a 3-digit code between 300 and 599");
				return;
			}
		}
		if (!isPathLike(values[values.size() - 1]->getValue()))
		{
			addInvalidValueError(errors, directive, values[values.size() - 1]->getValue(),
								 "error_page path must be non-empty");
		}
	}
	else if (name == "allow_methods")
	{
		if (values.empty())
		{
			addInvalidValueError(errors, directive, "", "allow_methods expects one or more HTTP methods");
			return;
		}
		for (size_t i = 0; i < values.size(); ++i)
		{
			if (!isValidMethod(values[i]->getValue()))
			{
				addInvalidValueError(errors, directive, values[i]->getValue(),
									 "allow_methods supports GET, POST, DELETE, PUT and HEAD");
				return;
			}
		}
	}
	else if (name == "autoindex")
	{
		if (values.size() != 1 || (values[0]->getValue() != "on" && values[0]->getValue() != "off"))
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values), "autoindex expects 'on' or 'off'");
	}
	else if (name == "return")
	{
		if (values.empty() || values.size() > 2)
		{
			addInvalidValueError(errors, directive, getFirstValueOrEmpty(values),
								 "return expects 'url' or 'status_code url'");
			return;
		}
		if (values.size() == 1)
		{
			if (!isPathLike(values[0]->getValue()))
				addInvalidValueError(errors, directive, values[0]->getValue(), "return URL must be non-empty");
			return;
		}
		if (!isValidStatusCode(values[0]->getValue()))
			addInvalidValueError(errors, directive, values[0]->getValue(),
								 "return status code must be a 3-digit code between 300 and 599");
		if (!isPathLike(values[1]->getValue()))
			addInvalidValueError(errors, directive, values[1]->getValue(), "return URL must be non-empty");
	}
	else if (name == "cgi_path")
	{
		if (values.empty())
		{
			addInvalidValueError(errors, directive, "", "cgi_path expects one or more executable paths");
			return;
		}
		for (size_t i = 0; i < values.size(); ++i)
		{
			if (!isPathLike(values[i]->getValue()))
			{
				addInvalidValueError(errors, directive, values[i]->getValue(),
									 "cgi_path values must be non-empty paths");
				return;
			}
		}
	}
	else if (name == "cgi_ext")
	{
		if (values.empty())
		{
			addInvalidValueError(errors, directive, "", "cgi_ext expects one or more extensions");
			return;
		}
		for (size_t i = 0; i < values.size(); ++i)
		{
			if (!isValidCgiExtension(values[i]->getValue()))
			{
				addInvalidValueError(errors, directive, values[i]->getValue(),
									 "cgi_ext values must start with '.' followed by alnum or '_'");
				return;
			}
		}
	}
}
