/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASTDirective.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 20:44:43 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/04 00:36:45 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ASTDIRECTIVE_HPP
#define ASTDIRECTIVE_HPP

#include <vector>

#include "../../../value_objects/ASTNodeType.hpp"
#include "../../../value_objects/ASTValueType.hpp"
#include "../base/ASTNode.hpp"

class ASTValue;

class ASTDirective : public ASTNode
{
	private:
		std::string			   _name;	// "listen", "server_name"
		std::vector<ASTValue*> _values; // [8080], [example.com, www.example.com]

	public:
		ASTDirective(const std::string& name, const SourceLocation& loc);
		~ASTDirective();

		const std::string&			  getName() const;
		const std::vector<ASTValue*>& getValues() const;

		std::string toString() const;

		void addValue(ASTValue* value);
};

#endif /* ASTDIRECTIVE_HPP */
