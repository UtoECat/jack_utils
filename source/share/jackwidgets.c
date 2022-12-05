// GUI Extension for jackutils library.
// Waveviem and waveedit widgets
// 
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#include <jackgui.h>

/* TODO: Custom widgets :
        void draw_red_rectangle_widget(struct nk_context *ctx)
        {
            struct nk_command_buffer *canvas;
            struct nk_input *input = &ctx->input;
            canvas = nk_window_get_canvas(ctx);

            struct nk_rect space;
            enum nk_widget_layout_states state;
            state = nk_widget(&space, ctx);
            if (!state) return;

            if (state != NK_WIDGET_ROM)
                update_your_widget_by_user_input(...);
            nk_fill_rect(canvas, space, 0, nk_rgb(255,0,0));
        }
Important to know if you want to create your own widgets is the `nk_widget`
    call. It allocates space on the panel reserved for this widget to be used,
    but also returns the state of the widget space. If your widget is not seen and does
    not have to be updated it is '0' and you can just return. If it only has
    to be drawn the state will be `NK_WIDGET_ROM` otherwise you can do both
    update and draw your widget. The reason for separating is to only draw and
    update what is actually necessary which is crucial for performance.

NK_API void nk_stroke_polyline(struct nk_command_buffer*, float *points, int point_count, float line_thickness, struct nk_color col);
NK_API void nk_stroke_rect(struct nk_command_buffer*, struct nk_rect, float rounding, float line_thickness, struct nk_color);
NK_API void nk_fill_rect(struct nk_command_buffer*, struct nk_rect, float rounding, struct nk_color);
 */

/* Layout :
 * ///     // fixed widget pixel width
///     nk_layout_row_static(&ctx, 30, 80, 1);
///     if (nk_button_label(&ctx, "button")) {
///         // event handling
///     }
///
///     // fixed widget window ratio width
///     nk_layout_row_dynamic(&ctx, 30, 2);
///     if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
///     if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;
///
///     // custom widget pixel width
///     nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
///     {
///         nk_layout_row_push(&ctx, 50);
///         nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
///         nk_layout_row_push(&ctx, 110);
///         nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
///     }
///     nk_layout_row_end(&ctx);
*/

struct waveinfo waveinfo_default() {
	return (struct waveinfo){-1, 1, 0, 0, 1, 1};
}

#define BG_CELL_MIN_INDEXIES 50
#define BG_CELL_MIN_PIXELS   50
#define BG_CELL_COLOR nk_rgb(0, 55,   40)
#define FG_LINE_COLOR nk_rgb(0, 255, 200)
#define DIRTY_MIN(A, B) ((A) < (B) ? (A) : (B))

static int jg_check_and_get(jg_ctx_t* ctx, struct nk_command_buffer** cnv, struct nk_input** in, struct nk_rect* space) {
	if (!ctx || !cnv || !in || !space) return 0; // error;
	*cnv = nk_window_get_canvas(ctx);
	enum nk_widget_layout_states state = nk_widget(space, ctx);

	if (!state) return 0; // nothing to do
	if (state == NK_WIDGET_ROM) {
		*in = NULL; // no input and update -> only drawing
		return 2;
	}
	*in = &ctx->input;
	return 1;
}

static inline void getlimarr(float* arr, size_t cnt, float* max) {
	if (!cnt) {
		*max = 0;
		return;				
	};
	*max = arr[0];
	for (size_t i = 1; i < cnt; i++) {
		if (arr[i] > *max) *max = arr[i];
	}
}

