/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 04:25:58 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 04:27:48 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Validator.hpp"

#include "../../infrastructure/common/ValidatorResult.hpp"
#include "../../infrastructure/io/SemanticAnalyzer.hpp"
#include "../entities/ast/base/ASTNode.hpp"

Validator::Validator(const SemanticAnalyzer& analyzer) : _analyzer(analyzer) {}
Validator::~Validator() {}

ValidatorResult Validator::validate(const ASTNode& root)
{
	ErrorList errors;
	_analyzer.analyze(root, errors);

	if (!errors.hasErrors())
		return ValidatorResult();
	else
		return ValidatorResult(errors);
}
