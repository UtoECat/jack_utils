// Extension for jackutils library.
// Hepls to manipulate windows. :D
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

#define JWU_API

#include <GL/gl.h>

typedef struct ju_win_s ju_win_t;

// width-height and x-y pairs
typedef struct {int w, h;}       w_wh_t;
typedef struct {int x, y;}       w_xy_t;
// rectangle
typedef struct {int x, y, w, h;} w_rect_t;

/*
 * Opens new window.
 * @arg width
 * @arg height
 *
 * window is hidden by default. You can show it, using ju_win_show()
 */
JWU_API ju_win_t* ju_win_open(int, int);
JWU_API void      ju_win_close(ju_win_t*);
JWU_API bool      ju_win_should_close(ju_win_t*);

JWU_API void      ju_win_show(ju_win_t*);
JWU_API void      ju_win_hide(ju_win_t*);
JWU_API void      ju_win_title(ju_win_t*, ju_cstr_t);

JWU_API w_wh_t    ju_win_size(ju_win_t*);
JWU_API w_xy_t    ju_win_mouse(ju_win_t*);
JWU_API float     ju_win_scroll(ju_win_t* w);

JWU_API int       ju_win_getkey(ju_win_t* w, int key);
JWU_API int       ju_win_mousekey(ju_win_t* w, int key);

JWU_API void      ju_win_resize(ju_win_t*, w_wh_t);
/*
 * Enables(true)/disables(false, default) stretching window buffer.
 * Mouse input will be stretched too! 
 * If enabled, setuped window size will be keeped, but real image will
 * be scaled and fitted into window.
 * If disabled, window size will changes, and framebuffer size too.
 */
JWU_API void      ju_win_stretch(ju_win_t*, bool);

/*
 * Bind window framebuffer.
 * CleanUps Window and Modelview matrix.
 * Swaps framebuffers.
 * Pool windows events.
 *
 * @arg context
 * @ret delta time since previous frame
 */
JWU_API double ju_draw_begin(ju_win_t*);

/*
 * Setups window viewport with stretch checks and calculations.
 */
JWU_API void   ju_draw_view(ju_win_t*, w_rect_t);

/*
 * This function should be called every frame, after processing all windows
 */
JWU_API void ju_win_pool_events(void);

/***********************************************
 *
 * Drawing functions
 *
 ***********************************************
 */

JWU_API void ju_draw_samples(ju_sample_t* arr, size_t l, float x, float y, float w, float k);

JWU_API void ju_draw_grid(float kw, float kh, float x, float y, float w, float h);

JWU_API void ju_draw_digit(char d, float x, float y, float w, float h);
JWU_API void ju_draw_int(int i, float x, float y, float s);
