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

static void error_callback(int, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

// implement rendering
#define NK_GLFW_GL3_IMPLEMENTATION
#include <ext/nuklear_glfw_gl3.h>
#include <ext/nuklear_image.h>

/*
 * Initializes GUI context.
 * @ret NULL in case of error
 */

static GLFWwindow* window;
static int showed = 1;
static struct nk_image    ju_image;

// static const icon :)
#include <ju_icon.h>

JG_API struct nk_image jg_jackutils_icon(void) {
	return ju_image;
}

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
	glfwSetWindowSizeLimits(window, 50, 50, GLFW_DONT_CARE, GLFW_DONT_CARE);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);

	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	
	struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
	nk_glfw3_font_stash_end();
	/*nk_style_set_font(ctx, &droid->handle);*/

	// init jackutils icon
	ju_image = jg_image_load_from_memory(ju_icon_data, ju_icon_width, ju_icon_height, ju_icon_channels);
	if (!ju_image.handle.id)
		perror("[jackgui] Can't load jackutils icon!");
	showed = 1;
	return ctx;
}

/*
 * Destroys GUI context
 */
JG_API void      jg_uninit(jg_ctx_t*) {
	showed = 0;
	jg_image_free(ju_image);
	nk_glfw3_shutdown();
}

/*
 * Starts GUI objects enumeration.
 * You must call jg_end() when you done
 *
 * @ret 1 on sucess. Else you must not enumerate anything!
 */
JG_API int  jg_begin(jg_ctx_t* ctx) {
	if (!showed) return 0; // nothing showed
	int width, height;
	struct nk_rect area;
	
	glfwGetWindowSize(window, &width, &height);
	area = nk_rect(0.f, 0.f, (float) width, (float) height);

	nk_glfw3_new_frame();
	return nk_begin(ctx, "", area, NK_WINDOW_BACKGROUND);
}

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 512 * 1024

static int show_about = 0;
static int show_message = 0;
static char messge_data[512] = {0};
const int   popup_flags = NK_WINDOW_CLOSABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE | NK_WINDOW_BORDER | NK_WINDOW_TITLE;

static void draw_text_lines(jg_ctx_t* gui, const char* txt) {
	nk_layout_row_dynamic(gui, 15, 1);
	size_t pos = 0, len = strlen(txt) + 1;
	for (size_t i = pos; i < len; i++) if (txt[i] == '\n' || txt[i] == '\0') {
		size_t cnt = i - pos;
		char buff[cnt + 1];
		memcpy(buff, txt + pos, cnt);
		buff[cnt] = '\0';
		nk_label(gui, buff, NK_TEXT_CENTERED);
		pos = i + 1;
	}
}

static void process_about(jg_ctx_t* gui) {
	if (show_about) {
		static struct nk_rect rect = {20, 20, 480, 320};
		if (nk_begin(gui, "about", rect, popup_flags)) {


			// draw icon
			nk_layout_row_template_begin(gui, 64);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_push_static(gui, 64);
			nk_layout_row_template_push_variable(gui, 80);
			nk_layout_row_template_end(gui);

			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			nk_image(gui, jg_jackutils_icon());
			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			
			// draw basic info
			nk_layout_row_dynamic(gui, 15, 1);
			nk_labelf(gui, NK_TEXT_CENTERED, "%s v.%.1f", program_info.name, program_info.version);
			nk_labelf(gui, NK_TEXT_CENTERED, "by %s", program_info.author);
			nk_label(gui, "", NK_TEXT_CENTERED); // dummy

			// select info tab
			nk_layout_row_template_begin(gui, 25);
			nk_layout_row_template_push_variable(gui, 32);
			nk_layout_row_template_push_static(gui, 64);
			nk_layout_row_template_push_static(gui, 64);
			nk_layout_row_template_push_static(gui, 96);
			nk_layout_row_template_push_variable(gui, 32);
			nk_layout_row_template_end(gui);

			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			if (nk_button_label(gui, "About")) show_about = 1;
			if (nk_button_label(gui, "License")) show_about = 2;
			if (nk_button_label(gui, "Contributors")) show_about = 3;
			nk_label(gui, "", NK_TEXT_CENTERED); // dummy

			// draw info
			nk_layout_row_dynamic(gui, 15, 1);
			nk_label(gui, "", NK_TEXT_CENTERED); // dummy
			if (show_about == 1) {
				draw_text_lines(gui, program_info.description); 
				// github button
				nk_layout_row_template_begin(gui, 25);
				nk_layout_row_template_push_variable(gui, 32);
				nk_layout_row_template_push_static(gui, 128);
				nk_layout_row_template_push_variable(gui, 32);
				nk_layout_row_template_end(gui);

				nk_label(gui, "", NK_TEXT_CENTERED);
				if (nk_button_label(gui, "Get Source Code")) {
					char buff[512];
					snprintf(buff, 511, "xdg-open %s", program_info.source_url);
					system(buff);
				};
				nk_label(gui, "", NK_TEXT_CENTERED);
			}
			else if (show_about == 2) draw_text_lines(gui, program_info.license); 
			else {
				const char** i = program_info.contributors;
				nk_label(gui, "Contributors : ", NK_TEXT_CENTERED);
				while(*i && **i) {
					nk_label(gui, *i, NK_TEXT_CENTERED);
					i++;
				}
			}
		} else {
			show_about = 0;
		}
		nk_end(gui);
	}
}

