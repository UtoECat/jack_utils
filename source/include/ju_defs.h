// General definitions :p 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

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
