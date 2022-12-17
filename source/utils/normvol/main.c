// JackUtils library Utility : Dynamic Volume Normalizer
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <jackgui.h>
#include <stdlib.h>
#include <stdio.h>
#include "about.h"

#define IN_RANGE(V, L, M) (V < L ? L : (V > M ? M : V))
#define ABS(a) (a > 0.0 ? a : -(a))

static int   inport, outport;
static float volume = 1.0f;
static float k      = 0.01f;
static float limit  = 1.0f;

static void process(ju_ctx_t* ctx, size_t) {
	const size_t len = ju_length(ctx);
	const float* src = ju_port_read(ctx, inport);
	float tmp[len];
	float maxv = 0.0f;

	memcpy(tmp, src, len * sizeof(float));
	for (size_t i = 0; i < len; i++) {
		tmp[i] *= volume;
		const float absv = ABS(tmp[i]);
		if (absv > limit ) {
			if (tmp[i] > 0.0f) tmp[i] = limit;
			else tmp[i] = -limit;
			volume /= absv; 
		} else {
			maxv += absv;
		}
	}
	maxv /= (float) len;
	volume += (1.0 - maxv) * k;
	ju_port_write(ctx, outport, tmp, len);
}

struct jg_bar_item bar_items[4];
char text_buffer[32] = "";

int main(int, char** argv) {
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1); // init jackutils
	inport = ju_port_open(ctx, "input", JU_INPUT, 0);
	outport = ju_port_open(ctx, "output", JU_OUTPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 320, 80);
	bar_items[0] = jg_float_item("volume", &limit, 0.01, 0.01, 2.0);
	bar_items[1] = jg_float_item("coeff.", &k, 0.001, 0.01, 0.1);
	bar_items[2] = jg_text_item("result", text_buffer, 80);
	bar_items[3] = jg_null_item();
	ju_start(ctx, process); // start jack
	while (ju_is_online(ctx, 0)) {
		if (jg_begin(gui)) {
			jg_ju_topbar(gui, ctx, bar_items);
			//loop(ctx, gui);
			snprintf(text_buffer, 25, "%.3f", volume);
		}
		jg_end(gui, 1);
		jg_sync_visibility(gui, ctx);
		ju_pool_events(ctx);
	}
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
}
