/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 18:20:00 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:29:35 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTNODE_HPP
#define ASTNODE_HPP

#include "../../../value_objects/config/ASTNodeType.hpp"
#include "../../../value_objects/config/ASTValueType.hpp"

#include "../../common/SourceLocation.hpp"

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

		virtual std::string toString(int indentation) const = 0;

		static std::string indentString(int indentation);
};

#endif /* ASTNODE_HPP */
