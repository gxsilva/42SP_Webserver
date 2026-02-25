/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:29:24 by cadete            #+#    #+#             */
/*   Updated: 2026/02/25 01:40:01 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MACROS_HPP
#define MACROS_HPP

/**
 * Macro helper para prevenir cópia (C++98 style)
 * Uso: DISABLE_COPY(MinhaClasse)
 */
#define DISABLE_COPY(ClassName)                                                                    \
private:                                                                                           \
	ClassName(const ClassName&);                                                                   \
	ClassName& operator=(const ClassName&)

#endif /* MACROS_HPP */
