/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTResult.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 00:13:21 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:20:42 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTRESULT_HPP
#define ASTRESULT_HPP

#include "ResultBase.hpp"

#include "../../domain/entities/ast/base/ASTNode.hpp"

#include "../../domain/errors/ErrorList.hpp"

class ASTResult : public ResultBase
{
	private:
		ASTNode*  _astRoot;
		ErrorList _errorList;

	public:
		ASTResult(const ErrorList& error);
		ASTResult(ASTNode* astRoot);
		~ASTResult();

		ASTNode*		 unwrap();
		ASTNode*		 release();
		const ErrorList& error() const;
};

#endif /* ASTRESULT_HPP */
