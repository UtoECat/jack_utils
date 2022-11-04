// Jack Utils Setting SubModule.
// Allows you to load/parse from args some settings
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

#include <ju_defs.h>
#include <threads.h>

#define JU_MAX_VAR_NAME 64

struct ju_sett_s;
struct ju_val_s;

/*
 * User defined callback value type :
 * if @arg3 != NULL => 
 * - process this value as you want
 * - return 0 in case of sucess
 * - return nonzero in case of error (abort program in case of args parsing)
 * if @arg3 == NULL =>
 * - convert value to string format
 * - pass this string to ju_sett_save_value(sett_s*, val_s*, char*) function
 * - return 0 if it is done
 * - return nonzero if it is impossible to save this value
 * or if @arg1 == NULL and @arg3 == NULL =>
 * - free all resources, used by variable
 */
typedef int (*ju_sett_cb) (struct ju_sett_s*, struct ju_val_s*, const char*);

typedef struct ju_val_s { // value
	struct ju_val_s* next;
	void* value;
	ju_sett_cb callback;
	char  name[JU_MAX_VAR_NAME]; // name must not contain nonprint or = chars
	char  shortname;
	bool  as_default; // use this var as unnamed
	bool  no_argument; // we don't need any arguments :)
} ju_val_t;

/*
 * How values setted from args :
 * >> -<short_name> <value>
 * >> --<long name> <value>
 * >> --<long name>=<value>
 * How values setted from environ ? Directly :D
 * >> <long name>=<value>
 * How values setted from settings file :
 * >> [<SECTION_NAME>]
 * >> <long name>=<value><newline>
 * User Callbacks, if present with some data, will be called :)
 * If callback returns nonzero in saving data process, it will be ignored
 */


// init/uninit functions
void      ju_sett_init(ju_sett_t* s);
void      ju_sett_uninit(ju_sett_t* s);

// exception callback set (FUTURE TODO)
//void      ju_sett_except(ju_sett_t* s);

// parse functions
// pargs - noreturns in case of error. penv - returns always.
// pfile - always returns + return nonzero in case of error
void      ju_sett_pargv(ju_sett_t* s, const char** args);
void      ju_sett_penvi(ju_sett_t* s);
int       ju_sett_pfile(ju_sett_t* s, const char* f);

// save functions - saves settings to file
int       ju_sett_sfile(ju_sett_t* s, const char* f);

// defines setting. do this AFTER intialization and BEFORE any parsing
void      ju_sett_define(ju_sett_t* s, const char* name,
	char shname, void* val, ju_sett_cb type);

void*     ju_sett_getval(ju_sett_t* s, const char* name);
