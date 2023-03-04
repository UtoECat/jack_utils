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

/**
 * @file jackgui.h
 * GUI and window creation extension for JackUtils :)
 */
/**
 * @defgroup jackgui JackGUI extension
 * GUI and window creation extension for JackUtils :)
 * All other GUI functions are provided by nuklear API. :)
 * @ref nuklear
 */

/** */
#define JG_API

#include <ext/gl.h>
#include <stdarg.h>
#include <ext/nuklear.h>

typedef struct nk_context jg_ctx_t;

/**
 * Initializes GUI context.
 * @param title window title
 * @param w default width of the window
 * @param h default height of the window
 * @return NULL in case of error
 * @return GUI context on sucess
 * @ingroup jackgui
 */
JG_API jg_ctx_t* jg_init(const char* title, int w, int h);

/**
 * Destroys GUI context.
 * @param u GUI context
 * @ingroup jackgui
 */
JG_API void jg_uninit(jg_ctx_t* u);

/**
 * Starts GUI objects enumeration.
 * You must call jg_end() when you done
 *
 * You can use OpenG commands here ONLY before drawing any GUI element (for future compability).
 *
 * @param u GUI context
 * @param fullscreen draw window on fullscreen? (if your program does not draw anything using OpenGL on the background...)
 * @return 1 on sucess. In other case you must not enumerate anything!
 * @ingroup jackgui
 */
JG_API int  jg_begin(jg_ctx_t* u, int fullscreen);

/**
 * Ends GUI objects enumeration.
 * It combines GUI verticies and send them to OpenGL.
 * Also pulls GLFW Window events, switches buffers, etc.
 *
 * @param u GUI Context
 * @param mul **For ABI compability! Set always to zero!**
 * @ingroup jackgui
 */
JG_API void jg_end(jg_ctx_t* u, int mul);

/**
 * Synchronize window visibility with Session Manager (if has one, else do nothing).
 * @param gui GUI context
 * @param ctx JackUtils context
 * @ingroup jackgui
 */
JG_API void jg_sync_visibility(jg_ctx_t* gui, ju_ctx_t* ctx);

/*
 * Shows or hides window.
 * @param u GUI Context
 * @param s Show(1) or hide(0) window?
 * @ingroup jackgui
 */
JG_API void jg_set_visibility(jg_ctx_t* u, int s);

/**
 * Sends redraw event to internal event pool.
 *
 * Window context redraws only user interacts with windowby default (when any event extst).
 * This may be not enough for you, and in this case you can call this function to make redraw window again at next tick.
 *
 * @param u GUI context
 * @ingroup jackgui
 */
JG_API void jg_request_redraw(jg_ctx_t* u); 

/**
 * Notifies user with message. You need to call it ONCE!
 * If you call it not once, old message will be concated with new.
 * Message will be cleared when user will close it.
 *
 * @param u GUI context
 * @param msg Message to show.
 * @ingroup jackgui
 */
JG_API void jg_show_message(jg_ctx_t* u, ju_cstr_t msg);

/**
 * Shows about subwindow with information about your program.
 *
 * @param u GUI Context
 * @ingroup jackgui
 */
JG_API void jg_show_about(jg_ctx_t*u);

/**
 * Nuklear Image load function.
 *
 * WARNING! Creating image from same texture will generate NEW IMAGE in gpu!
 * Also, there is no way to reference count this images :(
 * So, be careful, and create images before main loop, and free after.
 *
 * @param ptr pointer to the raw image
 * @param w width
 * @param h height
 * @param channels count of channels (1 ... 4)
 */
JG_API struct nk_image jg_image_load_from_memory(const unsigned char *ptr, int w, int h, int channels);

/**
 * Frees nuklear image.
 * @param img nuklear image
 * @see jg_image_load_from_memory()
 */
JG_API void jg_image_free(struct nk_image img);

/**
 * @returns constant jackutils icon.
 */
JG_API struct nk_image jg_jackutils_icon(void);

/**
 * @defgroup customwidgets Custom JackGUI widgets
 * @ref jackgui :)
 * @{
 */

/**
 * Float whell widget.
 */
float jg_whell_float(jg_ctx_t* ctx, float value, float min, float max);

/**
 * JackUtils Classic GUI topbar.
 * Shows info about jhack, session manager, about window, etc.
 *
 * @param u JackGUI context
 * @param j JACKUTILS context
 */
JG_API void jg_jackutils_topbar(jg_ctx_t* u, ju_ctx_t* j);

/**@}*/
