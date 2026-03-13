/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SemanticAnalyzer.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 03:00:15 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 03:10:27 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEMANTICANALYZER_HPP
#define SEMANTICANALYZER_HPP

#include <string>
#include <vector>

#include "../../domain/errors/ErrorList.hpp"
#include "../../domain/ports/ISemanticRule.hpp"

class SemanticAnalyzer
{
	private:
		std::vector< ISemanticRule* > _rules;

		void _visit(const ASTNode& node, const std::string& context, ErrorList& errors) const;

	public:
		explicit SemanticAnalyzer(const std::vector< ISemanticRule* >& rules);
		~SemanticAnalyzer();

		void analyze(const ASTNode& root, ErrorList& errors) const;
};

#endif /* SEMANTICANALYZER_HPP */
