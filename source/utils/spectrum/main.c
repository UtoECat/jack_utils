// JackUtils library Utility : Waveform (Visualizer)
/* 
 * Copyright (C) UtoECat 2022. All rights Reserved!

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jackutils.h>
#include <jackgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "about.h"
#include <fftw3.h>
#define ABS(x) ((x) >= 0.0 ? (x) : -(x))
#define PI 3.1415

static float wgausse(float v, float s) {
	float a = (s-1)/2.0;
	float t = (v-a)/(0.5*a);
	t *= t;
	return exp(-t/2);
}
static inline float logn(float v, float n) {return log10(v)/log10(n);}
static inline float logone(float v, float n) {
	return n > 1 ? logn(v*n + 1, n)/logn(n + 1, n) : n;
}

static int port = 0;
static ju_buff_t buff;

// variables
static float  logx = 10.0f;
static float  logy = 10.0f;
static float  skipk = 1.0f;


static inline float calcval(float v, float n) {
	float sign = 1;
	if (v < 0) {sign = -1; v = -v;}
	return logone(v/(float)n *3.1415*2, logy) * sign;
}

static void process(ju_ctx_t* ctx, size_t len) {
	ju_buff_lock(&buff);
	if (ju_buff_size(&buff) < len * sizeof(float))
		ju_buff_resize(&buff, len * sizeof(float));

	ju_vars_lock();
	size_t k = (size_t)skipk;
	ju_vars_unlock();

	const float* src = ju_port_read(ctx, port);
	float tmp[len/k+1]; 

	for (size_t i = 0; i < len; i += k) {
		// apply window right here :)
		tmp[i/k] = src[i] * wgausse(i, len);
	}

	ju_buff_append(&buff, tmp, len/k*sizeof(float));
	ju_buff_unlock(&buff);
}

static void draw(ju_ctx_t* ctx, jg_ctx_t* gui);
static ju_var_t variables[4];

int main(int, char** argv) {
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1); // init jackutils
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 640, 480);
	variables[0] = ju_var_float_rw("log(x)", &logx, 1, 100);
	variables[1] = ju_var_float_rw("log(y)", &logy, 1, 100);
	variables[2] = ju_var_float_rw("skip", &skipk, 1, 6);
	variables[3] = ju_var_nil();
	// init buffer
	ju_buff_init(&buff, ju_length(ctx) * sizeof(float));

	// start audio processing
	ju_start(ctx, process);
	while (ju_is_online(ctx, 0)) {
		if (jg_begin(gui, 0)) {
			draw(ctx, gui);
			jg_jackutils_topbar(gui, ctx);
			ju_vars_draw(gui, variables, 40);
		}
		jg_end(gui, 1);
		jg_sync_visibility(gui, ctx);
		ju_pool_events(ctx);
	}
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

#define MIN(a, b) (a < b ? a : b)

static void draw(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t size = ju_buff_size(&buff);
	size_t len = size/sizeof(float);
	float tmp[len], fft[len];
	memcpy(tmp, ju_buff_data(&buff), size);
	memset(fft, 0, size);
	ju_buff_unlock(&buff);

	fftwf_plan plan;
	plan = fftwf_plan_r2r_1d(len, tmp, fft, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);

	// apply logy and other stuff...
	len = len/2;
	for (size_t i = 0; i < len; i++) {
		tmp[i] = calcval(fft[i], len*2);
	} 
	
	glColor3f(0.5, 1.0, 0.8);
	glBegin(GL_LINE_STRIP);
		size_t hl = len;
		for (size_t i = 0; i < hl; i++) {
			glVertex2f((logone(i/(float)hl, logx)*2)-1.0f, tmp[i]);
		}
	glEnd();
	
	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
}


