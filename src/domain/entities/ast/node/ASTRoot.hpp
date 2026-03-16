/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTRoot.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 21:08:07 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:28:52 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTROOT_HPP
#define ASTROOT_HPP

#include <vector>

#include "../../../value_objects/config/ASTNodeType.hpp"
#include "../base/ASTNode.hpp"

// verificar se isso vai ser realmente necessário (host virtual é bonus?)

class ASTRoot : public ASTNode
{
	private:
		std::vector< ASTNode* > _statements;

	public:
		ASTRoot();
		~ASTRoot();

		std::vector< ASTNode* > getStatements() const;
		void					addStatement(ASTNode* statement);

		std::string toString(int indentation) const;
};

#endif /* ASTROOT_HPP */
