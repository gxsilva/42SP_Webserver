/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTBlock.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:57:47 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/05 22:17:38 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTBLOCK_HPP
#define ASTBLOCK_HPP

#include <vector>

#include "../base/ASTNode.hpp"

class ASTValue;

class ASTBlock : public ASTNode
{
	private:
		std::string			   name_;		// "server", "location", "http"
		std::vector<ASTValue*> parameters_; // Para location /api → ["/api"]
		std::vector<ASTNode*>  children_;	// Diretivas e blocos internos

	public:
		ASTBlock(const std::string& name, const SourceLocation& loc);
		~ASTBlock();
		const std::string&			  getName() const;
		const std::vector<ASTValue*>& getParameters() const;
		const std::vector<ASTNode*>&  getChildren() const;
		std::string					  toString(int ident) const;
		void						  addParameter(ASTValue* param);
		void						  addChild(ASTNode* child);
};

#endif /* ASTBLOCK_HPP */
