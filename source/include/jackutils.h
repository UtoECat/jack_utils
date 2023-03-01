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

#include <assert.h> // assertions 
#include <ju_defs.h>

#include <jack/jack.h> // jack

#define JU_API
#define JU_PROC

#define JU_MAX_PORTS 127 // max ports descriptors count
#define JU_INPUT  1
#define JU_OUTPUT 2

typedef unsigned char ju_uint8_t;
typedef const char*   ju_cstr_t;
typedef jack_default_audio_sample_t ju_sample_t;
typedef struct ju_ctx_s ju_ctx_t;

#include <ju_buffer.h>

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
 * If callback returns 0, it will not be called again! (means End of data)
 * WARNING: writing data out of bounds is Undefined Behaviour.
 * Pro Tip : This callback may be used as audio generator :)
 */
typedef size_t (ju_stream_func_t)(void*, ju_sample_t*, size_t);

#include <ju_context.h>
#include <ju_port.h>
#include <ju_osc.h>
