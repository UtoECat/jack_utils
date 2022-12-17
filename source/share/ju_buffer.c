// FIFO buffer manipulation library
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#define JU_IMPLEMENTATION
#include <ju_buffer.h>

/*
 *
 * IMPLEMENTATION
 *
 */
#include <assert.h>
#include <stdio.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
//#define BUFFER_DEBUG

static int buff_except(ju_buff_t* b, ju_ssize_t v) {
	if (v > 0) { // overflow
		// move backward
		ju_buff_move(b, -v);
#ifdef BUFFER_DEBUG
		fprintf(stderr, "BUFF(%p) : Overflow %li (used %li)\n", b, v, b->pos);
#endif
		return 0;
	} else { // underflow
		// do nothing
#ifdef BUFFER_DEBUG
		fprintf(stderr, "BUFF(%p) : Underflow %li (used %li)\n", b, -v, b->pos);
#endif
		return -1;
	}
}

JBU_API void (ju_buff_init  )(ju_buff_t* b, size_t s) {
	assert(b != NULL);
	b->data = calloc(s, 1);
	assert(b->data);
	b->len = s;
	b->pos = 0;
	b->except = buff_except;
	mtx_init(&b->lock, 0);
}
JBU_API void (ju_buff_uninit)(ju_buff_t* b) {
	free(b->data);
	b->data = 0;
	mtx_destroy(&b->lock);
}

JBU_API void (ju_buff_resize)(ju_buff_t* b, size_t s) {
	if (s == b->len) return;
	b->data = realloc(b->data, 1 * s);
	assert(b->data);
	b->len = s;
}

JBU_API void (ju_buff_check_size)(ju_buff_t* b, size_t s) {
	if (s <= b->len) return;
	b->data = realloc(b->data, 1 * s);
	assert(b->data);
	b->len = s;
}

JBU_API void (ju_buff_except)(ju_buff_t* b, int (*cb) (ju_buff_t*, ju_ssize_t)) {
	b->except = cb;
}

JBU_API size_t (ju_buff_size)(ju_buff_t* b) {
	return b->len;
}
JBU_API size_t (ju_buff_used)(ju_buff_t* b) {
	return b->pos;
}
JBU_API size_t (ju_buff_space)(ju_buff_t* b){
	return b->len - b->pos;
}

JBU_API size_t (ju_buff_append)(ju_buff_t* b, const void* src, size_t s) {
	s = MIN(s, b->len);
	if (s > (ju_buff_space)(b)) {
		if (b->except) b->except(b, s - (ju_buff_space)(b));
		// if exception not fixed this
		if (s > (ju_buff_space)(b)) s = (ju_buff_space)(b);
	};
	if (s == 0) return 0;
	memcpy(b->data + b->pos, src, s);
	b->pos += s;
	return s;
}

JBU_API size_t (ju_buff_fill)(ju_buff_t* b, const void* src, size_t s, size_t n) {
	size_t ms = MIN(s * n, b->len);
	if (ms > (ju_buff_space)(b)) {
		if (b->except) b->except(b, ms - (ju_buff_space)(b));
		// if exception not fixed this
		if (ms > (ju_buff_space)(b)) ms = (ju_buff_space)(b);
	};
	n = ms / s; // new count
	if (ms == 0) return 0;
	for (size_t i = 0; i < ms; i++)
		((char*)b->data)[b->pos + i] = ((char*)src)[i % s];
	b->pos += ms;
	return ms;
}

JBU_API size_t (ju_buff_remove)(ju_buff_t* b, void* dst, size_t s) {
	size_t fulls = s;
	s = MIN(s, b->len);
	if (s > b->pos) {
		if (b->except) b->except(b, (ju_ssize_t)b->pos - s);
		// if exception not fixed this :/
		if (s > b->pos) s = b->pos;
	}
	if (s == 0) return 0;
	memcpy(dst, b->data, s);
	memset(dst + fulls - s, 0, fulls - s);
	(ju_buff_move)(b, -((ju_ssize_t)s));
	return s;
}
JBU_API void   (ju_buff_move  )(ju_buff_t* b, ju_ssize_t v) {
	if (v > (ju_ssize_t)b->len) v = b->len;
	if (-v > (ju_ssize_t)b->pos) v = -b->pos;
	
	ju_ssize_t abs = (v > 0) ? v : -v;
	ju_ssize_t p = (v > 0) ? v  : 0;
	ju_ssize_t n = (v < 0) ? -v : 0;
	
	memmove(b->data + p, b->data + n, b->len - abs);
	b->pos += v;
	if (b->pos > b->len) b->pos = b->len;
}

#include <unistd.h>

JBU_API size_t (ju_buff_read )(ju_buff_t* b, int fd, size_t s) {
	s = MIN(s, b->len);
	if (s > (ju_buff_space)(b)) {
		if (b->except) b->except(b, s - (ju_buff_space)(b));
		// if exception not fixed this
		if (s > (ju_buff_space)(b)) s = (ju_buff_space)(b);
	};
	if (s == 0) return 0;
	ju_ssize_t r = read(fd, b->data + b->pos, s);
	if (r > 0) b->pos += r;
	return r;	
}
JBU_API size_t (ju_buff_write)(ju_buff_t* b, int fd, size_t s) {
	s = MIN(s, b->len);
	if (s > b->pos) {
		if (b->except) b->except(b, (ju_ssize_t)b->pos - s);
		// if exception not fixed this :/
		if (s > b->pos) s = b->pos;
	}
	if (s == 0) return 0;
	ju_ssize_t r = write(fd, b->data, s);
	if (r > 0) (ju_buff_move)(b, -r);
	return r;
}

JBU_API void*   (ju_buff_data)(ju_buff_t* b) {
	return b->data;
}

JBU_API void   (ju_buff_lock)(ju_buff_t* b) {
	mtx_lock(&b->lock);
}
JBU_API void   (ju_buff_unlock)(ju_buff_t* b) {
	mtx_unlock(&b->lock);
}

