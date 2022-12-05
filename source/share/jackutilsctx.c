// JackUtils Library - provides more simplificated JACK API for clients
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <threads.h>
#include <stdio.h>
#include <ju_bits_ctx.h>
#include <jack/jack.h>
#include <unistd.h>

static void (error_cb) (const char* s, int i) {
	fprintf(stderr, "JACKUTILS: %s (code %i)\n", s, i);
}

static void(*error)(const char*, int) = error_cb;

// buffer resize callback
static int (ju_bscb)(jack_nframes_t new, void* p) {
	ju_ctx_t* x = (ju_ctx_t*) p;
	// mutex is important for keep bufflen same!
	mtx_lock(&x->mdata);
	x->length = new;
	mtx_unlock(&x->mdata);
	return 0;
}

// sample rate changes callback
static int (ju_srcb)(jack_nframes_t new, void* p) {
	ju_ctx_t* x = (ju_ctx_t*) p;
	mtx_lock(&x->mdata);
	x->samplerate = new;
	mtx_unlock(&x->mdata);
	return 0;
}

static int (ju_prcb)(jack_nframes_t len, void* arg) {
	ju_ctx_t* x = (ju_ctx_t*) arg;
	mtx_lock(&x->mutex);
	x->proc_cb(x, len);
	mtx_unlock(&x->mutex);
	return 0;
}

static void (ju_shcb)(void* arg) {
	ju_ctx_t* x = (ju_ctx_t*) arg;
	error("Jack server is shudown!", 0);
	mtx_unlock(&x->works);
}

// that's right :D
static void (ju_close_check) (jack_port_id_t, jack_port_id_t b, int connect, void* arg) {
	ju_ctx_t* x = (ju_ctx_t*) arg;
	if (jack_port_by_id(x->client, b) == x->close && connect) {
		fprintf(stderr, "JACKUTILS: __close port is connected! Stopping client...\n");
		ju_stop(x);
	}
}

// API

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
	mtx_init(&p->mutex, 0);
	mtx_init(&p->works, 0);
	mtx_init(&p->mdata, 0);
	p->length = jack_get_buffer_size(p->client);
	p->samplerate = jack_get_sample_rate(p->client);

	jack_set_process_callback(p->client, ju_prcb, p);
	jack_on_shutdown(p->client, ju_shcb, p);	
	jack_set_buffer_size_callback(p->client, ju_bscb, p);
	jack_set_sample_rate_callback(p->client, ju_srcb, p);

	//setup close port
	p->close = jack_port_register(p->client, CLOSE_PORT_NAME, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput | JackPortIsTerminal, 0);
	jack_set_port_connect_callback(p->client, ju_close_check, p);
	return p;
}
JU_API void (ju_ctx_uninit) (ju_ctx_t* p) {
	// close port is user don't do that :p
	int test = mtx_trylock(&p->works);
	if (test != 0) jack_deactivate(p->client); // if working 
	mtx_unlock(&p->works);

	// close ports
	jack_port_unregister(p->client, p->close);
	for (int i = 0; i <= p->last_port; i++) {
		jack_port_t* t = PORTGETJ(p, i);
		if (t && jack_port_is_mine(p->client, t) 
			&& !PORTGETS(p, i).not_unregister) 
				jack_port_unregister(p->client, t);
		PORTGETJ(p, i) = NULL;
	}
	jack_client_close(p->client);
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
JU_API int  (ju_start) (ju_ctx_t* x, ju_process_func_t f) {
	if (!f) error("bad function ptr", 0);
	x->proc_cb = f;
	if (!f) return 1;
	if (mtx_trylock(&x->works) != 0) {
		error_cb("Can't start client!", 0);
		return 1;
	}
	return jack_activate(x->client);
}
JU_API void (ju_stop)  (ju_ctx_t* x) {
	int i = mtx_trylock(&x->works);
	if (i != 0) jack_deactivate(x->client); // if working 
	else error_cb("Can't stop client processing!", i);
	mtx_unlock(&x->works);
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


JU_PROC size_t (ju_length) (ju_ctx_t* x) {
	// mutex is important for keep bufflen same!
	mtx_lock(&x->mdata);
	size_t r = x->length;
	mtx_unlock(&x->mdata);
	return r;
}
JU_API  jack_nframes_t (ju_samplerate) (ju_ctx_t* x) {
	mtx_lock(&x->mdata);
	size_t r = x->samplerate;
	mtx_unlock(&x->mdata);
	return r;
}
