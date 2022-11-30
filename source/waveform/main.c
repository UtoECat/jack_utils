// JackUtils library Utility : Waveform (Visualizer)
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <jackgui.h>

#include <stdlib.h>
#include <stdio.h>
#define PROGRAM_NAME "waveform"
#define PROGRAM_VERSION 0.1
#define PROGRAM_USAGE "waveform [-h] [-v]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!"
#include <ju_args.h>

int port;
ju_buff_t buff;
jack_nframes_t oldsize = 0;

static void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x)) {
		oldsize = ju_length(x);
#ifndef NDEBUG
		fprintf(stderr, "buffer size changed to %i\n", oldsize);
#endif
		ju_buff_resize(&buff, oldsize * sizeof(float));
	}
}

// jack process callback
static void process(ju_ctx_t* ctx, size_t len) {
	// cache to buffer (lock to prevent data racing)
	ju_buff_lock(&buff);
	check_buffer(ctx);
	ju_buff_append(&buff, ju_port_read(ctx, port), len*sizeof(float));
	ju_buff_unlock(&buff);
}

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui);

int main(int argc, char** argv) {
	ja_parse(argc, argv, NULL, NULL);
	// create context
	ju_ctx_t* ctx = ju_ctx_init("waveform", NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// open GUI and cache buffers
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 640, 480);
	oldsize = ju_length(ctx);
	ju_buff_init(&buff, oldsize * sizeof(float));
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies or GUI will be closed xD
	fprintf(stderr, "Init success!\n");
	while (!jg_should_close(gui) && ju_is_online(ctx, 0)) {
		jg_begin(gui);
		loop(ctx, gui);
		jg_end(gui);
	}
	// free context (and window... and buffer xD)
	// context will be already stopped in case of server disconnect
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t sz = oldsize;
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	nk_layout_row_dynamic(gui, 30, 2);
	nk_label(gui, "(JackUtils) : Waveform visualizer", NK_TEXT_LEFT);
	nk_button_label(gui, "button");
	struct waveinfo info = {-1, 1, 0, 0, 1, 1};
	nk_layout_row_dynamic(gui, nk_window_get_content_region(gui).h - 42, 1);
	jg_waveview(gui, tmp, sz, &info);
	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
}


