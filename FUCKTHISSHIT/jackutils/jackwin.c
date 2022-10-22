// Window Extension for jackutils library.
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

JWU_API void ju_draw_begin(ju_ctx_t* x, int proj) {
	struct ju_wh_s size = ju_draw_size(x);
	glViewport(0,0,size.w, size.h);

	glfwMakeContextCurrent((GLFWwindow*)ju_ctx_win_get(x));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (proj == ORTHO_PROJECTION) {
		glOrtho(0, size.w, size.h, 0, -100, 100);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

JWU_API void ju_draw_show(ju_ctx_t* x) {
	glfwSwapBuffers((GLFWwindow*)ju_ctx_win_get(x));
}

JWU_API void ju_draw_pool() {
	glfwPollEvents();
}

/*
 * Returns size of the framebuffer
 */
JWU_API struct ju_wh_s ju_draw_size(ju_ctx_t* x) {
	struct ju_wh_s d = {0, 0};
	glfwGetWindowSize((GLFWwindow*)ju_ctx_win_get(x), &d.w, &d.h);
	return d;
}

/***********************************************
 *
 * Drawing functions
 *
 ***********************************************
 */

/*
 * This function draws waveform.
 * WARNING! DO NOT USE BUFFERS FROM PROCESS CALLBACK - 
 * YOU 100% WILL GET SEGFAULT!
 *
 * Instead, copy buffer from port, using JACKBUFFER API
 * <jackbuffer.h> or memcpy to temp array + mutex!
 * 
 * @arg sample array
 * @arg samples count
 * @arg x
 * @arg y
 * @arg width
 */

JWU_API void ju_draw_samples(ju_sample_t* q, size_t s, float x, float y, float w) {
	glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < s; i++) {
			const float xv = i / ((float)s) * w;
			glVertex2f(x + xv, y + q[i]);
		}
	glEnd();
}

