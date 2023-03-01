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
static inline float logn(float v, float n) {return log10(v)/log10(n);}
static inline float logone(float v, float n) {return logn(v*n + 1, n)/logn(n + 1, n);}

// log index
static inline size_t logindex(size_t i, size_t s, float n) {
	float f = 1.0 - (float)i / (float)s;
	float r = 1.0 - logone(f, n); // don't ask :p
	if (r >= 1) return s-1;
	return (size_t)(r * s);
}

#include <fftw3.h>
#define ABS(x) ((x) >= 0.0 ? (x) : -(x))
#define PI 3.1415

// spectrum-specific stuff
static const size_t _power_table[13] = {
	16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192,
	16384, 32768, 65536
};

static const char* _text_power_table[13] = {
	"16", "32", "64", "128", "256", "512", "1024", "2048", 
	"4096", "8192", "16384", "32768", "65536"
};
#define MAX_POWER 12


// windows (signal filters to make peaks more visible)
static float _wrect(float, float) {return 1;}
static float _wgausse(float v, float s) {
	float a = (s-1)/2.0;
	float t = (v-a)/(0.5*a);
	t *= t;
	return exp(-t/2);
}
static inline float _w_int_norm(float v, float s) {return (2*PI*v)/(s-1);}
static float _whamming(float v, float s) {return 0.54 - 0.46*cos(_w_int_norm(v, s));}
static float _whann   (float v, float s) {return 0.5*(1-cos(_w_int_norm(v, s))); }

static float (*spectrum_windows[]) (float, float) = {
	_wrect, _wgausse, _whamming, _whann
};

static int port = 0;
static ju_buff_t buff;
static size_t sp_buff_power = 9;
static size_t sp_window = 1;
static float  logx = 10.0f;
static float  logy = 10.0f;
static float  skipk = 1.0f;
static float 	lenk = 9;

static char curr_power_text[8] = "";

static size_t get_size(void) {return _power_table[sp_buff_power];}

static void process(ju_ctx_t* ctx, size_t len) {
	ju_buff_lock(&buff);
	// oh no
	sp_buff_power = lenk;
	if (sp_buff_power > MAX_POWER) sp_buff_power = MAX_POWER;
	//
	size_t portblen = len;
	size_t newblen = get_size();
	newblen = portblen < newblen ? newblen : portblen; // MAX

	size_t k = skipk;
	const float* src = ju_port_read(ctx, port);
	float tmp[len / k + 1]; 

	for (int i = 0; i < len; i += k) {
		tmp[i/k] = src[i];
	}
	if (ju_buff_size(&buff) < newblen * sizeof(float))
		ju_buff_resize(&buff, newblen * sizeof(float));
	ju_buff_append(&buff, tmp, len/k*sizeof(float));
	ju_buff_unlock(&buff);
}

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui);
struct jg_bar_item bar_items[6];

int main(int, char** argv) {
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1); // init jackutils
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 640, 480);
	bar_items[0] = jg_float_item("log(x)", &logx, 1, 1, 100);
	bar_items[1] = jg_float_item("log(y)", &logy, 1, 1, 100);
	bar_items[2] = jg_float_item("skipk", &skipk, 1, 1, 6);
	bar_items[3] = jg_float_item("length", &lenk, 1, 1, MAX_POWER);
	bar_items[4] = jg_text_item("result", curr_power_text, 80);
	bar_items[5] = jg_null_item();
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
		ju_pool_events(ctx);
	}
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

#define MIN(a, b) (a < b ? a : b)

static void loop(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	memcpy(curr_power_text, _text_power_table[sp_buff_power], strlen(_text_power_table[sp_buff_power]) + 1);
	size_t sz = get_size();
	float tmp[sz], fft[sz];
	// if has more data, than needed
	if (sz * sizeof(float) < ju_buff_size(&buff) &&
			ju_buff_used(&buff) > sz * 2 * sizeof(float)) {
		ju_buff_remove(&buff, tmp, sz * sizeof(float));
	} else { // keep data in buffer
		memcpy(tmp, ju_buff_data(&buff), MIN(sizeof(float) * sz, ju_buff_size(&buff)));
	}
	ju_buff_unlock(&buff);

	// apply window and limits
	for (int i = 0; i < (int)sz; i++) {
		tmp[i] = tmp[i] * spectrum_windows[sp_window](i, sz);
		if (ABS(tmp[i]) > 1.0) { // cutoff
			if (tmp[i] > 0.0) tmp[i] = 1.0;
			else tmp[i] = -1.0;
		}
	}

	fftwf_plan plan;
	plan = fftwf_plan_r2r_1d(sz, tmp, fft, FFTW_DHT, FFTW_ESTIMATE);
	fftwf_execute(plan);
	fftwf_destroy_plan(plan);

	// normalize spectrum
	size_t oldindx = 0;
	for (size_t i = 0; i < sz/2; i++) {
		// normalize by x
		size_t indx = i;
		if (logx > 1.0f) indx = logindex(i, sz/2, logx);
		// get
		float value = 0, norm = 0;
		// find biggest value between old and new index
		for (size_t j = oldindx; j <= indx; j++) {
			const float preval = ABS(fft[j]) / (float)sz * PI * 2;
			if (preval > value) value = preval;
		}
		oldindx = indx;
		tmp[i] = value;
		// normalize by y
		if (logy > 1.0f) tmp[i] = logone(tmp[i], logy);
	}

	// draw
	struct waveinfo info = {0, 1, 0, 0, 1, 1};
	nk_layout_row_dynamic(gui, nk_window_get_content_region(gui).h - 72 - 12, 1);
	jg_waveview(gui, tmp, sz/2, &info);

	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
}


