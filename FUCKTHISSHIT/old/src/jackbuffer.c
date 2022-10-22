// Extension for jackutils library.
// Sample buffer manipulation library
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <string.h>

struct ju_buff_s {
	size_t len, pos;
	mtx_t mutex;
	ju_sample_t *arr;
};


JBU_API ju_buff_t* ju_buff_init(size_t c) {
	if (!c) return NULL;
	ju_buff_t* p = calloc(sizeof(ju_buff_t), 1);
	if (!p) exit(-2);
	p->arr = calloc(sizeof(ju_sample_t), c);
	if (!p->arr) exit(-2);
	p->len = c;
	mtx_init(&p->mutex, 0);
	return p;
}
JBU_API void ju_buff_uninit(ju_buff_t* p) {
	if (!p) return;
	mtx_destroy(&p->mutex);
	free(p->arr);
	free(p);
}
JBU_API ju_buff_t* ju_buff_realloc(ju_buff_t* p, size_t c) {
	if (p) {
		p->arr = realloc(p->arr, c * sizeof(ju_sample_t));
		if (!p) exit(-2);
		size_t from = p->len;
		p->len = c;
		if (from < c)
		memset(p->arr + from, 0, sizeof(ju_sample_t) * (c - from));
		return p;
	}
	return ju_buff_init(c);
}

JBU_API size_t ju_buff_getpos(ju_buff_t* p) {
	return p->pos;
}
JBU_API size_t ju_buff_setpos(ju_buff_t* p, size_t i) {
	p->pos = (i <= p->len) ? i : p->len;
	return p->pos;
}
JBU_API size_t ju_buff_length(ju_buff_t* p) {
	return p->len;
}
JBU_API ju_sample_t* ju_buff_get(ju_buff_t* p) {
	return p->arr + p->pos;
}
JBU_API int ju_buff_add(ju_buff_t* p, ju_sample_t s) {
	if (p->pos < p->len) {
		p->arr[p->pos++] = s;
		return 0;
	}
	return 1;
}
JBU_API ju_sample_t ju_buff_remove(ju_buff_t* p) {
	if (p->pos > 0) {
		return p->arr[p->pos--];
	}
	return 0;
}
JBU_API size_t ju_buff_space(ju_buff_t* p) {
	return p->len - p->pos;
}
#define min(a, b) ((a) < (b) ? (a) : (b))

JBU_API size_t ju_buff_addarr(ju_buff_t* p, const ju_sample_t* src, size_t s) {
	size_t l = min(ju_buff_space(p), s);
	memcpy(ju_buff_get(p), src, sizeof(ju_sample_t) * l);
	p->pos += l;
	return l;
}

JBU_API size_t ju_buff_remarr(ju_buff_t* p, ju_sample_t* dst, size_t s) {
	size_t l = min(p->pos, s);
	memcpy(dst, ju_buff_get(p), sizeof(ju_sample_t) * l);
	p->pos -= l;
	return l;
}

JBU_API void ju_buff_cpyb2b(ju_buff_t* d, ju_buff_t* s, size_t l) {
	ju_buff_addarr(d, ju_buff_get(s), min(l, s->pos));
}

JBU_API void ju_buff_movb2b(ju_buff_t* d, ju_buff_t* s, size_t l) {
	d->pos += ju_buff_remarr(s, ju_buff_get(d), min(l, ju_buff_space(d)));
	
}



JBU_API void ju_buff_lock(ju_buff_t* p) {
	mtx_lock(&p->mutex);
}
JBU_API void ju_buff_unlock(ju_buff_t* p) {
	mtx_unlock(&p->mutex);
}

// HIGHLEVEL

static inline void swap(ju_sample_t* a, ju_sample_t* b) {
	ju_sample_t c = *a;
	*a = *b;
	*b = c;
}

JBU_API void ju_buff_move(ju_buff_t* p, const int d) {
	if (d == 0) return;
	const ssize_t plast = p->len - 1;

       	if (d > 0) {
		for (int i = plast - d; i > 0; i--) {
			p->arr[i + d] =  p->arr[i];
		}
	} else {
		// move
		for (size_t i = -d; i < p->len; i++) {
			p->arr[i + d] = p->arr[i];
		}
	}
}

