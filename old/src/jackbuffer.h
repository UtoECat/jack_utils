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

JBU_API ju_buff_t* ju_buff_realloc(ju_buff_t*, size_t c);

JBU_API size_t ju_buff_getpos(ju_buff_t*);
JBU_API size_t ju_buff_setpos(ju_buff_t*, size_t);
JBU_API size_t ju_buff_length(ju_buff_t*);
JBU_API size_t ju_buff_space(ju_buff_t*);
JBU_API ju_sample_t* ju_buff_get(ju_buff_t*); // data at pos

/*
 * adds one sample in position, and inctrements position
 * @return 0 if sucess
 */
JBU_API int ju_buff_add(ju_buff_t* p, ju_sample_t s);
/*
 * returns sample from buffer in pos, and decrements position
 */
ju_sample_t ju_buff_remove(ju_buff_t* p);

/*
 * puts data from array @2 a to buffer at position.
 * not all data may be copied
 */
JBU_API size_t ju_buff_addarr(ju_buff_t*, const ju_sample_t*, size_t);
JBU_API size_t ju_buff_remarr(ju_buff_t* p, ju_sample_t* dst, size_t s);

/*
 * Copies L samples(or less) from buffer s to buffer d
 */
JBU_API void ju_buff_cpyb2b(ju_buff_t* d, ju_buff_t* s, size_t l);
/*
 * Same, but removes it from source buffer after
 */
JBU_API void ju_buff_movb2b(ju_buff_t* d, ju_buff_t* s, size_t l);

JBU_API void   ju_buff_move(ju_buff_t*, const int);

JBU_API void   ju_buff_lock(ju_buff_t*);
JBU_API void   ju_buff_unlock(ju_buff_t*);

