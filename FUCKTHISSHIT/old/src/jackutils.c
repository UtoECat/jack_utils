// JackUtils Library - provides more simplificated JACK API for clients
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <threads.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

static void (error_cb) (const char* s, int i) {
	fprintf(stderr, "JACKUTILS: %s (code %i)\n", s, i);
}

static typeof(error_cb)* error = error_cb;

struct ju_ctx_s {
	jack_client_t* client;
	ju_cstr_t name;
	GLFWwindow* win; 
	// helpers mutex :p
	mtx_t mutex, works, mdata;
	ju_process_func_t proc_cb;
	size_t length;
	void *udata;
	ju_uint8_t   last_port;
	jack_port_t* ports[JU_MAX_PORTS + 1];
};

// STATIC
static jack_port_t* get_port(ju_ctx_t* x, int p) {
	if (p < 1 || p > x->last_port || !x->ports[p]) return NULL;
	return x->ports[p];
}

static int is_input(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port(x, p);
	return r ? jack_port_flags(r) & JackPortIsInput : -1;	
}

// Buffer resize callback

static int (ju_bscb)(jack_nframes_t new, void* p) {
	ju_ctx_t* x = (ju_ctx_t*) p;
	// mutex is important for keep bufflen same!
	mtx_lock(&x->mdata);
	x->length = new;
	mtx_unlock(&x->mdata);
}

// TODO : add latency support?
static int ju_process(jack_nframes_t len, void* arg) {
	ju_ctx_t* x = (ju_ctx_t*) arg;

	mtx_lock(&x->mutex);
	x->proc_cb(x, len);
	mtx_unlock(&x->mutex);
	return 0;
}

static void ju_shutdown(void* arg) {
	ju_ctx_t* x = (ju_ctx_t*) arg;
	error("Jack server is shudown!", 0);
	mtx_unlock(&x->works);
}

static int in_process(ju_ctx_t* x) {
#ifndef NDEBUG
	int i = mtx_trylock(&x->mutex);
	if (!i) mtx_unlock(&x->mutex);
	return i;
#else
	return 1; // let it works in release build
#endif
}

JU_API ju_ctx_t* (ju_ctx_init)  (ju_cstr_t name, ju_cstr_t server) {
	jack_status_t st;
	name = name ? name : "unnamed";
	jack_client_t* cli = jack_client_open(name, server?JackServerName : JackNullOption, &st, server ? server : "default");
	if (!cli) {
		error("Can't open jack client!", st);
		return NULL;
	}
	ju_ctx_t* p = calloc(sizeof(ju_ctx_t), 1);
	if (!p) {
		error("Memory allocation error!", 0);
		return NULL;
	}
	p->client = cli;
	p->name = name;
	mtx_init(&p->mutex, 0);
	mtx_init(&p->works, 0);
	mtx_init(&p->mdata, 0);
	p->length = jack_get_buffer_size(p->client);

	jack_set_process_callback(p->client, ju_process, p);
	jack_on_shutdown(p->client, ju_shutdown, p);	
	jack_set_buffer_size_callback(p->client, ju_bscb, p);
	return p;
}
JU_API void (ju_ctx_unint) (ju_ctx_t* p) {
	jack_deactivate(p->client);
	for (int i = 0; i <= p->last_port; i++) {
		if (p->ports[i] && jack_port_is_mine(
				p->client, p->ports[i]
			)) jack_port_unregister(p->client, p->ports[i]);
		p->ports[i] = NULL;
	}
	jack_client_close(p->client);
	if (p->win) glfwDestroyWindow(p->win);
	mtx_destroy(&p->mutex);
	mtx_destroy(&p->mdata);
	mtx_destroy(&p->works);
	free(p);
}

JU_API ju_cstr_t (ju_get_name) (ju_ctx_t* p) {
	return jack_get_client_name(p->client);
}
JU_API ju_cstr_t (ju_jack_info) (void) {
	return jack_get_version_string();
}
JU_API ju_cstr_t (ju_glfw_info) (void) {
	return glfwGetVersionString();
}

static int get_new_empity_port(ju_ctx_t* p) {
	int first_try = 1;
	int i = p->last_port;
	while (1) {
		i++;
		if (i >= JU_MAX_PORTS) {
			i = 1;
			if (first_try) {
				first_try = 0;
			} else {
				error("too many ports opened!", i);
				return -1;
			}
		}
		if (!p->ports[i]) {
			if (i > p->last_port) {
				p->last_port = i;
			}
			return i;
		}
	}
	abort();
}


JU_API int  ju_port_open (ju_ctx_t* p, ju_cstr_t n, int is_out, int t) {
	int i = get_new_empity_port(p);
	if (i < 0) return 0;
	p->ports[i] = jack_port_register(p->client, n, JACK_DEFAULT_AUDIO_TYPE, 
		(is_out == JU_INPUT ? JackPortIsInput : JackPortIsOutput) | t, 0);
	if (!p->ports[i]) return 0;
	return i;
}

