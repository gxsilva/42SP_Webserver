/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidatorResult.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 04:19:29 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/16 04:50:12 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATORRESULT_HPP
#define VALIDATORRESULT_HPP

#include "../ResultBase.hpp"

#include "../../../domain/errors/common/ErrorList.hpp"

#include "../../../../includes/macros.hpp"

class ValidatorResult : public ResultBase
{
	private:
		ErrorList _errorList;

		// DISABLE_COPY(ValidatorResult);
	public:
		ValidatorResult(const ErrorList& error);
		ValidatorResult();
		~ValidatorResult();

		const ErrorList& error() const;
};

#endif /* VALIDATORRESULT_HPP */