static void process_message(jg_ctx_t* gui) {
	if (show_message) {
		static struct nk_rect rect = {20, 20, 480, 320};
		if (nk_begin(gui, "message", rect, popup_flags)) {
			nk_layout_row_dynamic(gui, 15, 1);
			draw_text_lines(gui, messge_data);
			// ok button
			nk_layout_row_template_begin(gui, 25);
			nk_layout_row_template_push_variable(gui, 32);
			nk_layout_row_template_push_static(gui, 64);
			nk_layout_row_template_push_variable(gui, 32);
			nk_layout_row_template_end(gui);

			nk_label(gui, "", NK_TEXT_CENTERED);
			if (nk_button_label(gui, "Ok")) {
				show_message = 0;
				messge_data[0] = '\0';
			};
			nk_label(gui, "", NK_TEXT_CENTERED);
		} else {
			show_message = 0;
			messge_data[0] = '\0';
		}
		nk_end(gui);
	}
}

JG_API void jg_end(jg_ctx_t* ctx, int mul) {
	if (!showed) return;
	nk_end(ctx);

	// draw special windows before stop :D
	process_about(ctx);
	process_message(ctx);

	int width, height;
	struct nk_rect area;
	
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	area = nk_rect(0.f, 0.f, (float) width, (float) height);

	nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER * mul, MAX_ELEMENT_BUFFER * mul);
	glfwSwapBuffers(window);
	glfwWaitEventsTimeout(1.0);
}

JG_API void jg_show_message(jg_ctx_t*, const char* msg) {
	size_t len = strlen(msg), currlen = strlen(messge_data);
	if (len + currlen >= 511) len = 510 - currlen;
	if (!len) return;
	memcpy(messge_data + currlen, msg, len);
	messge_data[currlen + len] = '\0';
	show_message = 1;
}

JG_API void jg_show_about(jg_ctx_t*) {
	show_about = 1;
}

JG_API void jg_sync_visibility(jg_ctx_t* gui, ju_ctx_t* ctx){
	int val = ju_need_gui(ctx);
	int req = glfwWindowShouldClose(window);
	if (req) {
		jg_set_visibility(gui, 0);
		ju_set_gui(ctx, 0);
		glfwSetWindowShouldClose(window, 0);
		return;
	}

	// if difference
	if (val != showed) {
		jg_set_visibility(gui, val);
	}
}

JG_API void jg_set_visibility(jg_ctx_t*, int b) {
	if (b) glfwShowWindow(window);
	else glfwHideWindow(window);
	showed = b;
}

JG_API void jg_request_redraw(jg_ctx_t*) {
	glfwPostEmptyEvent();
}

JG_API struct nk_image jg_image_load_from_memory(const unsigned char *ptr, int w, int h, int ch);
JG_API void jg_image_free(struct nk_image img);


