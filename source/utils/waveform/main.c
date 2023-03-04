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
#include <ext/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include "about.h"

static int port = 0;
static ju_buff_t buff;

// variable is not used in process function LOL
static float var_volume = 1.0f;
static ju_var_t variables[2];

static void process(ju_ctx_t* ctx, size_t len) {
	ju_buff_lock(&buff);
	ju_buff_check_size(&buff, len*sizeof(float));
	ju_buff_append(&buff, ju_port_read(ctx, port), len*sizeof(float));
	ju_buff_unlock(&buff);
}

static void draw(ju_ctx_t* ctx, jg_ctx_t* gui);

int main(int, char** argv) {
	// init jackutils
	ju_ctx_t* ctx = ju_ctx_init(program_info.name, argv[0], 1);
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// init gui
	jg_ctx_t* gui = jg_init(ju_get_name(ctx), 640, 480);
	// init variables
	variables[1] = ju_var_nil();
	variables[0] = ju_var_float_rw("volume", &var_volume, 0.01, 1.5);
	// init buffer
	ju_buff_init(&buff, ju_length(ctx) * sizeof(float));
	// start audio processing
	ju_start(ctx, process); 

	while (ju_is_online(ctx, 0)) {
		if (jg_begin(gui, 0)) {
			// custom openGL drawning at first - for future compability!
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

static void draw(ju_ctx_t* ctx, jg_ctx_t* gui) {
	// get data
	ju_buff_lock(&buff);
	size_t sz = ju_buff_size(&buff) / sizeof(float);
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);

	glColor3f(0.5, 1.0, 0.8);
	glLineWidth(2.0f);

	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < sz; i++) {
		glVertex2f((i/(float)sz * 2.0f) - 1.0f, tmp[i] * var_volume);
	};
	glEnd();

	// request screen refresh	
	if (ju_port_connected(ctx, port))
		jg_request_redraw(gui); // if we have any data :)
}


