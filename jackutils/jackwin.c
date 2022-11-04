// Window Extension for jackutils library.
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#include <jwinutils.h>
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

struct ju_win_s {
	GLFWwindow* win;
	double old_time;
	int  stretch;
	double mdx, mdy, mkx, mky;
	int width, height;
};

static void glfwinit__() {
	static bool inited = false;
	if (inited) return;
	if (glfwInit()) inited = true;
	atexit(glfwTerminate);
}

JWU_API ju_win_t* ju_win_open(int a, int b) {
	glfwinit__();
	ju_win_t* o = malloc(sizeof(ju_win_t));
	assert(o != NULL);
	o->win = glfwCreateWindow(a, b, "unnamed", NULL, NULL);
	o->old_time = glfwGetTime();
	o->stretch = 0;
	o->mdx = 0; o->mdy = 0; o->mkx = 1; o->mky = 1;
	o->width = a; o->height = b;
	return o;
}
JWU_API void      ju_win_close(ju_win_t* w) {
	glfwDestroyWindow(w->win);
	free(w);
}

JWU_API void      ju_win_show(ju_win_t* w){
	glfwShowWindow(w->win);
}
JWU_API void      ju_win_hide(ju_win_t* w) {
	glfwHideWindow(w->win);
}
JWU_API void      ju_win_title(ju_win_t* w, ju_cstr_t n) {
	glfwSetWindowTitle(w->win, n);
}

JWU_API w_wh_t    ju_win_size(ju_win_t* w) {
	w_wh_t ret = {0, 0};
	glfwGetWindowSize(w->win, &ret.w, &ret.h);
	return ret;
}

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

JWU_API w_xy_t    ju_win_mouse(ju_win_t* w) {
	double mx = 0, my = 0;
	glfwGetCursorPos(w->win, &mx, &my);
	mx *= w->mkx; my *= w->mky;
	mx += w->mdx; my += w->mdy;
	mx = MIN(MAX(mx, 0), w->width);
	my = MIN(MAX(my, 0), w->height);
	return (w_xy_t){mx, my};
}

JWU_API void      ju_win_resize(ju_win_t* w, w_wh_t s) {
	glfwSetWindowSize(w->win, s.w, s.h);
}
JWU_API void      ju_win_stretch(ju_win_t* w, bool v) {
	w->stretch = v;
}

JWU_API double ju_draw_begin(ju_win_t* w) {
	glfwMakeContextCurrent(w->win);
	glfwSwapBuffers(w->win);
	double dt = glfwGetTime() - w->old_time;
	w->old_time += dt;
	// stretch calc...
	w_wh_t size = ju_win_size(w);
	if (!w->stretch) {
		w->width = size.w;
		w->height = size.h;
	};
	// calc new mouse and coefficients
	double scale = MIN(size.w/(double)w->width, size.h/(double)w->height); 
	double neww = w->width * scale;
	double newh = w->height* scale;
	w->mdx = -(size.w - neww) * 0.5f;
	w->mdy = -(size.h - newh) * 0.5f;
	w->mkx = 1/scale; w->mky = 1/scale;

	ju_draw_view(w, (w_rect_t){0, 0, w->width, w->height});
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w->width, w->height, 0, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return dt;
}

/*
 * Setups window viewport with stretch checks and calculations.
 */
JWU_API void ju_draw_view(ju_win_t* w, w_rect_t r) {
	w_wh_t size = ju_win_size(w);
	double scale = MIN(size.w/(double)w->width, size.h/(double)w->height); 
	double neww = w->width * scale;
	double newh = w->height* scale;
	glViewport((size.w - neww) * 0.5 + r.x * scale,
		(size.h - newh) * 0.5 + r.y * scale,
		r.w * scale, r.h * scale);
}

JWU_API void ju_win_pool_events(void) {
	glfwPollEvents();
}

JWU_API bool      ju_win_should_close(ju_win_t* w) {
	return glfwWindowShouldClose(w->win);
}

JWU_API void ju_draw_samples(ju_sample_t* arr, size_t l, float x, float y, float w, float k) {
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < l; i++) {
		glVertex2f(x + (i / (double) l) * w, y + arr[i] * k);
	}
	glEnd();
}


