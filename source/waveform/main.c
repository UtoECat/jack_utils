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
#include <about.h>

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

static float volume = 1.0f;

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t sz = oldsize;
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	// top layout
	nk_layout_row_begin(gui, NK_STATIC, 72, 2); {
		nk_layout_row_push(gui, 72);
		nk_image(gui, jg_jackutils_icon());
		nk_layout_row_push(gui, nk_window_get_content_region(gui).w - 72 - 12);
		// top group after icon
		nk_group_begin(gui, "top-group", 0);

			// setup template layout
			nk_layout_row_template_begin(gui, 40);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 80);
			nk_layout_row_template_end(gui);
			
			// first layer
			nk_label(gui, "(Jackutils) : waveform visualizer", NK_TEXT_CENTERED);
			jg_whell_float(gui, &volume, 0, 0.01, 1);
			if (nk_button_label(gui, "About")) {
				show_about();
			};
			// second layout
			nk_layout_row_template_begin(gui, 10);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 80);
			nk_layout_row_template_end(gui);
			// second layer
			nk_labelf(gui, NK_TEXT_CENTERED, "Sample array length : %li", ju_length(ctx));
			nk_label(gui, "Volume", NK_TEXT_CENTERED);
			nk_labelf(gui, NK_TEXT_CENTERED, "ver %0.1f", PROGRAM_VERSION);
		nk_group_end(gui);
	}
	nk_layout_row_end(gui);

	// bottom layout (waveform)
	struct waveinfo info = {-1/volume, 1/volume, 0, 0, 1, 1};
	nk_layout_row_dynamic(gui, nk_window_get_content_region(gui).h - 72 - 12, 1);
	jg_waveview(gui, tmp, sz, &info);

	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
	
	// process about window
	process_about(gui);	
}


