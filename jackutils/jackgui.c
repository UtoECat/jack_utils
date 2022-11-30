// GUI Extension for jackutils library.
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#define NK_IMPLEMENTATION
#include <jackgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

static void glfwinit__() {
	static bool inited = false;
	if (inited) return;
	if (glfwInit()) inited = true;
	atexit(glfwTerminate);
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

#define NK_GLFW_GL2_IMPLEMENTATION
#include <ext/nuklear_glfw_gl2.h>

/*
 * Initializes GUI context.
 * @ret NULL in case of error
 */
JG_API jg_ctx_t* jg_init(const char* t, int w, int h) {
	GLFWwindow* window;
	jg_ctx_t* ctx;
	struct nk_font_atlas* atlas;

	glfwSetErrorCallback(error_callback);
	glfwinit__();

	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	window = glfwCreateWindow(w, h, t, NULL, NULL);
	if (!window) {
		return NULL;
	}

	glfwMakeContextCurrent(window);
	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&atlas);
	nk_glfw3_font_stash_end();
}

/*
 * Destroys GUI context
 */
JG_API void      jg_uninit(jg_ctx_t* ctx) {
	nk_glfw3_shutdown();
}

/*
 * Starts GUI objects enumeration.
 * You must call jg_end() when you done
 *
 * @ret 1 on sucess. Else you must not enumerate anything!
 */
JG_API int  jg_begin(jg_ctx_t* ctx) {
	int width, height;
	struct nk_rect area;
	
	glfwGetWindowSize(glfw.win, &width, &height);

	area = nk_rect(0.f, 0.f, (float) width, (float) height);
	nk_window_set_bounds(ctx, "", area);

	glClear(GL_COLOR_BUFFER_BIT);
	nk_glfw3_new_frame();
	return nk_begin(ctx, "", area, 0);
}

JG_API void jg_end(jg_ctx_t* ctx) {
	nk_end(ctx);
	nk_glfw3_render(NK_ANTI_ALIASING_ON);
	glfwSwapBuffers(glfw.win);
	glfwWaitEventsTimeout(1.0);
}
