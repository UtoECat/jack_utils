// FIFO buffer manipulation library
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

/*
 * JackBuffer - fixed-size threadsafe FIFO buffer.
 * It uses char type as default :p
 */

#pragma once

#include <ju_defs.h>
#include <threads.h>
#include <template/macro.h>

#define S ju_buff
#define C(A, B) CONCAT(A, B)


typedef struct C(S, _s) ju_buff_t; 

struct C(S, _s) {
	char* data;
	size_t pos;
	size_t len;
	mtx_t lock;
	int (*except) (ju_buff_t*, ju_ssize_t);
};

#define JBU_API

JBU_API void C(S,_init)  (ju_buff_t*, size_t);
JBU_API void C(S,_uninit)(ju_buff_t*);
JBU_API void C(S,_resize)(ju_buff_t*, size_t);

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
JBU_API void C(S,_except)(ju_buff_t*, int (*cb) (ju_buff_t*, ju_ssize_t));

JBU_API size_t C(S,_size)(ju_buff_t*);  // buffer capacity (change using resize!)
JBU_API size_t C(S,_used)(ju_buff_t*);  // how many data buffer contains now
JBU_API size_t C(S,_space)(ju_buff_t*); // how many data we can write to buffer?

/*
 * Adds data from source array to end of buffer.
 * Returns count of sucessfully writed elements.
 */
JBU_API size_t C(S,_append)(ju_buff_t*, const void* src, size_t size);

/*
 * Fills buffer with data from Source of size Size, copied N times.
 *
 * FE this code :
 * > float val = 0.3f;
 * > ju_buff_fill(buff, &val, sizeof(float), 50);
 * adds 50 float values to the buffer :)
 */
JBU_API size_t C(S,_fill  )(ju_buff_t*, const void* src, size_t size, size_t n);

JBU_API size_t C(S,_remove)(ju_buff_t*, void* dst, size_t size);
JBU_API void   C(S,_move  )(ju_buff_t*, ju_ssize_t v);

JBU_API size_t C(S,_read )(ju_buff_t*, int fd, size_t);
JBU_API size_t C(S,_write)(ju_buff_t*, int fd, size_t);
				
JBU_API void*  C(S,_data)(ju_buff_t* b);

JBU_API void   C(S,_lock)(ju_buff_t*);
JBU_API void   C(S,_unlock)(ju_buff_t*);

#ifndef JU_IMPLEMENTATION
#undef S
#undef C
#undef JBU_API
#endif
