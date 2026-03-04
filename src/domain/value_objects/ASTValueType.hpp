/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTValueType.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:07:26 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 20:39:46 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTVALUETYPE_HPP
#define ASTVALUETYPE_HPP

enum ASTValueType
{
	AST_VALUE_STRING = 0,
	AST_VALUE_IDENTIFIER,
	AST_VALUE_NUMBER,
	AST_VALUE_PATH,
};

#endif /* ASTVALUETYPE_HPP */
