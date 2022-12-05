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
		C(S,_move)(b, -v);
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

JBU_API void C(S,_init  )(ju_buff_t* b, size_t s) {
	assert(b != NULL);
	b->data = calloc(s, 1);
	assert(b->data);
	b->len = s;
	b->pos = 0;
	b->except = buff_except;
	mtx_init(&b->lock, 0);
}
JBU_API void C(S,_uninit)(ju_buff_t* b) {
	free(b->data);
	b->data = 0;
	mtx_destroy(&b->lock);
}
JBU_API void C(S,_resize)(ju_buff_t* b, size_t s) {
	b->data = realloc(b->data, 1 * s);
	assert(b->data);
	b->len = s;
}

JBU_API void C(S,_except)(ju_buff_t* b, int (*cb) (ju_buff_t*, ju_ssize_t)) {
	b->except = cb;
}

JBU_API size_t C(S,_size)(ju_buff_t* b) {
	return b->len;
}
JBU_API size_t C(S,_used)(ju_buff_t* b) {
	return b->pos;
}
JBU_API size_t C(S,_space)(ju_buff_t* b){
	return b->len - b->pos;
}

JBU_API size_t C(S,_append)(ju_buff_t* b, const void* src, size_t s) {
	s = MIN(s, b->len);
	if (s > C(S,_space)(b)) {
		if (b->except) b->except(b, s - C(S,_space)(b));
		// if exception not fixed this
		if (s > C(S,_space)(b)) s = C(S,_space)(b);
	};
	if (s == 0) return 0;
	memcpy(b->data + b->pos, src, s);
	b->pos += s;
	return s;
}

JBU_API size_t C(S,_fill)(ju_buff_t* b, const void* src, size_t s, size_t n) {
	size_t ms = MIN(s * n, b->len);
	if (ms > C(S,_space)(b)) {
		if (b->except) b->except(b, ms - C(S,_space)(b));
		// if exception not fixed this
		if (ms > C(S,_space)(b)) ms = C(S,_space)(b);
	};
	n = ms / s; // new count
	if (ms == 0) return 0;
	for (size_t i = 0; i < ms; i++)
		((char*)b->data)[b->pos + i] = ((char*)src)[i % s];
	b->pos += ms;
	return ms;
}

JBU_API size_t C(S,_remove)(ju_buff_t* b, void* dst, size_t s) {
	s = MIN(s, b->len);
	if (s > b->pos) {
		if (b->except) b->except(b, (ju_ssize_t)b->pos - s);
		// if exception not fixed this :/
		if (s > b->pos) s = b->pos;
	}
	if (s == 0) return 0;
	memcpy(dst, b->data, s);
	C(S,_move)(b, -((ju_ssize_t)s));
	return s;
}
JBU_API void   C(S,_move  )(ju_buff_t* b, ju_ssize_t v) {
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

JBU_API size_t C(S,_read )(ju_buff_t* b, int fd, size_t s) {
	s = MIN(s, b->len);
	if (s > C(S,_space)(b)) {
		if (b->except) b->except(b, s - C(S,_space)(b));
		// if exception not fixed this
		if (s > C(S,_space)(b)) s = C(S,_space)(b);
	};
	if (s == 0) return 0;
	ju_ssize_t r = read(fd, b->data + b->pos, s);
	if (r > 0) b->pos += r;
	return r;	
}
JBU_API size_t C(S,_write)(ju_buff_t* b, int fd, size_t s) {
	s = MIN(s, b->len);
	if (s > b->pos) {
		if (b->except) b->except(b, (ju_ssize_t)b->pos - s);
		// if exception not fixed this :/
		if (s > b->pos) s = b->pos;
	}
	if (s == 0) return 0;
	ju_ssize_t r = write(fd, b->data, s);
	if (r > 0) C(S,_move)(b, -r);
	return r;
}

JBU_API void*   C(S,_data)(ju_buff_t* b) {
	return b->data;
}

JBU_API void   C(S,_lock)(ju_buff_t* b) {
	mtx_lock(&b->lock);
}
JBU_API void   C(S,_unlock)(ju_buff_t* b) {
	mtx_unlock(&b->lock);
}

