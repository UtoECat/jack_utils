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
	double scroll; bool updscroll;
	int width, height;
};

static void glfwinit__() {
	static bool inited = false;
	if (inited) return;
	if (glfwInit()) inited = true;
	atexit(glfwTerminate);
}

#define MAX(A, B) ((A) > (B) ? (A) : (B))

static void scrollcb (GLFWwindow* w, double x, double y) {
	ju_win_t* o = glfwGetWindowUserPointer(w);
	o->scroll = y;
	o->updscroll = true;
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
	o->scroll = 0;
	o->updscroll = false;
	glfwSetWindowUserPointer(o->win, o);
	glfwSetScrollCallback(o->win, scrollcb);
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
	w_wh_t ret = {w->width, w->height};
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

JWU_API float ju_win_scroll(ju_win_t* w) {
	return w->scroll;
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
	glfwGetWindowSize(w->win, &size.w, &size.h);
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

	if (w->updscroll == true) {
		w->updscroll = false;
	} else if (w->scroll != 0) {
		w->scroll = 0;
	}
	return dt;
}

/*
 * Setups window viewport with stretch checks and calculations.
 */
JWU_API void ju_draw_view(ju_win_t* w, w_rect_t r) {
	w_wh_t size = {1, 1};
	glfwGetWindowSize(w->win, &size.w, &size.h);
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

JWU_API int       ju_win_getkey(ju_win_t* w, int key) {
	return glfwGetKey(w->win, key);
}
JWU_API int       ju_win_mousekey(ju_win_t* w, int key) {
	return glfwGetMouseButton(w->win, key);
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

JWU_API void ju_draw_grid(float kw, float kh, float sx, float sy, float w, float h) {
	for (float x = sx; x < sx + w; x += kw)
		for (float y = sy; y < sy + h; y += kh) {
			glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x + kw, y);
			glVertex2f(x + kw, y + kh);
			glVertex2f(x, y + kh);
			glEnd();
		}
}

// clockwise
static void draw_rect_opt(unsigned char flag) { // [0..1]
	glBegin(GL_LINES);
	if (flag & 1  ) {glVertex2f(0.0, 0.0); glVertex2f(1.0, 0.0);};
	if (flag & 2  ) {glVertex2f(1.0, 0.0); glVertex2f(1.0, 0.5);};
	if (flag & 4  ) {glVertex2f(1.0, 0.5); glVertex2f(1.0, 1.0);};
	if (flag & 8  ) {glVertex2f(1.0, 1.0); glVertex2f(0.0, 1.0);};
	if (flag & 16 ) {glVertex2f(0.0, 1.0); glVertex2f(0.0, 0.5);};
	if (flag & 32 ) {glVertex2f(0.0, 0.5); glVertex2f(0.0, 0.0);};
	if (flag & 64 ) {glVertex2f(0.0, 0.5); glVertex2f(1.0, 0.5);};
	glEnd();
}

static unsigned char digarr[] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111, // 9
	0b01000000, // -
};

JWU_API void ju_draw_digit(char d, float x, float y, float w, float h) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(w, h, 1);
	if (d >= 0 && d <= 10) draw_rect_opt(digarr[d]);
	glPopMatrix();
}

JWU_API void ju_draw_char(char d, float x, float y, float w, float h) {
	if (d >= '0' && d <= '9') ju_draw_digit(digarr[d - '0'], x,y,w,h);
	else if (d == '-') ju_draw_digit(digarr[10],x,y,w,h);
}

static void draw_int(int i, float *x, float y, float s) {
	const float ws = s - s/3.0;
	if (i < 0) {
		ju_draw_digit(10, *x, y, ws, ws*2); *x += s;
		i = -i;
	}
	if (i == 0) {
		ju_draw_digit(0, *x, y, ws, ws*2); *x += s;
		return;
	} else {
		if (i/10) draw_int(i/10, x, y, s);
		ju_draw_digit(i%10, *x, y, ws, ws*2); *x += s;
		return;
	}
}

JWU_API void ju_draw_int(int i, float x, float y, float s) {
	return draw_int(i, &x, y, s);
}
