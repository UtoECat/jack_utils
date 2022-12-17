// JackUtils library Utility : Waveform (Visualizer)
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <jackgui.h>
#include <stdlib.h>
#include <stdio.h>
#include "about.h"

static int port = 0;
static ju_buff_t buff;
static float volume = 1.0f;

static void process(ju_ctx_t* ctx, size_t len) {
	ju_buff_lock(&buff);
	ju_buff_resize(&buff, len*sizeof(float));
	ju_buff_append(&buff, ju_port_read(ctx, port), len*sizeof(float));
	ju_buff_unlock(&buff);
}

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui);
struct jg_bar_item bar_items[2];

int main(int, char** argv) {
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1); // init jackutils
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 640, 480);
	bar_items[0] = jg_float_item("volume", &volume, 0.01, 0.01, 2.0);
	bar_items[1] = jg_null_item();
	// init buffer
	ju_buff_init(&buff, ju_length(ctx) * sizeof(float));
	ju_start(ctx, process); // start jack
	while (ju_is_online(ctx, 0)) {
		if (jg_begin(gui)) {
			jg_ju_topbar(gui, ctx, bar_items);
			loop(ctx, gui);
		}
		jg_end(gui, 1);
		jg_sync_visibility(gui, ctx);
	}
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t sz = ju_buff_size(&buff) / sizeof(float);
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	struct waveinfo info = {-1 / volume, 1 / volume, 0, 0, 1, 1};
	nk_layout_row_dynamic(gui, nk_window_get_content_region(gui).h - 72 - 12, 1);
	jg_waveview(gui, tmp, sz, &info);

	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
}


