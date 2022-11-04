// JackUtils library Utility : Signal Generator
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>

static int port;
static float freq = 1.0f;
static float rate = 44100.f;

#define PI2 (3.1415 * 2)

static size_t generator(void*, float* dst, size_t) {
	static double cnt = 0;
	dst[0] = cos(cnt) * 0.5;
	cnt += freq / rate;
	freq += 0.04;
	if ((freq / rate) > 3.1415) freq = 1.0f;
	if (cnt > PI2) cnt = cnt - PI2;
	return 1;
}

void process(ju_ctx_t* ctx, size_t) {
	rate = ju_samplerate(ctx);
	ju_port_write_stream(ctx, port, generator, NULL);
}

int main(void) {
	// create context
	ju_ctx_t* ctx = ju_ctx_init("generator", NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	port = ju_port_open(ctx, "output", JU_OUTPUT, JackPortIsTerminal);
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies xD
	ju_is_online(ctx, -1);
	// free context
	// context will be already stopped in case of server disconnect
	ju_ctx_uninit(ctx);
}
