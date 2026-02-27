/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 21:08:40 by lsilva-x          #+#    #+#             */
/*   Updated: 2026/02/26 23:04:27 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEVALIDATOR_HPP
#define FILEVALIDATOR_HPP

#include <fstream>
#include <string>
#include <unistd.h> // for access()

#include "../../../includes/macros.hpp"
#include "../../domain/errors/CompilerError.hpp"

struct FileValidator
{
		static CompilerError* validateFile(const std::string& filePath);
		static CompilerError* validateReadPermission(const std::string& filepath);
		static CompilerError* validateExists(const std::string& filepath);
};

#endif /* FILEVALIDATOR_HPP */
