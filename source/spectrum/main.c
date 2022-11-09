#include <jackutils.h>
#include <jwinutils.h>
#include <fftw3.h>

// JackUtils library usage example
// Spectrum visualizer using FFTW library

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#define PROGRAM_NAME "spectrum"
#define PROGRAM_VERSION 0.1
#define PROGRAM_USAGE "spectrum [-m MODE] [-w WINDOW] [-h] [-v]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!\n Keyboard controls : 1- change mode, 2 - change window, 0 - reset pos, 9 - reset width scale. +/- - increase/decrease width scale.\n LMB - move; Scroll - scale; :) enjoy"
#include <ju_args.h>

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

static void loop(ju_ctx_t* ctx, ju_win_t* w);

static int normalizer = 3; // best normalizer
static int window     = 0; // ddefault window = rectangle :D

static void argp (char c, const char* arg) {
	if (c == 'm') {
		normalizer = atoi(arg);
		if (normalizer > 3) normalizer = 3;
		if (normalizer < 0) normalizer = 0;
	} else {
		window = atoi(arg);
		if (window > 3) window = 3;
		if (window < 0) window = 0;
	}
}

int main(int argc, char** argv) {
	// parse arguments
	ja_parse(argc, argv, argp, "m:w:");
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
#define PI 3.1415

// Normalizers functions using macros magic :D
#define DEFNORM(name, code) \
static float name(float v, float m) {code;}
// unique filter definition 
#define FILTER(S, V) ({static float f[S] = {0}; \
for (int i = 0; i < S-1; i++) f[i] = f[i + 1]; float sum = 0;\
for (int i = 0; i < S-1; i++) sum += f[i];\
f[S-1] = (sum + V)/(float)S; f[S-1];})

DEFNORM(divsqrt, return v / sqrt(m)) // division on sqrt of length
DEFNORM(lognorm, return (log10((divsqrt(v, m)+0.07)*15)/2)-0.01) // log10 porn :D
DEFNORM(filtsqrt, return divsqrt(FILTER(5, v), m)) // same as divsqrt, but filtered
DEFNORM(filtnorm, return lognorm(FILTER(5, v), m)) // same as lognorm, but filtered (not bad)

// window functions using same magic :D
#define DEFWIN(name, code) static float name(float v, float s) {code;}
DEFWIN(winrect, return 1) // rectangle window
DEFWIN(wingaus, float a = (s-1)/2.0; float t = (v-a)/(0.5*a);
t *= t; return exp(-t/2)) // gausse
DEFWIN(winhamm, return 0.54 - 0.46*cos((2*PI*v)/(s-1))) // hamming
DEFWIN(winhann, return 0.5*(1-cos((2*PI*v)/(s-1)))) // Hann

// normalizers array
static bool switchnorm = false;
static bool switchwin  = false;

float (*normalizers[]) (float v, float m) = {
	divsqrt, lognorm, filtsqrt, filtnorm
};

struct {
	float (*win) (float v, float s);
	const char* name;
} windows[] = {
	{winrect, "rectangle"},
	{wingaus, "gausse"},
	{winhamm, "hamming"},
 	{winhann, "hann"}
};

// visual controls
static float scale = 1;
static float widthscale = 1;
static float X = 0, Y = 0;
static bool hold_started = false;
static w_xy_t oldmouse;

static void loop(ju_ctx_t* ctx, ju_win_t* w) {
	double dt = ju_draw_begin(w);
	w_wh_t ws = ju_win_size(w);

	// to prevent data racing
	ju_buff_lock(&buff);
	size_t sz = oldsize; // oldsize protected by buffer mutex :p
	float tmp[sz], freq[sz + 1];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	// apply window to signal
	for (int i = 0; i < sz; i++)
		tmp[i] *= windows[window].win(i, sz);

	// compute FFT and normalize
	fftwf_plan plan;	
	plan = fftwf_plan_r2r_1d(sz, tmp, freq, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);
	for (size_t i = 0; i < sz/2; i++) // optimisation : we don't show the half of FFT at all, so skip it :D
					freq[i] = normalizers[normalizer](ABS(freq[i]), sz);

	// input
	if (ju_win_mousekey(w, GLFW_MOUSE_BUTTON_1)) {
		if (!hold_started) {
			hold_started = true;
			oldmouse = ju_win_mouse(w);
		} else {
			w_xy_t m = ju_win_mouse(w);
			X += m.x - oldmouse.x;
			Y += m.y - oldmouse.y;
			oldmouse = m;
		}
	} else hold_started = false;

	if(ju_win_getkey(w, GLFW_KEY_1)) {
		if (!switchnorm) {
			normalizer += 1;
			if (normalizer > 3) normalizer = 0;
			switchnorm = true;
		}
	} else switchnorm = false;
	
	if(ju_win_getkey(w, GLFW_KEY_2)) {
		if (!switchwin) {
			window += 1;
			if (window > 3) window = 0;
			switchwin = true;
		}
	} else switchwin = false;

	if (ju_win_getkey(w, GLFW_KEY_MINUS)) widthscale -= 0.05;
	if (ju_win_getkey(w, GLFW_KEY_EQUAL)) widthscale += 0.05;
	if (ju_win_getkey(w, GLFW_KEY_9)) widthscale = 1;

	if (ju_win_getkey(w, GLFW_KEY_0)) { // reset position and scale
		X = 0;
		Y = 0;
		scale = 1;
	}
	scale += ju_win_scroll(w) * 0.05;

	// draw
	glPushMatrix();
	glTranslatef(X, Y, 0);
	glScalef(scale, scale, 1);
	glColor4f(0.2,0,0, 1);
	ju_draw_grid(25 * widthscale, 25, 0, 0, ws.w * widthscale, ws.h);
	glColor4f(1,1,1,1);
	// other half of spectre is an a mirror, so skip it :p
	ju_draw_samples(freq, sz/2, 0, ws.h - 2, ws.w * widthscale, -ws.h);
	glPopMatrix();
	ju_win_pool_events();
}
