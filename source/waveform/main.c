// JackUtils library Utility : Waveform (Visualizer)
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <jwinutils.h>

#include <stdlib.h>
#include <stdio.h>

int port;

ju_buff_t buff;
jack_nframes_t oldsize = 0;

void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x)) {
		oldsize = ju_length(x);
#ifndef NDEBUG
		fprintf(stderr, "buffer size changed to %i\n", oldsize);
#endif
		ju_buff_resize(&buff, oldsize * sizeof(float));
	}
}

// jack process callback
void process(ju_ctx_t* ctx, size_t len) {
	// cache to buffer (lock to prevent data racing)
	ju_buff_lock(&buff);
	check_buffer(ctx);
	ju_buff_append(&buff, ju_port_read(ctx, port), len*sizeof(float));
	ju_buff_unlock(&buff);
}


void loop(ju_ctx_t* ctx, ju_win_t* w);

int main(void) {
	// create context
	ju_ctx_t* ctx = ju_ctx_init("waveform", NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// open window and cache buffers
	ju_win_t* w = ju_win_open(640, 480);
	ju_win_title(w, ju_get_name(ctx));
	oldsize = ju_length(ctx);
	ju_buff_init(&buff, oldsize * sizeof(float));
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies or window will be closed xD
	while (!ju_win_should_close(w) && ju_is_online(ctx, 0)) {
		loop(ctx, w);
	}
	// free context (and window... and buffer xD)
	// context will be already stopped in case of server disconnect
	ju_win_close(w);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

void loop(ju_ctx_t* ctx, ju_win_t* w) {
	double dt = ju_draw_begin(w);
	w_wh_t ws = ju_win_size(w);

	// to prevent data racing
	ju_buff_lock(&buff);
	size_t sz = oldsize;
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	ju_draw_samples(tmp, sz, 0, ws.h/2, ws.w, ws.h/2);
	ju_win_pool_events();
}
