// JackUtils Library - provides more simplificated JACK API for clients
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#pragma once

#include <assert.h> // assertions 
#include <stddef.h> // size_t

#include <jack/jack.h> // jack

#define JU_API
#define JU_PROC // means that this function can be called ONLY in process callback

#define JU_MAX_PORTS 127 // max ports descriptors count
#define JU_INPUT  0
#define JU_OUTPUT 1

typedef unsigned char ju_uint8_t;
typedef const char*   ju_cstr_t;
typedef jack_default_audio_sample_t ju_sample_t;
typedef struct ju_ctx_s ju_ctx_t;
typedef struct ju_buff_s ju_buff_t;

#include <jackbuffer.h>

/*
 * Process callback function.
 *
 * @arg context
 * @arg length of sample arrays (same as ju_length())
 */
typedef void (*ju_process_func_t) (ju_ctx_t*, size_t);

/*
 * Stream callback functin for ju_port_write_stream() function
 *
 * @arg userdata
 * @arg dest sample array
 * @arg count of elements callback need to write
 *
 * @ret count of written elements
 *
 * If callback returns less data, than required, it will be called again.
 */
typedef size_t (ju_stream_func_t)(void*, ju_sample_t*, size_t);

/***************************************************
 *
 * Functions
 *
 ****************************************************
 */

/* 
 * Allocates and Initializes jackutils context structure. 
 *
 * @arg client name (may be changed by server)
 * @arg (optional) server name. Pass NULL to autoselect.
 * @ret 0 on sucess
 */
JU_API ju_ctx_t* (ju_ctx_init)  (ju_cstr_t, ju_cstr_t);

/*
 * Destroy context and release internal resources
 *
 * @arg jackutils context
 */
JU_API void      (ju_ctx_unint) (ju_ctx_t*);

/*
 * Returns real client name
 */
JU_API ju_cstr_t (ju_get_name) (ju_ctx_t*);

/*
 * Returns information about libraries.
 */
JU_API ju_cstr_t (ju_jack_info) (void);
JU_API ju_cstr_t (ju_glfw_info) (void);

/*
 * Creates new port, and returns port desriptor.
 * Jack ports - input and output points for single-channel audio data.
 *
 * Ports returned by descriptors - safe abstraction above pointers.
 * Valid ports descriptors 1 .. JU_MAX_PORTS
 *
 * @arg context
 * @arg port name. Same port names for single client are not allowed!
 * @arg input (0) or output (1) port type
 * @arg additional port type information (JackPortIsTerminal for example)
 *
 * @ret port descriptor (+1) if port is opened sucessfully
 * @ret 0 on error
 */
JU_API int  ju_port_open (ju_ctx_t*, ju_cstr_t, int, int);
JU_API void ju_port_close(ju_ctx_t*, int);

/* 
 * @arg context
 * @arg port descriptor
 * @ret port connections count
 */
JU_API int ju_port_connected(ju_ctx_t*, int);

/*
 * @arg context
 * @arg port descriptor
 * @ret 1 if context owns this port
 */
JU_API int ju_port_mine(ju_ctx_t*, int);

/* 
 * @arg context
 * @arg port handle
 * @arg is input(0) or output (1) port
 *
 * @ret next port handle owned by client, or 0 when loop is end.
 */
JU_API int ju_port_next(ju_ctx_t*, int, int);

/*
 * Finds port by full name. This function can find not only ports owned
 * by context.
 *
 * @arg context
 * @arg port name
 *
 * @ret founded port descriptor, or 0
 *
 * New founded ports, not owned by context MUST be closed (to free descriptor)!
 *
 * Be aware! If you will find your own port, it will have
 * SAME handle as OPENED! Don't close your own ports :)
 *
 * WARNING : This function CAN ONLY BE CALLED AFTER CALLING JU_START()!
 */
JU_API int ju_port_find(ju_ctx_t*, ju_cstr_t);

/*
 * Starts client processing. Do this after client and port creation.
 * ju_process_func_t will be called in another thread every time, when
 * audio data is available.
 *
 * This function must not call functions that might block for long time,
 * like malloc(), free(), pthread_join(), sleep(), wait() and etc.
 * This function should work as fast as possible! 
 *
 * @arg context
 * @arg process function
 * @ret 0 on sucess
 */
JU_API int  (ju_start) (ju_ctx_t*, ju_process_func_t);
JU_API void (ju_stop)  (ju_ctx_t*);

/*
 * Checks is server online, and we have connection.
 *
 * @arg context
 * @arg timeout in MILLISeconds to wait (-1 forever, 0 - no sleep)
 */
JU_API int (ju_is_online) (ju_ctx_t* x, int sec); 

/*
 * You should call this after proper initialization and setup!
 * It will enter in loop, that will works until Jack server will be closed,
 * or Window will be closed (if enabled).
 *
 * If enabed, this loop will handle window process and redraw events.
 * If disabled, works same as while(ju_is_online(x, time)) {func(x);}
 *
 * @arg context
 * @arg callback, runned in loop (optional for redraw)
 * @arg MILLIseconds await before new loop (-1 for forever, 0 for no wait)
 */
JU_API void (ju_loop) (ju_ctx_t* x, void (*) (ju_ctx_t*), int); 

/*
 * You should call this after initialization and ju_start().
 * Makes current thread sleep until jack server will be closed!
 * 
 * @arg context
 *
 * @ret if this function returns - connection with server is lost
 * by any reason. You still need to call ju_uninit() after this.
 *
 * Compability function. Replace it with ju_is_online(x, -1) if possible;
 */
JU_API void (ju_wait) (ju_ctx_t* x);

/* 
 * Window API.
 * This function will try to open window, if possible.
 * 
 * Only one window per context is available (to make it less complex)
 * Window will be closed and deinitialized after calling ju_ctx_uninit()
 *
 * Legacy OpenGL 1.X is used by default.
 *
 * @arg context
 * @arg width
 * @arg height
 * @ret 0 on sucess!
 */
JU_API int (ju_ctx_win) (ju_ctx_t*, int w, int h);

#include <jackwinutils.h>

/*
 * @arg context
 * @ret glfw Window handle (if available)
 */
JU_API void* (ju_ctx_win_get) (ju_ctx_t*); 

/*
 * @ret sample array length
 *
 * This function can be called ONLY from process callback!
 */
JU_PROC size_t (ju_length) (ju_ctx_t*);

/* 
 * @arg context
 * @ret sample rate
 */
JU_API  jack_nframes_t (ju_rate) (ju_ctx_t*);

/*
 * @arg context
 * @arg port descriptor
 *
 * @ret readonly buffer from InputPort
 * @ret NULL if port is Output or handle is invalid
 *
 * This function can be called ONLY from process callback!
 */
JU_PROC const ju_sample_t* (ju_port_read) (ju_ctx_t*, int);

/* 
 * Writes data to OutputPort from sample array in third argument.
 * Size of array must be same or more than ju_length();
 * 
 * @arg context
 * @arg port descriptor
 * @arg ju_sample_t[ju_length()]
 *
 * This function can be called ONLY from process callback!
 */
JU_PROC void  (ju_port_write) (ju_ctx_t*, int, const ju_sample_t*);

/* 
 * Writes data to OutputPort from callback in third argument.
 * Read commentary for ju_stream_func_t for more details!
 * 
 * @arg context
 * @arg port descriptor
 * @arg callback
 * @arg userdata for callback
 *
 * This function can be called ONLY from process callback!
 */
JU_PROC void  (ju_port_write_stream) (ju_ctx_t*, int, ju_stream_func_t, void*);

