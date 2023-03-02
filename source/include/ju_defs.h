/* General definitions :p 
 * Copyright (C) UtoECat 2022. All rights Reserved!

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

/**
 * @file ju_defs.h
 * Main header file with general definitions for the JackUtils API.
 * Included automaticlu by @ref jackutils.h file!
 * @see jackutils.h
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define JU_API
#define JU_DEPRECATED
#define JU_WEAK

/**
 * Signed size type.
 */
typedef signed long ju_ssize_t;

/**
 * Structure required for custom cllients, that uses jackutils!
 * It provides libjackutils with general information about your program.
 */
struct program_info {
	const char* name; /**< Name of your program. */
	const char* description; /**< General description of your program. */
	const char* author;  /**< Name of authors of your program. */
	const char* license;  /**< License of your program. */
	const char** contributors;  /**< Contributors of your program. (array of strings, terminated with NULL) */
	float version;  /**< Version of your program. Only versions like number.number are supported! */
	const char* source_url;  /**< Url to your project page, github, etc. */
};

extern const struct program_info program_info; /**< to make it simply acessable from ANYWHERE :D */