JU_API void ju_port_close(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port(x, p);
	if (r && jack_port_is_mine(x->client, r)) jack_port_unregister(x->client, r);
	x->ports[p] = NULL; // write NULL to port pointer
};

JU_API int ju_port_connected(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port(x, p);
	return r ? jack_port_connected(r) : 0;
}

JU_API int ju_port_mine(ju_ctx_t* x, int p) {
	jack_port_t* r = get_port(x, p);
	return r ? jack_port_is_mine(x->client, r) : 0;
} 

JU_API int  (ju_start) (ju_ctx_t* x, ju_process_func_t f) {
	if (!f) error("bad function ptr", 0);
	x->proc_cb = f;
	if (!f) return 1;
	mtx_lock(&x->works);
	return jack_activate(x->client);
}
JU_API void (ju_stop)  (ju_ctx_t* x) {
	mtx_unlock(&x->works);
	jack_deactivate(x->client);
}

#include <unistd.h> // usleep

JU_API int (ju_is_online) (ju_ctx_t* x, int msec) {
	int i = 0;
	
	if (msec >= 0) {
		i = mtx_trylock(&x->works);
		usleep(msec ? msec : 1);
	} else {
		i = mtx_lock(&x->works);
	};
	
	if (!i) mtx_unlock(&x->works);
	return i;
}

JU_API void (ju_loop) (ju_ctx_t* x, void (*loop_cb) (ju_ctx_t*), int ti) {
	if (x->win) {
		while(!glfwWindowShouldClose(x->win) && ju_is_online(x, ti)) {
			if (loop_cb) loop_cb(x);
		}
	} else {
		while (ju_is_online(x, ti)) {
			if (loop_cb) loop_cb(x);
		}
	}
}

JU_API void (ju_wait) (ju_ctx_t* x) {
	ju_is_online(x, -1);
}

static void init_glfw() {
	static int B = 0;
	if (!B) {
		if (!glfwInit()) {
			error("Can't Init GLFW!", 0);
			return;
		}
		atexit(glfwTerminate);
		B = 1;
	}
}

JU_API int (ju_ctx_win) (ju_ctx_t* x, int w, int h) {
	init_glfw();
	if (x->win) return 0;
	x->win = glfwCreateWindow(w, h, x->name, NULL, NULL);
	if (!x->win) {
		error("Can't open window!", 0);
		return -1;
	}
	return 0;
}

JU_API void* (ju_ctx_win_get) (ju_ctx_t* x) {
	return x->win;
}	

#define check(DO) if (!in_process(x)) {error("Using JU_PROC function outside process callback!", 0);DO;};

JU_PROC size_t (ju_length) (ju_ctx_t* x) {
	// mutex is important for keep bufflen same!
	mtx_lock(&x->mdata);
	size_t r = x->length;
	mtx_unlock(&x->mdata);
	return r;
}
JU_API  jack_nframes_t (ju_rate) (ju_ctx_t* x) {
	return jack_get_sample_rate(x->client);
}
JU_PROC const ju_sample_t* (ju_port_read) (ju_ctx_t* x, int p) {
	check(return NULL);
	jack_port_t* r = get_port(x, p);
	return r && is_input(x, p) ? jack_port_get_buffer(r, x->length) : NULL;
}
JU_PROC void  (ju_port_write) (ju_ctx_t* x, int p, const ju_sample_t* s)  {
	check(return);
	jack_port_t* r = get_port(x, p);
	if (!r || is_input(x, p) != 0) {
		error("Writing to invalid port!", p); return;
	}
	ju_sample_t* b = jack_port_get_buffer(r, x->length);
	memcpy(b, s, x->length * sizeof(ju_sample_t));
}
JU_PROC void  (ju_port_write_stream) (ju_ctx_t* x, int p, size_t (*cb)(void*, ju_sample_t*, size_t), void* ud) {
	check(return);
	jack_port_t* r = get_port(x, p);
	if (!r || is_input(x, p) != 0) {
		error("Writing to invalid port!", p); return;
	}
	ju_sample_t* orig = jack_port_get_buffer(r, x->length);
	size_t pos = 0;
	while (pos < x->length) {
		size_t old = pos;
		pos += cb(ud, orig + pos, x->length - pos);
		if (old == pos) {
			break; // no data available
		}
	}
}

JU_API int ju_port_find(ju_ctx_t* x, ju_cstr_t s) {
	jack_port_t* r = jack_port_by_name(x->client, s);
	if (!r) return -1;
	for (int i = 1; i <= x->last_port; i++) {
		if (x->ports[i] == r) return i;
	}
	int i = get_new_empity_port(x); // if no same port opened/founded, create new desciptor for him!
	if (i < 1) return -2;
	x->ports[i] = r;
	return i;
}

JU_API int ju_port_next(ju_ctx_t* x, int p, int t) {
	if (p < 0 || p > x->last_port || !x->ports[p]) return 0; // allow 0 here
	do {
		p++;
		if (x->ports[p] && (!is_input(x, p)) == t && ju_port_mine(x, p)) return p;
	} while (p < x->last_port); 
	return 0;
}

