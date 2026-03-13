/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ValidatorResult.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 04:20:43 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/03/13 04:22:19 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ValidatorResult.hpp"

ValidatorResult::ValidatorResult(const ErrorList& error) : ResultBase(false), _errorList(error) {}

ValidatorResult::ValidatorResult() : ResultBase(true), _errorList() {}

ValidatorResult::~ValidatorResult() {}

const ErrorList& ValidatorResult::error() const { return _errorList; }
