/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 18:20:00 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/05 22:13:04 by lsilva-x         ###   ########.fr       */
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

		virtual std::string toString(int ident) const = 0;

		static std::string indentString(int ident);
};

#endif /* ASTNODE_HPP */
