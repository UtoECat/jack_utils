/* JackUtils Ports Functions 
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
 * @file ju_port.h
 * JackUtils Ports API.
 */

/**
 * @addtogroup jackutilsport
 * @{
 */

/**
 * Creates new port, and returns port desriptor.
 * Jack ports are input or output points for single-channel audio data.
 *
 * Ports returned by descriptors are *safe* abstraction above pointers.
 * Valid ports descriptors are 0 .. @ref JU_MAX_PORTS
 *
 * @param x jackUtils context
 * @param n port name. Same port names for single client are not allowed!
 * @param t input or output port type
 * @param f additional port flags (@ref JackPortIsTerminal, for example)
 * @see enum ju_port_type
 *
 * @return port descriptor (+0) if port have been opened sucessfully
 * @return -1 in case of error
 */
JU_API int  ju_port_open (ju_ctx_t* x, ju_cstr_t n, enum ju_port_type t, int f);

/**
 * Decreases reference counter of port, and destroys it, if refcounter == 0
 * @param x JackUtils context.
 * @param p port descriptor.
 */
JU_API void ju_port_close(ju_ctx_t* x, int p);

/**
 * Increases reference counter of port.
 * @param x JackUtils context.
 * @param p port descriptor.
 */
JU_API void ju_port_take (ju_ctx_t* x, int p);

/** 
 * Returns number of connections to the port (or simply is port connected to anything)
 * @param x JackUtils context.
 * @param p port descriptor.
 * @return count of connections to the port (0 if nothing connected)
 */
JU_API int ju_port_connected(ju_ctx_t* x, int p);

/**
 * Returns is port owned by context.
 * @param x JackUtils context
 * @param p port descriptor
 * @return 1 if context owns this port.
 *
 * returns 1 only, and ONLY if port descriptor was not NOT created from RAW jack_port with nonzero not_own!
 * @see ju_port_rawfind
 */
JU_API int ju_port_mine(ju_ctx_t* x, int p);

/** 
 * Returns next valid port descriptor, owned by JackUtils context.
 * @param x context
 * @param p port descriptor
 * @param t search for input or output port
 * @see enum ju_port_type
 * @see ju_port_mine()
 *
 * @return next port descriptor, owned by client, or -1 when no ports left.
 * 
 * **WARNING**: this function CLOSES given port handle before searching for next port!
 *        : Call port_take on firstly iteratable port handle! (or pass -1 as port handle)
 *
 * Returned port descriptor already TAKEN! (refcnt increased).
 * You MUST CLOSE port, when it is not needed for you anymore outside of the searching loop!
 *
 * @see ju_port_take()
 * @see ju_port_close()
 */
JU_API int ju_port_next(ju_ctx_t* x, int p, enum ju_port_type t);

/**
 * Finds port by full name. This function can find not only ports owned by context.
 *
 * @param x context
 * @param n port name
 * @return founded (or new) port descriptor, or -1
 *
 * **WARNING**: founded port handle is already TAKEN, so fon't forget to CLOSE it, when you don't need this handle anymore!
 * **WARNING**: This function CAN ONLY BE CALLED AFTER STARTING JACK AUDIO PROCESSING!
 *
 * @see ju_port_close()
 * @see ju_start()
 * @see ju_port_mine()
 */
JU_API int ju_port_find(ju_ctx_t* x, ju_cstr_t n);

/**
 * Gets audio buffer from the port.
 * @param x context
 * @param p port descriptor (with type JU_INPUT)
 *
 * @return readonly buffer from Input port
 * @return NULL if port is with output type or port handle is invalid
 *
 * This function can be called ONLY inside the Jack Audio Process Callback!
 * @see ju_process_func_t
 *
 * Theese data buffers are valid only until Audio Processs Callback is working.
 *
 * Caching and acessing theese buffers later, or outside of Jack Audio Process Callback is Undefined Behaviour!
 */
JU_PROC const ju_sample_t* (ju_port_read) (ju_ctx_t* x, int p);

/** 
 * Writes data to port with Output type from sample array.
 * 
 * @param x context
 * @param p port descriptor
 * @param a array of ju_sample_t
 * @param l length of array a
 *
 * @return count of writed samples (<= ju_length())
 *
 * This function can be called ONLY inside the Audio Process Callback!
 */
JU_PROC size_t (ju_port_write) (ju_ctx_t* x, int p, const ju_sample_t* a, size_t l);

/** 
 * Writes data to port with Output type from stream callback.
 * 
 * @param x context
 * @param p port descriptor
 * @param s stream callback
 * @param u userdata to the stream callback
 * @see ju_stream_func_t
 *
 * This function can be called ONLY inside the Audio Process Callback!
 */

JU_PROC void  (ju_port_write_stream) (ju_ctx_t* x, int p, ju_stream_func_t s, void* u);

/** 
 * Finds existed port handle, or takes new port handle from jack_port_t.
 * **LOWLEVEL API!** Use this function ONLY to deal with another kinds of Jack Libraries.
 *
 * @param x JackUtils Context
 * @param p JACK PORT
 * @param not_own if == 1, JackUtils will not close this port and threat it as it's owned port.
 * @return founeded (or new) port descriptor
 * 
 * returned descriptor is already TAKEN! So you must to CLOSE it, when
 * you don't need it anymore!
 *
 * **WARNING** : jack_port_unregister will be called when both :
 * 	1. refcnt is equal 0!
 * 	2. port is owned by context!
 * If you don't need jack_port_unregister() call by JackUtils, pass 1 to not_own argument => it will prevent port from being unregistered and threated as owned.
 * **WARNING**: unregister JACK port while it's used by JACKUTILS is UNDEFINED BEHAVIOUR!
 */
JU_API int  ju_port_rawfind(ju_ctx_t* x, jack_port_t* p, int not_own);

/**
 * Returns jack_port_t, associated with handle. (LOWLEVEL)
 * Use this function ONLY to deal with another kinds of Jack Libraries.
 *
 * @param x JackUtils context
 * @param p port descriptor
 *
 * you MUST NOT UNREGISTER returned jack_port_t, or change it's ownership manually! It is Undefined Behaviour!
 * Refcounter of port descriptor is NOT changed here!
 */
JU_API jack_port_t* ju_port_rawget(ju_ctx_t* x, int p);

/** @} */

