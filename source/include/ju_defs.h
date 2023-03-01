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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define JU_API
#define JU_DEPRECATED
#define JU_WEAK

typedef signed long ju_ssize_t;

struct program_info {
	const char* name;
	const char* description;
	const char* author;
	const char* license;
	const char** contributors;
	float version;
	const char* source_url;
};

// to make it simply acessable from ANYWHERE :D
extern const struct program_info program_info;
