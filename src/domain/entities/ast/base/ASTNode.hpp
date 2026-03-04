/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 18:20:00 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:35:55 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTNODE_HPP
#define ASTNODE_HPP

#include "../../../value_objects/ASTNodeType.hpp"
#include "../../../value_objects/ASTValueType.hpp"

#include "../../SourceLocation.hpp"

class ASTNode
{
	protected:
		ASTNodeType	   _type;
		SourceLocation _location;

	public:
		ASTNode(ASTNodeType type, const SourceLocation& location);
		virtual ~ASTNode();

		ASTNodeType	   getType() const;
		SourceLocation getLocation() const;

		virtual std::string toString() const = 0;
};

#endif /* ASTNODE_HPP */
