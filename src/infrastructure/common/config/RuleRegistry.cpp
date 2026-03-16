/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RuleRegistry.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 22:55:20 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 03:27:34 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RuleRegistry.hpp"

#include <set>
#include <string>

// ------------------------ OCCF ------------------------ //

RuleRegistry::RuleRegistry()
{
	this->_registerContextRuleServices();
	this->_registerCardinalityRules();
	this->_registerConflictRules();
	this->_registerDependencyRules();
}

RuleRegistry::~RuleRegistry() {}

// ------------------------ GETTERS ------------------------ //
const RuleTable& RuleRegistry::getContextTable() const { return this->_context; }

const RuleTable& RuleRegistry::getCardinalityTable() const { return this->_cardinality; }

const RuleTable& RuleRegistry::getConflictTable() const { return this->_conflict; }

const RuleTable& RuleRegistry::getDependencyTable() const { return this->_dependency; }

// ------------------------ REGISTER RULES ------------------------ //

/*
void _registerContextRuleServices();
		void _registerCardinalityRules();
		void _registerConflictRules();
		void _registerDependencyRules();

		CARDINALITY
		{0,0} - not allowed
		{0,1} - optional
		{1,1} - required
		{1,}  - at least one
		{0,}  - any number

		{1, -1} - one or more
		{-1, -1} - any number
*/

void RuleRegistry::_registerContextRuleServices()
{
	std::set< std::string > root;
	root.insert("root");

	std::set< std::string > server;
	server.insert("server");

	std::set< std::string > server_location;
	server_location.insert("server");
	server_location.insert("location");

	std::set< std::string > location;
	location.insert("location");
	_context.addContext("server", root);
	_context.addContext("server_name", server);
	_context.addContext("listen", server);
	_context.addContext("host", server);
	_context.addContext("client_max_body_size", server);
	_context.addContext("location", server);

	_context.addContext("root", server_location);
	_context.addContext("index", server_location);
	_context.addContext("error_page", server_location);

	_context.addContext("allow_methods", location);
	_context.addContext("autoindex", server_location);
	_context.addContext("return", location);
	_context.addContext("cgi_path", location);
	_context.addContext("cgi_ext", location);
	_context.addContext("upload_path", location);
}

void RuleRegistry::_registerCardinalityRules()
{
	_cardinality.setCardinality("server_name", 1, 1);
	_cardinality.setCardinality("listen", 1, 1);
	_cardinality.setCardinality("host", 1, 1);
	_cardinality.setCardinality("client_max_body_size", 1, 1);
	_cardinality.setCardinality("location", 0, -1);

	_cardinality.setCardinality("root", 0, 1);
	_cardinality.setCardinality("index", 0, 1);
	_cardinality.setCardinality("error_page", 0, -1);

	_cardinality.setCardinality("allow_methods", 0, 1);
	_cardinality.setCardinality("autoindex", 0, 1);
	_cardinality.setCardinality("return", 0, 1);
	_cardinality.setCardinality("cgi_path", 0, 1);
	_cardinality.setCardinality("cgi_ext", 0, 1);
	_cardinality.setCardinality("upload_path", 0, 1);
}

void RuleRegistry::_registerConflictRules()
{
	std::set< std::string > aliasSet;
	aliasSet.insert("alias");
	_conflict.addConflict("root", aliasSet);
}

void RuleRegistry::_registerDependencyRules() {}
