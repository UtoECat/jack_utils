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
	char buffer[512];
	size_t bufflen;
} ju_sett_t;

void ju_sett_init(ju_sett_t* s) {
	assert(s != NULL);
	s->values  = NULL;
	s->bufflen = 0;
	memset(s->buffer, 0, 512);
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
	fprintf(stderr, "[ju_settings] Parse Error : %s\n\
	[ju_setings] at (%i:%i) \"%s\"\n", m,  l, n, e);
}

#define parse_abort(A, B, C, D) {parse_error(A, B, C, D); abort();}
#define parse_fatal(A, B, C, D) {parse_error(A, B, C, D); return -1;}

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

				v = ju_sett_find(s, name);
				if (!v) parse_abort("bad key name", it - argv, i + 2, (*it)+2);

				if (str[i] == '=') str += i + 1;
				else str = *(++it);

				if (!v->no_argument && !*str)
							parse_abort("this key requires value!", it-argv, 0, *it);
			} else {
				// short name
				char sh = *(++str);
				if (!sh) parse_abort("don't write space after '-'", it-argv, 0, *it);
				v = ju_sett_find_short(s, sh);
				if (!v) parse_abort("bad short key name", it - argv, 0, *it);
			}
		} else { // find any as_default guy
			v = ju_sett_default(s);
			if (!v) parse_abort("no default arguments left", it - argv, 0, *it);
		}
		// same part
		int r = v->callback(s, v, str); // parse
		if (r) parse_abort("can't parse value", it - argv, (*it) - str, str);
		it++;
	}
}

extern const char** environ;

// fortuantely, enviroment parser is MUSH simpler
void      ju_sett_penvi(ju_sett_t* s) {
	const char** const env = environ;
	const char** it = environ;
	assert(s != NULL);

	while (*it && **it) {
		ju_val_t* v = NULL;
		const char* str = *it; 
		char name[JU_MAX_VAR_NAME] = {0};

		size_t i = 0;
		for (i = 0; str[i] != '\0' && str[i] != '=' && i < JU_MAX_VAR_NAME - 1;
			i++) {
			name[i] = str[i];
		};
		v = ju_sett_find(s, name);
		if (str[i] == '=') str += i + 1;
		else {
			parse_error("bad enviroment string", it - env, 0, *it);
			return;
		}
		if (!v->no_argument && !*str) { 	
			parse_error("this key requires value!", it-env, 0, *it);
			it++; // we can skip this error :)
			continue;
		}
		int r = v->callback(s, v, str);
		if (r) parse_error("can't parse envvar", it-env, 0, *it);
		it++;
	}
}

// FIXME Not implemented yet
//int       ju_sett_pfile(ju_sett_t* s, const char* f);
//int       ju_sett_sfile(ju_sett_t* s, const char* f);

void  ju_sett_define(ju_sett_t* s, const char* name,
	char shname, void* val, ju_sett_cb type, bool d, const char* desc) {
	assert(s && name && shname && type != NULL);
	ju_val_t* v = NULL;
	// check is variable exist firsly
	char name_buff[JU_MAX_VAR_NAME] = {0};
	memcpy(name_buff, name, MIN(strlen(name), JU_MAX_VAR_NAME-1));
	v = ju_sett_find(s, name_buff);
	if (v) {
		error("Attempt to create same-named variables!", 0);
		assert(v == NULL);
		return;
	}

	v = calloc(sizeof(ju_val_t), 1);
	assert(v != NULL);
	memcpy(v->name, name_buff, JU_MAX_VAR_NAME);
	v->shortname = shname;
	v->callback = type;
	// insert
	v->next = s->values;
	s->values = v;
	// other fields
	v->value = val;
	if (!val) v->no_argument = true;
	v->as_default = d;
	v->description = desc;
}

void*  ju_sett_getval(ju_sett_t* s, const char* name) {
	char name_buff[JU_MAX_VAR_NAME] = {0};
	memcpy(name_buff, name, MIN(strlen(name), JU_MAX_VAR_NAME-1));
	ju_val_t* v = ju_sett_find(s, name_buff);
	if (!v) return NULL;
	return v->value;
}

static void ju_sett_flush(ju_sett_t* s, FILE* f) {
	assert(s);
	if (f) fwrite(s->buffer, 1, s->bufflen, f);
	s->bufflen = 0;
}


void ju_sett_print(ju_sett_t* s, const char* name) {
	assert(s != NULL);
	ju_val_t* v = s->values;
	printf("USAGE : %s -<key> <value> --<long key>=<value> --<long key> <value> <additional values>\n", name);
	while (v) {
		printf("\t-%c, --%s, %s%s\n", v->shortname, v->name, v->no_argument ? "" : "<argument>", v->description);
		v = v->next;
	}
	fflush(stdout);
}

void ju_sett_debug(ju_sett_t* s) {
	assert(s != NULL);
	ju_val_t* v = s->values;
	printf("List of variavles with associated values :\n");
	while(v) {
		int r = v->callback(s, v, NULL);
		if (r) printf("\t %s = (non-convertable) %p\n", v->name, v->value);
		else {
			printf("\t %s = ", v->name);
			ju_sett_flush(s, stdout);
			putchar('\n');
		}
		v = v->next;
	}
}

void ju_sett_save_value(ju_sett_t* s, ju_val_t* v, char* str) {
	assert(s && v && s != NULL);
	size_t l = MIN(strlen(str), 512 - s->bufflen);
	memcpy(s->buffer, str, l);
}

