// Jack Utils Args SubModule.
// Allows you to parse from args some settings
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

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
