/* FIFO Buffer Extension for jackutils library.
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

/**
 * @file ju_buffer.h
 * Functions for fixed-size threadsafe FIFO buffer.
 * This buffer uses **char** as data type!
 * So use not sample buffers length, but sample buffers sizes.
 *
 * Sample buffer size == Sample buffer Length * sizeof(@ref ju_sample_t)
 */

#pragma once

#include <ju_defs.h>
#include <threads.h>

/**
 * typedef wrapper around buffer structure
 */
typedef struct ju_buff_s ju_buff_t; 

/**
 * FIFO Buffer internal structure.
 */
struct ju_buff_s {
	char* data; /*< raw buffer */
	size_t pos; /*< position in buffer */
	size_t len; /*< size of the buffer in bytes */
	mtx_t lock; /*< mutex */
	int (*except) (ju_buff_t*, ju_ssize_t); /*< owerflow/underflow handler */
};

#define JBU_API

/**
 * Initializes buffer structure.
 *
 * @param b pointer on buffer structure.
 * @param s size of the buffer.
 */
JBU_API void (ju_buff_init)  (ju_buff_t* b, size_t s);

/**
 * Deinitializes buffer structure.
 *
 * @param b pointer on buffer structure.
 */
JBU_API void (ju_buff_uninit)(ju_buff_t* b);

/**
 * Resizes buffer structure.
 * Buffer must be initialized before!
 *
 * @param b pointer on buffer structure.
 * @param s size of the buffer.
 */
JBU_API void (ju_buff_resize)(ju_buff_t* b, size_t s);

/**
 * Check size of buffer with requested, and resizes if there is not enough spce.
 *
 * @param b pointer on buffer structure.
 * @param s requested size of the buffer.
 */
JBU_API void (ju_buff_check_size)(ju_buff_t* b, size_t s);

/**
 * Underflow/owerflow exception handling.
 * Sets cb callback to be runned when exeption happens with buffer underflow(-) or owerflow(+) value.
 * Callback may MOVE buffer data backward to give space for new data in case of owerflow(default), or
 * do nothing. In case of underflow, callback may add zeros to buffer, or do nothing(default).
 * 
 * Or even callback may print error message, and do some stuff of course :)
 * Callback underflow/overflow value will **NEVER BE MORE THAN SIZE OF BUFFER**!
 *
 * if you did something, it's a good sign to return 0, else return nonzero value from callback;
 *
 * @param b buffer
 * @param cb new exception handler callback
 */
JBU_API void ju_buff_except(ju_buff_t* b, int (*cb) (ju_buff_t*, ju_ssize_t));

/**
 * Returns buffer summary size (used and unused).
 *
 * @param b buffer
 * @return buffer size
 */
JBU_API size_t (ju_buff_size)(ju_buff_t* b);  

/**
 * Returns size of buffer busy part.
 *
 * @param b buffer
 * @return buffer used part size
 */
JBU_API size_t (ju_buff_used)(ju_buff_t* b);

/**
 * Returns size of buffer empity part.
 *
 * @param b buffer
 * @return buffer free space size
 */
JBU_API size_t (ju_buff_space)(ju_buff_t* b);

/**
 * Adds data from array to te end of the buffer.
 *
 * @param b buffer.
 * @param src source array of bytes.
 * @param size size of the source array.
 * @return count of appended bytes
 */
JBU_API size_t (ju_buff_append)(ju_buff_t* b, const void* src, size_t size);

/**
 * Adds data from src array copied N times to the end of the buffer
 *
 * @param b buffer.
 * @param src source array of bytes.
 * @param size size of the source array.
 * @param n how much array duplicates?
 * @return count of appended bytes
 */
JBU_API size_t (ju_buff_fill  )(ju_buff_t* b, const void* src, size_t size, size_t n);

/**
 * Removes data from start of buffer to the dst array.
 *
 * @param b buffer.
 * @param dst destination array of bytes.
 * @param size how many elements to put into the array.
 * @return count of removed bytes
 */
JBU_API size_t (ju_buff_remove)(ju_buff_t*, void* dst, size_t size);

/**
 * Moves buffer data forwards or backwards...
 *
 * @param b buffer.
 * @param v move steps with sign (+ forward) (-backward)
 */
JBU_API void   (ju_buff_move) (ju_buff_t*, ju_ssize_t v);

/**
 * Similar to @ref ju_buff_append(), but reads values from the file by descriptor
 */
JBU_API size_t (ju_buff_read )(ju_buff_t*, int fd, size_t);

/**
 * Similar to @ref ju_buff_remove(), but writes values to the the file by descriptor
 */
JBU_API size_t (ju_buff_write)(ju_buff_t*, int fd, size_t);

/**
 * Returns buffer data from the beginning
 * @param buffer
 * @return pointer on the data at the beginning of the buffer
 */
JBU_API void*  (ju_buff_data)(ju_buff_t* b);

/**
 * Locks buffer. 
 * @param b buffer
 *
 * If buffer is already locked, this function waits until it unlocks, lock it again and returns.
 * This operation is atomary and threadsafe.
 *
 * Don't forget to unlock buffer when you're done reading/writing and other buffer operations.
 * Locking already locked buffer in the same thread causes to the DEADLOCK.
 */
JBU_API void   (ju_buff_lock)(ju_buff_t* b);

/*
 * Unlocks buffer. 
 * @param b buffer
 */
JBU_API void   (ju_buff_unlock)(ju_buff_t* b);

