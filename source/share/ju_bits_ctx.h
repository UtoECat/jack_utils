// see jackutils.h for details!

/**
 * @file ju_bits_ctx.h
 * Theese data structures are INTERNAL and MUST NOT BE USED
 * IN PROGRAMS!
 */

/**
 * @addtogroup jackutilslowlevel
 * @{
 */

#pragma once

/** idk why,  but i made a macro for this :p */
#define CLOSE_PORT_NAME "__close"
/** reaability? what is this? :D */
#define JU_STAT static inline
/** magic :O */
#define S_TO_MS(V) (V * 1000000)

/**
 * internal JackUtils port structure.
 * 
 * DO NOT USE THIS IN YOUR PROGRAMS!
 */
typedef struct ju_port_s {
	jack_port_t* port; /**< JACK port */
	uint16_t refcnt;   /**< reference counter */
	bool   not_unregister; /**< false in most cases :/ */
} ju_port_t;

/**
 * Internal JackUtils context structure.
 * 
 * DO NOT USE THIS IN YOUR PROGRAMS!
 */
struct ju_ctx_s {
	jack_client_t* client; /**< JACK client */
	jack_port_t* close; /**< JACK close port :D, created if connection to the session manager fails */

	/**
	 * this mutex is locked while Jack Audio Process callback is running :)
	 */
	mtx_t mutex;

	/**
	 * This mutex is locked since ju_start called and until ju_stop is not called,
	 * or jack server/session manager is not stopped/crashed.
	 */
	mtx_t works;

	/**
	 * mutex for samplerate, sample buffers length and etc.
	 */
	mtx_t mdata;

	/**
	 * Jack Audio Processing User Callback :)
	 */
	ju_process_func_t proc_cb;

	/** protected with mutex mdata */
	size_t       length;
	/** protected with mutex mdata */
	size_t       samplerate;
	/** protected with mutex mdata */
	ju_uint8_t   connected_ports_cnt; 
	/** protected with mutex mdata */
	ju_uint8_t   last_port;
	/** protected with mutex mdata */
	ju_port_t    ports[JU_MAX_PORTS + 1];

	/** protected with mutex mdata. Session managment context */
	void* osc;
	/** protected with mutex mdata. Session path string. malloc()'d! */
	char* session_path;
	/** protected with mutex mdata. Cache for session manager event GUI turn on/off */
	int   gui_showed;
	/** protected with mutex mdata. Cache for session manager GUI extension */
	int   gui_supported;
	/** save callback and user data for it */
	ju_save_cb onsave;
	void* onsave_ud;
	/** I... Don't remember what the shit is this :D, but it protected with mdata mutex... */
	char* tmp; // don't use this!
};

/** get internal port structure by the given descriptor */
JU_STAT struct ju_port_s* get_port_s(ju_ctx_t* x, int p) {
	if (p < 0 || p > x->last_port || !x->ports[p].port) return NULL;
	return x->ports + p;
}

/** get jack_port_t by the given descriptor */
JU_STAT jack_port_t* get_port_j(ju_ctx_t* x, int p) {
	if (p < 0 || p > x->last_port || !x->ports[p].port) return NULL;
	return x->ports[p].port;
}

/** check is port has JU_INPUT type by given descriptor */
JU_STAT int is_input(ju_ctx_t* x, int p) {
	ju_port_t* r = get_port_s(x, p);
	return r ? jack_port_flags(r->port) & JackPortIsInput : -1;	
}

#define PORTGETS(X, I) (X->ports[I])
#define PORTGETJ(X, I) PORTGETS(X, I).port

/**
 * @}
 */
