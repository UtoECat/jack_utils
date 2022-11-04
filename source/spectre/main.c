#include <jackutils.h>
#include <jwinutils.h>
#include <fftw3.h>

// JackUtils library usage example
// Spectrum visualizer using FFTW library

#include <stdlib.h>
#include <stdio.h>

int ports[3] = {0};

ju_buff_t buff;
jack_nframes_t oldsize = 0;

void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x)) {
		oldsize = ju_length(x);
		fprintf(stderr, "buffer size changed to %i\n", oldsize);
		ju_buff_resize(&buff, oldsize * sizeof(float));
	}
}

// jack process callback
void process(ju_ctx_t* ctx, size_t len) {
	// output < input
	ju_port_write(ctx, ports[1], ju_port_read(ctx, ports[0]), len);
	// cache to buffer (lock to prevent data racing)
	ju_buff_lock(&buff);
	check_buffer(ctx);
	ju_buff_append(&buff, ju_port_read(ctx, ports[0]), len*sizeof(float));
	ju_buff_unlock(&buff);
}


void loop(ju_ctx_t* ctx, ju_win_t* w);

int main(void) {
	// create context
	ju_ctx_t* ctx = ju_ctx_init("waveform", NULL);
	// debug info
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	ports[0] = ju_port_open(ctx, "input", JU_INPUT, 0),
	ports[1] = ju_port_open(ctx, "output", JU_OUTPUT, 0),
	// independed generator is terminal :p
	ports[2] = ju_port_open(ctx, "generator", JU_OUTPUT, JackPortIsTerminal);
	// open window, FFTWPlan and cache buffers
	ju_win_t* w = ju_win_open(640, 480);
	oldsize = ju_length(ctx);
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

void loop(ju_ctx_t* ctx, ju_win_t* w) {
	double dt = ju_draw_begin(w);

	w_wh_t ws = ju_win_size(w);
	size_t sz = oldsize;
	float tmp[sz], freq[sz];

	// to prevent data racing
	ju_buff_lock(&buff);
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	// compute FFT and normalize TODO replace FFTW_DHT with something more good
	fftwf_plan plan;	
	plan = fftwf_plan_r2r_1d(sz, tmp, freq, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);

	ju_draw_samples(freq, sz, 0, 0, ws.w, ws.h/2);
	ju_win_pool_events();
}
