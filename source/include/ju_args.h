/* Jack Utils Argument parser extension 
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define JA_API static inline

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "dummy"
#endif

#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION 0.0
#endif

#ifndef PROGRAM_USAGE
#define PROGRAM_USAGE "no usage defined!"
#endif

#ifndef PROGRAM_HELP
#define PROGRAM_HELP "help is not defined!"
#endif

// optarg - where argument is placed
JA_API void ja_parse(int argc, char* const* argv, void (*cb) (char, const char*), const char* e) {
	if (!cb && e) return;
	char c = 0;

	size_t s = e ? strlen(e) : 0;
	char exp[s + 3];
	exp[0] = 'v';
	exp[1] = 'h';
	if (e) memcpy(exp + 2, e, s+1);

	while ((c = getopt(argc, argv, exp)) != -1) {
		switch (c) {
			case 'v':
				printf("%s %f\n", PROGRAM_NAME, PROGRAM_VERSION);
				break;
			case 'h':
				printf("%s %f : %s\n%s\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_USAGE, PROGRAM_HELP);
				break;
			case '?' :
				printf("abort...\n");
				exit(1);
				break;
			default :
				if (cb) cb(c, optarg);
		};
	};
}
