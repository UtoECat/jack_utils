// JackUtils Ports Functions 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

/* ---------------------------------------------
 *
 * JACK PORTS API
 *
 * ---------------------------------------------
 */

/*
 * Creates new port, and returns port desriptor.
 * Jack ports - input or output points for single-channel audio data.
 *
 * Ports returned by descriptors - safe abstraction above pointers.
 * Valid ports descriptors 0 .. JU_MAX_PORTS
 *
 * @arg context
 * @arg port name. Same port names for single client are not allowed!
 * @arg input (1) or output (2) port type
 * @arg additional port flags (JackPortIsTerminal for example)
 *
 * @ret port descriptor (+0) if port is opened sucessfully
 * @ret -1 on error
 */
JU_API int  ju_port_open (ju_ctx_t*, ju_cstr_t, int, int);

/*
 * Decreases reference counter of port, and destroys it, if
 * refcounter == 0
 */
JU_API void ju_port_close(ju_ctx_t*, int);

/*
 * Increases reference counter of port.
 */
JU_API void ju_port_take (ju_ctx_t*, int);

/* 
 * @arg context
 * @arg port descriptor
 * @ret port connections count
 */
JU_API int ju_port_connected(ju_ctx_t*, int);

/*
 * @arg context
 * @arg port descriptor
 * @ret 1 if context owns this port (and 
 * if NOT created from RAW with nonzero flag! *LOWLEVEL*)
 */
JU_API int ju_port_mine(ju_ctx_t*, int);

/* 
 * @arg context
 * @arg port handle
 * @arg is input(1) or output (2) port
 *
 * @ret next port handle owned by client, or -1 when loop is end.
 * this function CLOSES port before increasing handle! call port_take on
 * iteratable port handle!
 * Returned port handle already TAKEN! (refcnt increased).
 * You MUST CLOSE port, when it not important for you anymore.
 */
JU_API int ju_port_next(ju_ctx_t*, int, int);

/*
 * Finds port by full name. This function can find not only ports owned
 * by context.
 *
 * @arg context
 * @arg port name
 * @ret founded port descriptor, or -1
 *
 * founded port handle is already TAKEN, so fon't forget to CLOSE it,
 * when you don't need this handle anymore!
 *
 * WARNING : This function CAN ONLY BE CALLED AFTER CALLING JU_START()!
 */
JU_API int ju_port_find(ju_ctx_t*, ju_cstr_t);

/**********************************************
 *
 * Port Audio Data Modification
 *
 **********************************************
 */ 

/*
 * @arg context
 * @arg port descriptor
 *
 * @ret readonly buffer from InputPort
 * @ret NULL if port is Output or handle is invalid
 *
 * This function can be called ONLY in Jack Audio Process Callback!
 * Theese data are valid only until Audio Processs Callback is working.
 */
JU_PROC const ju_sample_t* (ju_port_read) (ju_ctx_t*, int);

/* 
 * Writes data to OutputPort from sample array in third argument.
 * Size of array must be passed as third argument.
 * 
 * @arg context
 * @arg port descriptor
 * @arg ju_sample_t
 * @arg length of array
 *
 * @ret Count of writed samples (less or equal to ju_length())
 * This function can be called ONLY in Audio Process callback!
 */
JU_PROC size_t (ju_port_write) (ju_ctx_t*, int, const ju_sample_t*, size_t);

/* 
 * Writes data to OutputPort from callback in third argument.
 * Read commentary for ju_stream_func_t for more details!
 * 
 * @arg context
 * @arg port descriptor
 * @arg callback
 * @arg userdata for callback
 *
 * This function can be called ONLY in Audio Process callback!
 */
JU_PROC void  (ju_port_write_stream) (ju_ctx_t*, int, ju_stream_func_t, void*);


/**********************************************
 *
 * LOW LEVEL API
 *
 * *********************************************
 */

/* 
 * LOWLEVEL : finds existed handle, or takes new handle for jack_port_t.
 * Use this function ONLY to deal with another types of Jack Libraries.
 *
 * returned handle is already TAKEN! So you must to CLOSE it, when
 * you don't need it anymore!
 *
 * WARNING : jack_port_unregister will be called when both :
 * 	1. refcnt will be equal 0!
 * 	2. port will be owned by context!
 * If you don't need this behaviour, pass nonzero third argument => it
 * will prevent port from being unregistered.
 *
 * WARNING: unregistering port while it used by JACKUTILS HANDLE SYSTEM is
 * UNDEFINED BEHAVIOUR!
 */
JU_API int  ju_port_rawfind(ju_ctx_t*, jack_port_t*, int);

/*
 * LOWLEVEL : returns jack_port_t, associated with handle.
 * Use this function ONLY to deal with another kinds of Jack Libraries.
 *
 * you MUST NOT UNREGISTER returned jack_port_t, or change it ownership!
 * It can produce an Undefined Behaviour!
 *
 * Refcounter of port is NOT changed here!
 */
JU_API jack_port_t* ju_port_rawget(ju_ctx_t*, int);

