// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <ju_settings.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ju_sett_s {
	ju_val_t* values;
	ju_val_t* current;
} ju_sett_t;

void ju_sett_init(ju_sett_t* s) {
	assert(s != NULL);
	s->values  = NULL;
	s->current = NULL;
}
void ju_sett_uninit(ju_sett_t* s) {
	assert(s != NULL);
	s->current = NULL;
	ju_val_t* n = s->values;
	while(n) {
		ju_val_t* f = n;
		n = n->next;
		f->next = NULL;
		f->callback(NULL, f, NULL);
		free(f);
	}
	s->values = NULL;
}

static void error(const char* m, int v) {
	fprintf(stderr, "[ju_settings] Error: %s (%i)\n", m, v);
}

static void parse_error(const char* m, int l, int n, const char* e) {
	fprintf(stderr, "[ju_settings] Parse Error : %s\n
	[ju_setings] at (%i:%i) \"%s\"\n", m,  l, v, e);
}

#define parse_abort(A, B, B, D) {parse_error(A, B, C, D); abort();}
#define parse_fatal(A, B, B, D) {parse_error(A, B, C, D); return -1;}

static inline int ju_sett_compare(ju_val_t* v, char *name, char sh) {
	assert(v != NULL);
	if (sh != 0) return v->shortname == sh;
	if (name) return memcmp(v->name, name, JU_MAX_VAR_NAME) == 0;
	assert("Both arguments are NULL!" == 0);
}

static ju_val_t* ju_sett_find(ju_sett_t* s, char name[JU_MAX_VAR_NAME]) {
	ju_val_t* v = s->values;
	while (v && !ju_sett_compare(v, name, 0)) 
		v = v->next;
	return v;
}

static ju_val_t* ju_sett_find_short(ju_sett_t* s, char sh) {
	ju_val_t* v = s->values;
	while (v && !ju_sett_compare(v, NULL, sh)) 
		v = v->next;
	return v;
}

#define MIN(A, B) ((A) < (B) ? (A) : (B))

void*     ju_sett_getval(ju_sett_t* s, const char* name) {
	char name_buff[JU_MAX_VAR_NAME] = {0};
	memcpy(name_buff, name, MIN(strlen(name), JU_MAX_VAR_NAME-1));
	ju_val_t* v = ju_sett_find(s, name_buff);
	if (!v) return NULL;
	return v->value;
}

static ju_val_t* ju_sett_default(ju_sett_t* s) {
	assert(s != NULL);
	while (s->current && s->current->as_default == false && !s->current->no_argument)
		s->current = s->current->next;
	return s->current;
}

#define ISEOF(str) (!str || !*str) 
#define GETARGUMENT(v, str, it) if (!v->no_argument) { \
	str = *(++it); \
	if (ISEOF(str)) parse_abort("this key requires argument!", it - argv - 1, 0, str); \
	}
/*
 * I hate create parsers :(
 * This is so frickin hard shit.
 * I spend fuckin 6 hours for this bullshit.
 */
void ju_sett_pargv(ju_sett_t* s, const char** argv) {
	const char** it = argv;
	assert(s && argv != NULL);
	s->current = s->values; // reset current

	while (*it && **it) {
		const char* str = *it;
		ju_val_t* v = NULL;

		if (*str == '-') {
			// we have name
			str++;
			if (*str == '-') {
				// long name
				str++;
				char name[JU_MAX_VAR_NAME] = {0};
				size_t i = 0;
				for (i = 0; str[i] != '\0'
										&& str[i] != '='
										&& i < JU_MAX_VAR_NAME - 1;
					i++) {
					name[i] = str[i];
				};
				if (str[i] != '=') 
						str = ++it;
				// else value is in current string
				v = ju_sett_find(s, name);
				if (!v) parse_abort("bad key", it - argv, i + 2, name);
			} else {
				// short name (TODO NAMES)
				char sh = *(++str);
				if (!sh) parse_abort("what???", it-argv, 2, "(EOF)");
				v = ju_sett_find_short(s, sh);
				if (!v) parse_abort("bad short key", it - argv, 2, str-1);
			}
		} else { // find any as_default guy
			v = ju_sett_default(s);
			if (!v) parse_abort("no default arguments left", it - argv, 0, "");
		}
		// same part
		int r = v->callback(s, v, str); // parse
		if (r) parse_abort("can't parse value", it - argv, 0, str);
		it++;
	}
}

void      ju_sett_penvi(ju_sett_t* s);
int       ju_sett_pfile(ju_sett_t* s, const char* f);

int       ju_sett_sfile(ju_sett_t* s, const char* f);

void      ju_sett_define(ju_sett_t* s, const char* name,
	char shname, void* val, ju_sett_cb type);
