// GUI Extension for jackutils library.
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!


#pragma once

#define JG_API
#include <ext/gl.h>
#include <stdarg.h>
#include <ext/nuklear.h>

typedef struct nk_context jg_ctx_t;

/*
 * Initializes GUI context.
 * @ret NULL in case of error
 */
JG_API jg_ctx_t* jg_init(const char* title, int w, int h);

/*
 * Destroys GUI context
 */
JG_API void jg_uninit(jg_ctx_t*);

/*
 * Starts GUI objects enumeration.
 * You must call jg_end() when you done
 *
 * @ret 1 on sucess. Else you must not enumerate anything!
 */
JG_API int  jg_begin(jg_ctx_t*);
JG_API void jg_end(jg_ctx_t*);

JG_API int  jg_should_close(jg_ctx_t*);

/*
 * Your utilities redraws only when user interacts with window.
 * This may be not enough for you. Then, you can call
 * jg_request_redraw() to make window redraws again at next tick.
 */
JG_API void jg_request_redraw(jg_ctx_t*); 

/*
 * All other GUI functions are provided by nuklear API. :)
 */

// custom widgets :)

struct waveinfo {
	float min, max;
	float x, y, sx, sy;
};

struct waveinfo waveinfo_default();

void jg_waveview(jg_ctx_t* ctx, float* arr, size_t len, struct waveinfo*);
void jg_waveedit(jg_ctx_t* ctx, float* arr, size_t len, struct waveinfo*);
