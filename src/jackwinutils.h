// Extension for jackutils library.
// Hepls to manipulate windows. :D
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

#define JWU_API

#define DEVICE_PROJECTION 0
#define ORTHO_PROJECTION 1

/*
 * Bind window framebuffer.
 * CleanUps Window and Modelview matrix. SetUps projection
 * Matix.
 *
 * @arg context
 * @arg projection type : Device or Ortho
 *
 * Device projection : x[-1 .. 1]     y[-1 .. 1]
 * Orthographic      : x[0  .. width] y[0  .. height]
 */
JWU_API void ju_draw_begin(ju_ctx_t*, int);

/*
 * Swaps window buffers.
 */
JWU_API void ju_draw_show(ju_ctx_t*);

/*
 * Pool events for all windows
 */
JWU_API void ju_draw_pool();

/*
 * Returns size of the framebuffer
 */
JWU_API struct{int w, h;} ju_draw_size(ju_ctx_t*);

/***********************************************
 *
 * Drawing functions
 *
 ***********************************************
 */

/*
 * This function draws waveform.
 * WARNING! DO NOT USE BUFFERS FROM PROCESS CALLBACK - 
 * YOU 100% WILL GET SEGFAULT!
 *
 * Instead, copy buffer from port, using JACKBUFFER API
 * <jackbuffer.h> or memcpy + mutex!
 * 
 * @arg sample array
 * @arg samples count
 * @arg x
 * @arg y
 * @arg width
 * @arg height
 */
JWU_API void ju_draw_samples(ju_sample_t*, size_t, int, int, int, int);
JWU_API void ju_draw_buff (ju_buff_t*, int, int, int, int);

