/* Jackutils library.
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

#include <jackutils.h>
#include <threads.h>
#include <ju_bits_ctx.h>
#include <stdio.h>

static void (error_cb) (const char* s, int i) {
	fprintf(stderr, "JACKUTILS: %s (code %i)\n", s, i);
}

static void(*error)(const char*, int) = error_cb;

JU_API jack_port_t* ju_port_rawget(ju_ctx_t* x, int i) {
	return get_port_j(x, i);
}

JU_API int ju_port_rawfind(ju_ctx_t* x, jack_port_t* p, int b) {
	int empity = -1;
	for (int i = 0; i < x->last_port; i++) {
		ju_port_t* port = x->ports + i;
		if (port->port == p) { // port is already here
			port->refcnt++;
			return i;
		} else if (!port->port) {
			empity = i;
		}
	}
	// not founded? Create new! :D
	if (empity < 0) {
		if (x->last_port == JU_MAX_PORTS) return -1; // error
		empity = x->last_port;
		x->last_port++;
	}
	PORTGETS(x, empity).port = p;
	PORTGETS(x, empity).not_unregister = b;
	PORTGETS(x, empity).refcnt = 1;
	return empity; 
}

//jack_port_by_id

JU_API int  ju_port_open (ju_ctx_t* x, ju_cstr_t n, enum ju_port_type mode, int f) {
	if (mode != JU_INPUT && mode != JU_OUTPUT) return -1;
	jack_port_t* p = jack_port_register(x->client,
		n, JACK_DEFAULT_AUDIO_TYPE,
		(mode == JU_INPUT ? JackPortIsInput : JackPortIsOutput) | f,
		0);
	if (!p) return -1;
	int r = ju_port_rawfind(x, p, 0);
	if (r < 0) jack_port_unregister(x->client, p);
	return r;
}

JU_API void ju_port_close(ju_ctx_t* x, int p) {
	ju_port_t* r = get_port_s(x, p);
	if (!r) return;
	if (r->refcnt) r->refcnt--;
	if (!r->refcnt && jack_port_is_mine(x->client, r->port)) {
		if (!r->not_unregister) jack_port_unregister(x->client, r->port);
		x->ports[p].port = NULL;
	}
};

JU_API void ju_port_take (ju_ctx_t* x, int p) {
	ju_port_t* r = get_port_s(x, p);
	if (!r) return;
	if (r->refcnt) r->refcnt++;	
}

JU_API int ju_port_connected(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port_j(x, p);
	return r ? jack_port_connected(r) : 0;
}

JU_API int ju_port_mine(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port_j(x, p);
	return r ? jack_port_is_mine(x->client, r) : 0;
} 

// oh... of course

JU_API int ju_port_find(ju_ctx_t* x, ju_cstr_t s) {
	jack_port_t* r = jack_port_by_name(x->client, s);
	if (!r) return -1;
	int i = ju_port_rawfind(x, r, 0);
	return i;
}

JU_API int ju_port_next(ju_ctx_t* x, int p, enum ju_port_type t) {
	if (p < -1 || p > x->last_port || !x->ports[p].port) return -1; // allow -1 here
	if (p != -1) ju_port_close(x, p); // close current port
	do {
		p++;
		if (x->ports[p].port 
			&& (is_input(x, p) ? JU_INPUT : JU_OUTPUT) == t
			&& ju_port_mine(x, p)) {
				ju_port_take(x, p); // refinc
				return p;
		};
	} while (p < x->last_port); 
	return -1;
}

//////////////////////////

static int (is_inproc) (ju_ctx_t* x) {
	#ifndef NDEBUG
	int i = 0;
	i = mtx_trylock(&x->mutex);
	if (!i) mtx_unlock(&x->mutex);
	if (!i) {
		error("Attempt to call JU_PROC function outside of JackAudioProcessCallback!", 0);
	}
	return i;
	#else
	return 1;
	#endif
}

JU_PROC const ju_sample_t* (ju_port_read) (ju_ctx_t* x, int p) {
	if (!is_inproc(x)) return NULL;
	jack_port_t* r = get_port_j(x, p);
	return r && is_input(x, p) ? jack_port_get_buffer(r, ju_length(x)) : NULL;
}

#define MIN(A, B) ((A) < (B) ? (A) : (B))

JU_PROC size_t (ju_port_write) (ju_ctx_t* x, int p, const ju_sample_t* s, size_t len)  {
	if (!is_inproc(x)) return 0;
	jack_port_t* r = get_port_j(x, p);
	if (!r || is_input(x, p) != 0) {
		error("Writing to invalid port!", p); return 0;
	}
	size_t xlen = ju_length(x);
	len = MIN(len, xlen);
	ju_sample_t* b = jack_port_get_buffer(r, xlen);
	memcpy(b, s, len * sizeof(ju_sample_t));
	return len;
}
JU_PROC void  (ju_port_write_stream) (ju_ctx_t* x, int p, size_t (*cb)(void*, ju_sample_t*, size_t), void* ud) {
	if (!is_inproc(x)) return;
	jack_port_t* r = get_port_j(x, p);
	if (!r || is_input(x, p) != 0) {
		error("Writing to invalid port!", p); return;
	}
	size_t req = ju_length(x);
	ju_sample_t* orig = jack_port_get_buffer(r, req);
	size_t pos = 0;
	while (pos < req) {
		size_t old = pos;
		pos += cb(ud, orig + pos, req - pos);
		if (old == pos) {
			break; // no data available
		}
	}
}