static inline float limitf(float v, float min, float max) {
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

void jg_waveview(jg_ctx_t* ctx, float* arr, size_t sz, struct waveinfo* info) {
	if (!ctx || !arr || !sz || !info || info->max < info->min) return;

	// try to get get all important data
	struct nk_command_buffer *canvas;
	struct nk_input *input;
	struct nk_rect space;
	if (!jg_check_and_get(ctx, &canvas, &input, &space)) return;

	if (input) {/* TODO: what to do here? */}

	// fill background
	nk_fill_rect(canvas, space, 0, nk_rgb(0, 0, 0));
	// calculate important values
	float width = space.w, height = space.h;
	float range = info->max - info->min;
	// now fill background cells
	for (int i = 0; i < width; i += BG_CELL_MIN_PIXELS) {
		nk_stroke_line(canvas, space.x + i, space.y, space.x + i, space.y +space.h, 1, BG_CELL_COLOR);
	}
	for (int i = 0; i < height; i += BG_CELL_MIN_PIXELS) {
		nk_stroke_line(canvas, space.x, space.y + i, space.x + space.w, space.y + i, 1, BG_CELL_COLOR);
	}
	// new optimized drawing algo

	// get first vertex
	size_t oldindx = 0;
	float oldx = space.x;
	float oldmax = arr[0];
	float step = width/(float)sz;
	if (step < 1.0) step = 1.0;

	for (float i = 1; i < width; i += step) { // optimitsation
		size_t indx = i/width * sz;
		float x = i + space.x;

		// minimal and maximum values in current values range
		float maxv;
		getlimarr(arr + oldindx, indx - oldindx, &maxv);
		maxv = limitf(maxv, info->min, info->max);
		// setup points
		float y1 = space.y + height - (maxv - info->min)/range * height;
		float y2 = space.y + height - (oldmax - info->min)/range * height;
		nk_stroke_line(canvas, oldx, y2, x, y1, 1, FG_LINE_COLOR);
		oldindx = indx;
		oldmax = maxv;
		oldx = x;
	}

	/*
	// and draw all other
	for (size_t i = 1; i < sz; i++) {
		float x = (i / (float)sz) * width + space.x;
		float v = arr[i] > info->min ? (arr[i] < info->max ? arr[i] : info->max) : info->min;
		float y = space.y + height - (v - info->min)/range * height;
		nk_stroke_line(canvas, oldx, oldy, x, y, 2, FG_LINE_COLOR);
		oldx = x; oldy = y;
	}*/
	return; // success
}

#include <math.h>

static float vec2length(struct nk_vec2 vec) {
	// Pifagora theorema lmao
	return sqrtf((vec.x*vec.x)+(vec.y*vec.y));
}

static struct nk_vec2 vec2normal(struct nk_vec2 vec) {
	struct nk_vec2 res = {0, 0};
	float len = vec2length(vec);

	if (len > 0.0) {
		float k = 1.0/len;
		res.x = vec.x * k;
		res.y = vec.y * k;
	}
	return res;
}

static float vec2angle(struct nk_vec2 vec) {
	return atan2f(vec.y, vec.x);
}

#define DIRTY_POW2(A) (A)*(A)

static float calcposon(struct nk_vec2 pos, struct nk_vec2 center) {
	float x = pos.x - center.x;
	float y = pos.y - center.y;
	float a = atan2f(-x, -y);
	a += 3.1415;
	return a;
}

/*
 * FLoat value whell.
 * Returns 1 if whell was scrolled
 */
int jg_whell_float(jg_ctx_t* ctx, float* value, float min, float step, float max) {
	if (!ctx || !value || step <= 0.0 || min > max) return 0;

	// try to get get all important data
	struct nk_command_buffer *canvas;
	struct nk_input *input;
	struct nk_rect bounds;
	if (!jg_check_and_get(ctx, &canvas, &input, &bounds)) return 0;

	//style = &ctx->style; // `style->slider
	float old_value = *value;

	float  radius = DIRTY_MIN(bounds.w/2.0f, bounds.h/2.0f);
	struct nk_vec2 center = {bounds.x+bounds.w/2.0, bounds.y+bounds.h/2.0};
	int himouse = 0;

	if (input) {
		struct nk_vec2 mdiff = {
			input->mouse.pos.x - center.x,
			input->mouse.pos.y - center.y
		};
		if (vec2length(mdiff) <= radius) { // we are in bounds!
			if (input->mouse.buttons[NK_BUTTON_LEFT].down == nk_true) {
				// move whell using mouse movement
				float v0 = calcposon(input->mouse.prev, center);
				float v1 = calcposon(input->mouse.pos , center);
				//(*value - min)/max * 3.1415 * 1.5 + 3.1415 * 0.25;
				v0 = v0 / (3.1415 * 1.5) - 3.1415 * 0.25; // radian to value
				v0 = v0*(max - min) + min;
				v1 = v1 / (3.1415 * 1.5) - 3.1415 * 0.25;
				v1 = v1*(max - min) + min;
				*value += v1 - v0; // add difference
			} else { // move whell using.. mouse whell! :D
				*value += input->mouse.scroll_delta.y * step/max;
			}
			himouse = 1;
		}
		// check is in bounds
		if (*value < min) *value = min;
		if (*value > max) *value = max;
	}

	// draw whell
	struct nk_rect circle = {
		center.x - radius,
		center.y - radius,
		radius * 2,
		radius * 2
	};

	struct nk_color col = nk_rgb(166,166,166);
	if (himouse) col = nk_rgb(200,200,200); 

	nk_fill_circle(canvas, circle, col);
	nk_stroke_circle(canvas, circle, 2, nk_rgb(26, 26, 26));

	float path = (*value - min)/max * 3.1415 * 1.5 + 3.1415 * 0.25;
	float kx   = sinf(path) * radius;
	float ky   = cosf(path) * radius;

	nk_stroke_line(canvas, center.x+kx, center.y+ky, 
			center.x + kx/3, center.y+ ky/3, 4, nk_rgb(32,32,32));

	nk_stroke_line(canvas, center.x, center.y, center.x, center.y, 5, nk_rgb(200,0,0)); 
	nk_fill_triangle(canvas, center.x, center.y, 
			bounds.x, bounds.y + bounds.h,
			bounds.x + bounds.w, bounds.y + bounds.h,
			nk_rgb(64,64,64));
	nk_stroke_triangle(canvas, center.x, center.y, 
			bounds.x, bounds.y + bounds.h,
			bounds.x + bounds.w, bounds.y + bounds.h,
			4, nk_rgb(32,32,32));
	// if value is changed
	return (old_value > *value || old_value < *value);
}

/* Chart implementation :
 * color = g->slots[slot].color;
        if (!(layout->flags & NK_WINDOW_ROM) &&
            NK_INBOX(i->mouse.pos.x,i->mouse.pos.y, g->slots[slot].last.x-3, g->slots[slot].last.y-3, 6, 6)){
            ret = nk_input_is_mouse_hovering_rect(i, bounds) ? NK_CHART_HOVERING : 0;
            ret |= (i->mouse.buttons[NK_BUTTON_LEFT].down &&
                i->mouse.buttons[NK_BUTTON_LEFT].clicked) ? NK_CHART_CLICKED: 0;
            color = g->slots[slot].highlight;
        }
        nk_fill_rect(out, bounds, 0, color);
        g->slots[slot].index += 1;
        return ret; */

/*
 * Custom top-level jackutils bar :)
 */

#define arr_foreach(arr) for (struct jg_bar_item* pos = arr; pos->cb_draw != NULL; pos++)

int jg_ju_topbar(jg_ctx_t* gui, ju_ctx_t* cli, struct jg_bar_item* arr) {
	if (!arr || !gui || !cli) return 0;

	nk_layout_row_begin(gui, NK_STATIC, 72, 2);
	nk_layout_row_push(gui, 72);
	nk_image(gui, jg_jackutils_icon());
	nk_layout_row_push(gui, nk_window_get_content_region(gui).w - 72 - 12);
	// top group after icon
	nk_group_begin(gui, "top-group", 0);
		// setup template layout
		nk_layout_row_template_begin(gui, 40);
		nk_layout_row_template_push_variable(gui, 80);
		arr_foreach(arr) {	
			nk_layout_row_template_push_static(gui, pos->width);
		}
		nk_layout_row_template_push_static(gui, 80);
		nk_layout_row_template_end(gui);
			// first layer
			nk_labelf(gui, NK_TEXT_CENTERED, "(Jackutils) : %s", program_info.name);
			arr_foreach(arr) {	
				pos->cb_draw(gui, pos);
			}
			if (nk_button_label(gui, "About")) {
				jg_show_about(gui);
			};
			// second layout
			nk_layout_row_template_begin(gui, 10);
			nk_layout_row_template_push_variable(gui, 80);
			arr_foreach(arr) {	
				nk_layout_row_template_push_static(gui, pos->width);
			}
			nk_layout_row_template_push_static(gui, 80);
			nk_layout_row_template_end(gui);
			// second layer
			if (ju_is_online(cli, 0))
			nk_labelf(gui, NK_TEXT_CENTERED, "Jack : [%s, length=%li]", ju_jack_info(), ju_length(cli));
			else nk_label(gui, "Jack : OFFLINE!", NK_TEXT_CENTERED);
			arr_foreach(arr) {
				nk_label(gui, pos->desc, NK_TEXT_CENTERED);
			}
			nk_labelf(gui, NK_TEXT_CENTERED, "v.%0.1f", program_info.version);
		nk_group_end(gui);
	nk_layout_row_end(gui);
	return 0;
}
#undef arr_foreach

struct jg_bar_item jg_null_item() {
	return (struct jg_bar_item){NULL, {NULL}, 0, NULL};
}

static void cb_float(jg_ctx_t* gui, struct jg_bar_item* i) {
	jg_whell_float(gui, (float*)i->data[0].p, i->data[1].n, i->data[2].n, i->data[3].n);
}

struct jg_bar_item jg_float_item(const char* desc, float* val, float min, float step, float max) {
	struct jg_bar_item i = jg_null_item();
	i.cb_draw = cb_float;
	i.data[0].p = val;
	i.data[1].n = min;
	i.data[2].n = step;
	i.data[3].n = max;
	i.width = 45;
	i.desc = desc ? desc : "(null)";
	return i;
}

static void cb_text(jg_ctx_t* gui, struct jg_bar_item* i) {
	nk_label(gui, (char*)i->data[0].p, NK_TEXT_CENTERED);
}

struct jg_bar_item jg_text_item(const char* desc, const char* text, int w) {
	struct jg_bar_item i = jg_null_item();
	i.cb_draw = cb_text;
	i.data[0].p = text;
	i.width = w;
	i.desc = desc ? desc : "(null)";
	return i;
}

