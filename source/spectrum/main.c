// JackUtils library Utility : Spectrum (Visualizer)
// FFT required as dependency!
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>
#include <jackgui.h>

#include <stdlib.h>
#include <stdio.h>
#define PROGRAM_NAME "spectrum"
#define PROGRAM_VERSION 0.2
#define PROGRAM_USAGE "spectrum [-h] [-v]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!"
#include <ju_args.h>
#include <about.h>

static size_t buff_mul = 6;
static int port;
static ju_buff_t buff;
static jack_nframes_t oldsize = 0;

static void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x) * buff_mul) {
		oldsize = ju_length(x) * buff_mul;
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
	ju_ctx_t* ctx = ju_ctx_init(PROGRAM_NAME, NULL);
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

#include <fftw3.h>
#define ABS(x) ((x) >= 0.0 ? (x) : -(x))
#define PI 3.1415
#include <spectrum_bits.h>
static int   window  = 1 ;
static const char* window_names[] = {
	"rectangle",
	"gausse",
	"hann",
	"hann???",
	NULL
};
static float logvalx = 10;
static float logvaly = 10;

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t sz = oldsize;
	float tmp[sz], freq[sz+1];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	// apply window to signal
	for (int i = 0; i < (int)sz; i++) {
		tmp[i] *= windows[window](i, sz);
		if (ABS(tmp[i]) > 1.0) { // cutoff
			if (tmp[i] > 0.0f) tmp[i] = 1.0;
			else tmp[i] = -1.0f;
		}
	}

	// compute FFT
	fftwf_plan plan;	
	plan = fftwf_plan_r2r_1d(sz, tmp, freq, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
	// normalize spectrum
	for (size_t i = 0; i < sz/2; i++)
		freq[i] = ABS(freq[i]) / (float)sz * PI;

	// final normalizing
	for (size_t i = 0; i < sz/2; i++) {
		size_t indx = i;
		if (logvalx > 1) indx = _logindex(i, sz, logvalx);
		if (logvaly > 1)
			tmp[i] = logone(freq[indx], logvaly);
		else
			tmp[i] = freq[indx];
	}

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
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 80);
			nk_layout_row_template_end(gui);
			
			// first layer
			nk_label(gui, "(Jackutils) : spectrum visualizer", NK_TEXT_CENTERED);
			// hehe
			static float val = 0;
			nk_combobox(gui, window_names, 4, &window, nk_widget_height(gui), nk_vec2(nk_widget_width(gui), 300));
			val = logvalx;
			if (jg_whell_float(gui, &val, 1, 0.1, 20)) {
				logvalx = val;
			}
			val = logvaly;
			if (jg_whell_float(gui, &val, 1, 0.1, 20)) {
				logvaly = val;
			}
			// end of wheels
			if (nk_button_label(gui, "About")) {
				show_about();
			};
			// second layout
			nk_layout_row_template_begin(gui, 10);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 40);
			nk_layout_row_template_push_static(gui, 80);
			nk_layout_row_template_end(gui);
			// second layer
			nk_labelf(gui, NK_TEXT_CENTERED, "Sample array length : %li", oldsize);
			nk_label(gui, "Window", NK_TEXT_CENTERED);
			nk_label(gui, "logN(x)", NK_TEXT_CENTERED);
			nk_label(gui, "logN(y)", NK_TEXT_CENTERED);
			nk_labelf(gui, NK_TEXT_CENTERED, "ver %0.1f", PROGRAM_VERSION);
		nk_group_end(gui);
	}
	nk_layout_row_end(gui);

	// bottom layout (spectrum)
	struct waveinfo info = {0, 1, 0, 0, 1, 1};
	nk_layout_row_dynamic(gui, nk_window_get_content_region(gui).h - 72 - 12, 1);
	jg_waveview(gui, tmp, sz/2, &info);

	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
	
	// process about window
	process_about(gui);	
}


