/* GUI Extension for jackutils library.
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
JG_API void jg_end(jg_ctx_t*, int mul);

/*
 * Synchronize window visibility with Session Manager.
 */
JG_API void jg_sync_visibility(jg_ctx_t* gui, ju_ctx_t* ctx);

/*
 * Shows or hides window
 */
JG_API void jg_set_visibility(jg_ctx_t*, int);

/*
 * Your utilities redraws only when user interacts with window.
 * This may be not enough for you. Then, you can call
 * jg_request_redraw() to make window redraws again at next tick.
 */
JG_API void jg_request_redraw(jg_ctx_t*); 

/*
 * Notifies user with message. You need to call it ONCE!
 * If you call it not once, old message will be concated with new :)
 * Message will not be cleared until user not closes it :)
 */
JG_API void jg_show_message(jg_ctx_t*, const char* msg);

/*
 * this function called from jg_ju_bar :)
 */
JG_API void jg_show_about(jg_ctx_t*);

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

/*
 * Float whell widget.
 */
int jg_whell_float(jg_ctx_t* ctx, float* value, float min, float step, float max);

/*
 * Custom top-level jackutils bar :)
 */
struct jg_bar_item {
	void (*cb_draw) (jg_ctx_t*, struct jg_bar_item*);
	union {
		void* p;
	 	int i;
		float n;	
	}	data[5];
	int width;
	const char* desc;
};

struct jg_bar_item jg_float_item(const char* desc, float* val, float min, float step, float max);
struct jg_bar_item jg_text_item(const char* desc, const char* text, int w);
struct jg_bar_item jg_null_item();

int jg_ju_topbar(jg_ctx_t* ctx, ju_ctx_t* cli, struct jg_bar_item* arr);

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
