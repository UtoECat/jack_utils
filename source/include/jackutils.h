/* JackUtils Library - provides more simplificated JACK API for clients
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
 * @file jackutils.h
 * Main header file of the JackUtils API.
 */

#include <assert.h>  
#include <ju_defs.h>
#include <jack/jack.h> 

#define JU_API
#define JU_PROC

#define JU_MAX_PORTS 127 /**< max ports descriptors count. */

/**
 * Enum of all available port types.
 * Port types cannot be mixed!
 */
enum ju_port_type {
	JU_INPUT  = 1, /**< Input port type. Port recieves audio data. */
	JU_OUTPUT = 2 /**< Output port type. Port returns audio data. */
};

typedef unsigned char ju_uint8_t; /**< Unsigned int definition... */
typedef const char*   ju_cstr_t; /**< Informative type definition. C String type. */
typedef jack_default_audio_sample_t ju_sample_t; /**< Sample type. Typically it is float, but not predict it! Depends on definition in jack/jack.h file! */
typedef struct ju_ctx_s ju_ctx_t; /**< Typedef wrapper around internal JackUtils Context structure. */

#include <ju_buffer.h>

/**
 * Jack Audio processing callback.
 * In this callback you generally go through all your ports and process audio data in some way...
 *
 * This process callback **should not call functions that might block for long time**,
 * like malloc(), free(), pthread_join(), sleep(), wait() and etc.
 * => *process callback should work as fast as possible!* 
 *
 * @param x JackUtils context.
 * @param s length of sample arrays (same as ju_length())
 * @see ju_start()
 * @return void
 */
typedef void (*ju_process_func_t) (ju_ctx_t* x, size_t s);

/**
 * Stream callback function for ju_port_write_stream() function.
 * This is cool wrapper for generators or audio data readers from something :)
 *
 * @param p userdata
 * @param x destination sample array
 * @param count of elements callback need to write
 * @return count of written elements
 *
 * If callback returns less data, than required, it will be called again.
 * If callback returns 0, it will not be called again! (means End of data)
 * WARNING: writing data out of bounds is Undefined Behaviour.
 * Pro Tip : This callback may be used as audio generator :)
 */
typedef size_t (ju_stream_func_t)(void* p, ju_sample_t* x, size_t s);

#include <ju_context.h>
#include <ju_port.h>
#include <ju_osc.h>
