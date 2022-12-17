// see jackutils.h for details!

/*
 * Theese data structures are INTERNAL and MUST NOT BE USED
 * IN PROGRAMS!
 */
#pragma once

#define CLOSE_PORT_NAME "__close"
#define JU_STAT static inline
#define S_TO_MS(V) (V * 1000000)

typedef struct ju_port_s {
	jack_port_t* port;
	uint16_t refcnt;
	bool   not_unregister; // false in most cases :/
} ju_port_t;

struct ju_ctx_s {
	jack_client_t* client;
	jack_port_t* close; // close port :)
	// protection mutexes
	// works - locked until ju_start called and ju_stop is not called
	// mdata - lock for samplerate, sample buffers length
	// mutex - locked while Jack Audio Process function is working :p
	// init callback is successed
	mtx_t mutex, works, mdata;
	ju_process_func_t proc_cb;
	// mdata protection begin
	size_t       length;
	size_t       samplerate;
	ju_uint8_t   connected_ports_cnt; // count of connected ports :)
	ju_uint8_t   last_port;
	ju_port_t    ports[JU_MAX_PORTS + 1];
	// session manager
	void* osc;
	char* session_path; // malloc()'ed 
	int   gui_showed;
	int   gui_supported;
	char* tmp; // don't use this!
	// mdata protection end
};

// internal helpers

JU_STAT struct ju_port_s* get_port_s(ju_ctx_t* x, int p) {
	if (p < 0 || p > x->last_port || !x->ports[p].port) return NULL;
	return x->ports + p;
}

JU_STAT jack_port_t* get_port_j(ju_ctx_t* x, int p) {
	if (p < 0 || p > x->last_port || !x->ports[p].port) return NULL;
	return x->ports[p].port;
}

JU_STAT int is_input(ju_ctx_t* x, int p) {
	ju_port_t* r = get_port_s(x, p);
	return r ? jack_port_flags(r->port) & JackPortIsInput : -1;	
}

#define PORTGETS(X, I) (X->ports[I])
#define PORTGETJ(X, I) PORTGETS(X, I).port
