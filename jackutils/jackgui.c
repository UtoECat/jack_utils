// GUI Extension for jackutils library.
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#define NK_IMPLEMENTATION
#include <jackgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
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

#define NK_GLFW_GL3_IMPLEMENTATION
#include <ext/nuklear_glfw_gl3.h>

/*
 * Initializes GUI context.
 * @ret NULL in case of error
 */

static GLFWwindow* window;

JG_API jg_ctx_t* jg_init(const char* t, int w, int h) {
	jg_ctx_t* ctx;

	glfwSetErrorCallback(error_callback);
	glfwinit__();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	window = glfwCreateWindow(w, h, t, NULL, NULL);
	assert(window != NULL && "can't create glfw window!");
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);

	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	
	struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
	nk_glfw3_font_stash_end();
	/*nk_style_set_font(ctx, &droid->handle);*/
	return ctx;
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
	
	glfwGetWindowSize(window, &width, &height);
	area = nk_rect(0.f, 0.f, (float) width, (float) height);

	nk_glfw3_new_frame();
	return nk_begin(ctx, "", area, 0);
}

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

JG_API void jg_end(jg_ctx_t* ctx) {
	nk_end(ctx);
	
	int width, height;
	struct nk_rect area;
	
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	area = nk_rect(0.f, 0.f, (float) width, (float) height);

	nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
	glfwSwapBuffers(window);
	glfwWaitEventsTimeout(1.0);
}

JG_API int  jg_should_close(jg_ctx_t*) {
	return glfwWindowShouldClose(window);
}

JG_API void jg_request_redraw(jg_ctx_t*) {
	glfwPostEmptyEvent();
}
