// JackUtils library Utility : Dynamic Volume Normalizer
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define PROGRAM_NAME "normvol"
#define PROGRAM_VERSION 0.1
#define PROGRAM_USAGE "normvol [-k fade coefficient] [-h] [-v]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!\nChange volume of input data :)"
#include <ju_args.h>

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

static void argp (char, const char* arg) {
	k = atof(arg);
}

int main(int argc, char** argv) {
	// parse arguments
	ja_parse(argc, argv, argp, "k:");
	k = IN_RANGE(k, 0.001f, 0.5f);
	printf("coefficient = %f :) \n", k);
	// create context
	ju_ctx_t* ctx = ju_ctx_init(PROGRAM_NAME, NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	outport = ju_port_open(ctx, "output", JU_OUTPUT, 0);
	inport  = ju_port_open(ctx, "input ", JU_INPUT , 0);
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies xD
	while (ju_is_online(ctx, 1000)) {};
	// free context
	// context will be already stopped in case of server disconnect
	ju_ctx_uninit(ctx);
}
