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
#include <stdlib.h>

static void (error_cb) (const char* s, int i) {
	fprintf(stderr, "JACKUTILS: %s (code %i)\n", s, i);
}

static void(*error)(const char*, int) = error_cb;

// include mixed nsm.h header
#include <ext/nsm.h>

// some callbacks
static int cb_open (const char *session_path, const char*, const char *new_name, char**, void *ud) {                                                                      
	ju_ctx_t* p = ((ju_ctx_t*)ud);
	p->session_path = strdup(session_path);	
	p->tmp = strdup(new_name);
	return ERR_OK;                                                 
}

static int cb_save (char**, void* ud) {
	ju_ctx_t* p = ((ju_ctx_t*)ud);
	fprintf(stderr, "JACKUTILS: session manager stops us...\n");
	ju_stop(p);
	return ERR_OK;
}

static void cb_show(void* ud) {
	ju_ctx_t* p = ((ju_ctx_t*)ud);
	ju_set_gui(p, 1);
}

static void cb_hide(void* ud) {
	ju_ctx_t* p = ((ju_ctx_t*)ud);
	ju_set_gui(p, 0);
}

int ju_internal_try_osc(ju_ctx_t* p, const char** name, const char* argv0, int has_gui) {
	// context is not fully initializaed at this moment :p
	const char* nsm_url = getenv("NSM_URL");
	p->gui_showed = 1; p->gui_supported = 0;
	p->tmp = NULL;

	if (nsm_url) {
		fprintf(stderr, "JACKUTILS : NSM_URL = %s\n", nsm_url);
		p->osc = nsm_new();
		nsm_set_open_callback(p->osc, cb_open, p);
		nsm_set_save_callback(p->osc, cb_save, p);
		if (nsm_init(p->osc, nsm_url) != 0) goto fail;
		nsm_send_announce(p->osc, *name, has_gui ? ":optional-gui:" : "", argv0);
		while(!p->tmp) nsm_check_wait(p->osc, 500);
		*name = p->tmp;
		if (strstr(nsm_get_session_manager_features(p->osc), ":optional-gui:")) {
			p->gui_supported = 1;
			nsm_set_show_callback(p->osc, cb_show, p);
			nsm_set_hide_callback(p->osc, cb_hide, p);
		}
	} else {
		fail :
		fprintf(stderr, "JACKUTILS : Can't connect to session manager!\n");
		return 0; // unsucessfully
	}
}

void ju_internal_free_osc(ju_ctx_t* p) {
	if (p->osc) {
		nsm_free(p->osc);
		p->osc = NULL;
	}
	if (p->session_path) {
		free(p->session_path);
		p->session_path = NULL;
	}
	if (p->tmp) free(p->tmp);
}

/*
 * Returns is GUI enabled for us.
 * @arg context
 * @ret 1 if enabled.
 */
JU_API int (ju_need_gui) (ju_ctx_t* p) {
	mtx_lock(&p->mdata);
	int v = p->gui_showed;
	mtx_unlock(&p->mdata);
	return v;
}

/*
 * Set ju_need_gui returned value and send it to session manager.
 * @arg context
 * @arg show gui
 */
#include <assert.h>
JU_API void (ju_set_gui) (ju_ctx_t* p, int new) {
	new = !!new;

	mtx_lock(&p->mdata);
	int old = p->gui_showed;
	if (new != old) p->gui_showed = new;
	mtx_unlock(&p->mdata);
	if (new == old) return;
	if (new) {
		assert(p->osc != NULL); // emmm... what are you doing? :D
		nsm_send_is_shown(p->osc);
	} else {
		if (p->osc)
		nsm_send_is_hidden(p->osc);
		else ju_stop(p);
	}
	return;
}

/*
 * Returns information about session manager, if available.
 * @arg context
 * @ret session manager info string or NULL if session manager is not available
 *
 */
JU_API ju_cstr_t (ju_osc_info) (ju_ctx_t* ctx) {
	return ctx->osc ? nsm_get_session_manager_name(ctx->osc) : NULL;
}

/*
 * Returns OSC path to store your session data.
 * If there is no session manager, returns "~/.local/share/jackutils/"
 * @arg context
 * @ret path to load and save session data
 */
JU_API ju_cstr_t (ju_osc_path) (ju_ctx_t* ctx) {
	return ctx->osc ? ctx->session_path : "~/.local/share/jackutils/";
}

JU_API void ju_pool_events(ju_ctx_t* ctx) {
	if (!ctx || !ctx->osc) return;
	mtx_lock(&ctx->mdata);
	nsm_check_nowait(ctx->osc);
	mtx_unlock(&ctx->mdata);
}
