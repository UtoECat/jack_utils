#include <jackutils.h>
#include <jwinutils.h>
#include <fftw3.h>

// JackUtils library usage example
// Spectrum visualizer using FFTW library

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// TODO: make it dynamical
#define SPECTRUM_QUALITY_K 6

static int port = -1;
static ju_buff_t buff;
static jack_nframes_t oldsize = 0;

static void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x) * SPECTRUM_QUALITY_K) {
		oldsize = ju_length(x) * SPECTRUM_QUALITY_K;
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
	ju_ctx_t* ctx = ju_ctx_init("spectrum", NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// open window and cache buffer
	ju_win_t* w = ju_win_open(640, 480);
	ju_win_title(w, ju_get_name(ctx));
	oldsize = ju_length(ctx) * SPECTRUM_QUALITY_K;
	ju_buff_init(&buff, oldsize * sizeof(float));
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies or window will be closed xD
	while (!ju_win_should_close(w) && ju_is_online(ctx, 0)) {
		loop(ctx, w);
	}
	// free anyithing
	ju_win_close(w);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

#define ABS(x) ((x) >= 0.0 ? (x) : -(x))

void loop(ju_ctx_t* ctx, ju_win_t* w) {
	double dt = ju_draw_begin(w);
	w_wh_t ws = ju_win_size(w);

	// to prevent data racing
	ju_buff_lock(&buff);
	size_t sz = oldsize; // oldsize protected by buffer mutex :p
	float tmp[sz], freq[sz + 1];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	// compute FFT and normalize
	fftwf_plan plan;	
	plan = fftwf_plan_r2r_1d(sz, tmp, freq, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
	for (size_t i = 0; i < sz; i++)
					freq[i] = ABS(freq[i] / sqrt(sz));

	// other half of spectre is an a mirror :p
	ju_draw_samples(freq, sz/2, 0, ws.h - 2, ws.w, -ws.h);
	ju_win_pool_events();
}
