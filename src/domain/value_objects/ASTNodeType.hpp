/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTNodeType.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:06:20 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/03 20:12:35 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTNODETYPE_HPP
#define ASTNODETYPE_HPP

enum ASTNodeType
{
	AST_NODETYPE_ROOT = 0,
	AST_NODETYPE_DIRECTIVE,
	AST_NODETYPE_BLOCK,
	AST_NODETYPE_VALUE
};

#endif /* ASTNODETYPE_HPP */
