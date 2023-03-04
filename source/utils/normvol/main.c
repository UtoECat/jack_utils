// JackUtils library Utility : Dynamic Volume Normalizer
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
#include "about.h"

#define IN_RANGE(V, L, M) (V < L ? L : (V > M ? M : V))
#define ABS(a) (a > 0.0 ? a : -(a))

// used in process function only!
static int   inport, outport;
static float volume = 1.0f;

// variables
static float var_k      = 0.01f;
static float var_limit  = 1.0f;
static ju_var_t variables[3];

static void process(ju_ctx_t* ctx, size_t) {
	
	// read variables
	ju_vars_lock();
	float k     = var_k;
	float limit = var_limit;
	ju_vars_unlock();

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
	if (volume < 0) volume = 0.001;
	ju_port_write(ctx, outport, tmp, len);
}

int main(int, char** argv) {
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1); // init jackutils
	inport  = ju_port_open(ctx, "input", JU_INPUT, 0);
	outport = ju_port_open(ctx, "output", JU_OUTPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 480, 170);
	// set variables
	variables[2] = ju_var_nil();
	variables[1] = ju_var_float_rw("volume", &var_limit, 0.85, 1.5);
	variables[0] = ju_var_float_rw("coeff.", &var_k, 0.005, 0.09);

	// start audio processing
	ju_start(ctx, process); 

	while (ju_is_online(ctx, 0)) {
		if (jg_begin(gui, 0)) {
			jg_jackutils_topbar(gui, ctx);
			ju_vars_draw(gui, variables, 40);
		}
		jg_end(gui, 1);
		jg_sync_visibility(gui, ctx);
		ju_pool_events(ctx);
	}
	jg_uninit(gui);
	ju_ctx_uninit(ctx);
}
