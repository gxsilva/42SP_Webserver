/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigResult.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:04:12 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/17 16:05:44 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGRESULT_HPP
#define CONFIGRESULT_HPP

#include "../ResultBase.hpp"

#include "../../../../includes/macros.hpp"
#include "../../../domain/entities/server/HttpBlock.hpp"
#include "../../../domain/errors/common/CompilerError.hpp"
#include "../../../domain/errors/common/ErrorList.hpp"

class ConfigResult : public ResultBase
{
	private:
		HttpBlock* _config;
		ErrorList  _errorList;

	public:
		ConfigResult(HttpBlock* config);

		ConfigResult(const CompilerError& err);
		ConfigResult(const ErrorList& error);
		virtual ~ConfigResult();

		HttpBlock*		 unwrap();
		const ErrorList& error() const;

		// DISABLE_COPY(ConfigResult);
};

#endif /* CONFIGRESULT_HPP */
