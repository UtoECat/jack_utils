// JackUtils library Utility : Volumer
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define PROGRAM_NAME "volumer"
#define PROGRAM_VERSION 0.1
#define PROGRAM_USAGE "volumer [-a volume] [-h] [-v]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!\nChange volume of input data :)"
#include <ju_args.h>

#define IN_RANGE(V, L, M) (V < L ? L : (V > M ? M : V))
#define ABS(a) (a > 0.0 ? a : -(a))

static int   inport, outport;
static float volume = 1.0f;
static float limit  = 1.0f;

static void process(ju_ctx_t* ctx, size_t) {
	const size_t len = ju_length(ctx);
	const float* src = ju_port_read(ctx, inport);
	float tmp[len];

	memcpy(tmp, src, len * sizeof(float));
	for (size_t i = 0; i < len; i++) {
		tmp[i] *= volume;
		if (tmp[i] > limit ) tmp[i] =  limit;
		if (tmp[i] < -limit) tmp[i] = -limit;
	}

	ju_port_write(ctx, outport, tmp, len);
}

static void argp (char, const char* arg) {
	volume = atof(arg);
}

int main(int argc, char** argv) {
	// parse arguments
	ja_parse(argc, argv, argp, "a:");
	volume = IN_RANGE(volume, 0.0f, 10.0f);
	printf("volume = %f :) \n", volume);
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
