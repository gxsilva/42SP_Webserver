/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResultBase.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 00:37:58 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/25 01:15:14 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESULT_HPP
#define RESULT_HPP

class ResultBase
{
	protected:
		bool _is_ok;

		ResultBase(bool ok) : _is_ok(ok) {}

	public:
		virtual ~ResultBase() {}

		bool isOk() const { return _is_ok; }
		bool isErr() const { return !_is_ok; }
};

#endif /* RESULT_HPP */
