// Extension for jackutils library.
// Sample buffer manipulation library
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

#define JBU_API

/*
 * JackBuffer - fixed-size threadsafe sample arrray.
 * Maked specially for jackutils. :P
 */

JBU_API ju_buff_t* ju_buff_init(size_t c);
JBU_API void ju_buff_uninit(ju_buff_t*);

JBU_API size_t ju_buff_getpos(ju_buff_t*);
JBU_API size_t ju_buff_setpos(ju_buff_t*, size_t);
JBU_API size_t ju_buff_length(ju_buff_t*);
JBU_API ju_sample_t* ju_buff_get(ju_buff_t*); // data at pos

JBU_API size_t ju_buff_add(ju_buff_t*, const ju_sample_t*, size_t);
JBU_API size_t ju_buff_add_val(ju_buff_t*, ju_sample_t, size_t);
JBU_API size_t ju_buff_add_buff(ju_buff_t*, const ju_buff_t*, size_t);

JBU_API size_t ju_buff_remove(ju_buff_t*, ju_sample_t*, size_t);

JBU_API void ju_buffer_lock(ju_buffer_t*);
JBU_API void ju_buffer_unlock(ju_buffer_t*);

/*
 * HIGH LEVEL API
 */

JBU_API void ju_buffer_append(ju_buffer_t*)
