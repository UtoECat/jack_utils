/* FIFO Extension for jackutils library.
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

/*
 * JackBuffer - fixed-size threadsafe FIFO buffer.
 * It uses char type as default :p
 */

#pragma once

#include <ju_defs.h>
#include <threads.h>

typedef struct ju_buff_s ju_buff_t; 

struct ju_buff_s {
	char* data;
	size_t pos;
	size_t len;
	mtx_t lock;
	int (*except) (ju_buff_t*, ju_ssize_t);
};

#define JBU_API

JBU_API void (ju_buff_init)  (ju_buff_t*, size_t);
JBU_API void (ju_buff_uninit)(ju_buff_t*);
JBU_API void (ju_buff_resize)(ju_buff_t*, size_t);

// checks size and realloc if space is not enough
JBU_API void (ju_buff_check_size)(ju_buff_t* b, size_t s);
/*
 * Underflow/owerflow exception handling.
 * Runs @arg2 callback with buffer and underflow(-) or owerflow(+) value if it happens.
 * Buffer may MOVE buffer data backward to give space for new data in case of owerflow(default), or
 * do nothing. In case of underflow, buffer may add zeros to buffer, or do nothing(default).
 * 
 * Or even callback may print error message, and do some stuff of course :)
 * Callback underflow/overflow value will NEVER BE MORE THAN SIZE OF BUFFER!
 *
 * if you did something, it's a good sign to return 0, else return nonzero value from callback;
 */
JBU_API void ju_buff_except(ju_buff_t*, int (*cb) (ju_buff_t*, ju_ssize_t));

JBU_API size_t (ju_buff_size)(ju_buff_t*);  // buffer capacity (change using resize!)
JBU_API size_t (ju_buff_used)(ju_buff_t*);  // how many data buffer contains now
JBU_API size_t (ju_buff_space)(ju_buff_t*); // how many data we can write to buffer?

/*
 * Adds data from source array to end of buffer.
 * Returns count of sucessfully writed elements.
 */
JBU_API size_t (ju_buff_append)(ju_buff_t*, const void* src, size_t size);

/*
 * Fills buffer with data from Source of size Size, copied N times.
 *
 * FE this code :
 * > float val = 0.3f;
 * > ju_buff_fill(buff, &val, sizeof(float), 50);
 * adds 50 float values to the buffer :)
 */
JBU_API size_t (ju_buff_fill  )(ju_buff_t*, const void* src, size_t size, size_t n);

JBU_API size_t (ju_buff_remove)(ju_buff_t*, void* dst, size_t size);
JBU_API void   (ju_buff_move  )(ju_buff_t*, ju_ssize_t v);

JBU_API size_t (ju_buff_read )(ju_buff_t*, int fd, size_t);
JBU_API size_t (ju_buff_write)(ju_buff_t*, int fd, size_t);
				
JBU_API void*  (ju_buff_data)(ju_buff_t* b);

JBU_API void   (ju_buff_lock)(ju_buff_t*);
JBU_API void   (ju_buff_unlock)(ju_buff_t*);

