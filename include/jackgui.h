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

/*
 * Float whell widget.
 */
int jg_whell_float(jg_ctx_t* ctx, float* value, float min, float step, float max);

/*
 * Image load/free functions.
 *
 * WARNING! Creating image from same texture will generate NEW IMAGE in gpu!
 * Also, there is no way to reference count this images :(
 * So, be careful, and create images before main loop, and free after
 */

JG_API struct nk_image jg_image_load_from_memory(const unsigned char *ptr, int w, int h, int channels);
JG_API void jg_image_free(struct nk_image img);

/*
 * Returns constant jackutils icon.
 */
JG_API struct nk_image jg_jackutils_icon(void);
