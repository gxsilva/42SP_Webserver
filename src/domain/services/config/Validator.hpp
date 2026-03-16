/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Validator.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 04:18:57 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:52:52 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include "../../../infrastructure/common/config/ValidatorResult.hpp"
#include "../../../infrastructure/io/config/SemanticAnalyzer.hpp"
#include "../../entities/ast/base/ASTNode.hpp"

class Validator
{
	private:
		const SemanticAnalyzer& _analyzer;

	public:
		Validator(const SemanticAnalyzer& analyzer);
		~Validator();
		ValidatorResult validate(const ASTNode& root);
};

#endif /* VALIDATOR_HPP */
