/* 
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

#include <jackutils.h>
#include <threads.h>
#include <stdio.h>
#include <stdlib.h>

JU_API ju_var_t ju_var_nil() {
	ju_var_t v;
	memset(&v, 0, sizeof(ju_var_t));
	return v;
}

// i know that this is not very good thing... but... c'mon...
static int init_mutex = 0;
static mtx_t vars_mutex;

static void vars_atexit(void) {
	mtx_destroy(&vars_mutex);
}

JU_API void ju_vars_lock() {
	if (!init_mutex) {
		init_mutex = 1;
		mtx_init(&vars_mutex, 0);
		atexit(vars_atexit);
	}
	mtx_lock(&vars_mutex);
}

JU_API void ju_vars_unlock() {
	mtx_unlock(&vars_mutex);
}

JU_API int ju_vars_save(ju_cstr_t filename, ju_var_t* v) {
	FILE* f = fopen(filename, "w");
	if (!f) return -1;
	int line = 1;

	for (;v->_vp != NULL; v++) {
		fprintf(f, "%s=", v->name);
		int s = v->_vp->write(f, v);
		if (s) {
			fprintf(stderr, "JackVars : can't serialize variable %s by some reason (code %i)(line %i)! Saving process is failed!\n", v->name, s, line);
			return s; // error
		}
		fputc('\n', f);
		line++;
	}
	fclose(f);
}

static char* readvname(FILE* f, char buff[512]) {
	int c;
	char* p = buff;

	while ((c = fgetc(f)) != EOF) {
		if (c == '=') { // valid thing
			break;
		} else if (c == '\n') {
			// just in case...
			return NULL;
		}
		*(p++) = c;
	}

	*(p++) = '\0';
	return buff;
}

JU_API ju_var_t* ju_vars_find(ju_cstr_t name, ju_var_t* v) {
	for (;v->_vp != NULL; v++) {
		if (strcmp(v->name, name) == 0) return v;
	}
	return NULL;
}

JU_API int ju_vars_load(ju_cstr_t filename, ju_var_t* v) {
	FILE* f = fopen(filename, "r");
	if (!f) return -1;
	char buff[512]; // tmp
	int line = 1;

	for (;v->_vp != NULL; v++) {
		char* name = readvname(f, buff);
		if (!name) {
			fprintf(stderr, "JackVars : can't pass variable name (line %i)!", line);
			return -1; // error
		}
		int s = v->_vp->parse(f, v);
		if (s) {
			fprintf(stderr, "JackVars : can't parse variable %s by some reason (code %i)(line %i)! Reading process is failed!\n", v->name, s, line);
		}
		line++;
	}
	fclose(f);
}

// implementations

static ju_var_t ju_var_default(ju_cstr_t name, struct ju_varclass* class_ptr) {
	ju_var_t v;
	memset(&v, 0, sizeof(ju_var_t));
	v.name = name;
	v._vp = class_ptr;
	return v;
}
/**
 *DATA to REFERENCE ON FLOAT from DATA VOID POINTER
 */
#define D2RFLT(v) (*(float*)(v.p))

/*
 * Float read/write'able
 */

//implemented in jackwidgets.c
void __render_frw (jg_ctx_t*, ju_var_t*);

static int  parse_frw (FILE* f,  ju_var_t* v) {
	float tmp = 0.0f;
	int s = fscanf(f, "%f\n", &tmp);
	if (s < 1) return -1;
	// apply limits
	if (tmp < v->data[1].f) tmp = v->data[1].f;
	if (tmp > v->data[2].f) tmp = v->data[2].f;
	// "atomic" write
	ju_vars_lock();
	D2RFLT(v->data[0]) = tmp;
	ju_vars_unlock();
	return 0;
}

static int  write_frw (FILE* f,  ju_var_t* v) {
	ju_vars_lock();
	float val = D2RFLT(v->data[0]);
	ju_vars_unlock();
	return fprintf(f, "%f", val) > 1;
}

struct ju_varclass float_rw_class = {__render_frw, parse_frw, write_frw, 1};

JU_API ju_var_t ju_var_float_rw(ju_cstr_t name, float* val, float min, float max) {
	ju_var_t var = ju_var_default(name, &float_rw_class);
	var.data[0].p = val;
	var.data[1].f = min;
	var.data[2].f = max;
	return var;

}

/*
 * Float readonly 
 */

//implemented in jackwidgets.c
void __render_fro (jg_ctx_t*, ju_var_t*);

static int  parse_fro (FILE* f,  ju_var_t* ) {
	fscanf(f, "\n"); return 0;
}

static int  write_fro (FILE* ,  ju_var_t* ) {
	return 0;
}

struct ju_varclass float_ro_class = {__render_fro, parse_fro, write_fro, 2};

JU_API ju_var_t ju_var_float_ro(ju_cstr_t name, float* val) {
	ju_var_t var = ju_var_default(name, &float_ro_class);
	var.data[0].p = val;
	return var;
}

/*
 * Text readonly 
 */

//implemented in jackwidgets.c
void __render_ctxt (jg_ctx_t*, ju_var_t*);

static int  parse_ctxt (FILE* f,  ju_var_t* ) {
	fscanf(f, "\n"); return 0;
}

static int  write_ctxt (FILE* ,  ju_var_t* ) {
	return 0;
}

struct ju_varclass float_ctxt_class = {__render_ctxt, parse_ctxt, write_ctxt, 2};

JU_API ju_var_t ju_var_float_ctxt(ju_cstr_t name, const char** val) {
	ju_var_t var = ju_var_default(name, &float_ctxt_class);
	var.data[0].p = val;
	return var;
}


