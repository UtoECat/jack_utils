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

void jg_waveview(jg_ctx_t* ctx, float* arr, size_t sz, struct waveinfo* info) {
	if (!ctx || !arr || !sz || !info) return;

	struct nk_command_buffer *canvas;
	struct nk_input *input = &ctx->input;
	canvas = nk_window_get_canvas(ctx);

	struct nk_rect space;
	enum nk_widget_layout_states state = nk_widget(&space, ctx);
	if (!state) return; // nothing to do

	if (state != NK_WIDGET_ROM) {
   	//update_your_widget_by_user_input(...);
	}

	// fill background
	nk_fill_rect(canvas, space, 0, nk_rgb(0, 0, 0));

	// calculate important values
	float width = space.w, height = space.h;
	float range = info->max - info->min;

	float oldx = space.x, oldy = space.y + height - (arr[0] - info->min)/range * height;

	for (size_t i = 1; i < sz; i++) {
		float x = (i / (float)sz) * width + space.x;
		float v = arr[i] > info->min ? (arr[i] < info->max ? arr[i] : info->max) : info->min;
		float y = space.y + height - (v - info->min)/range * height;
		nk_stroke_line(canvas, oldx, oldy, x, y, 1, nk_rgb(0, 255, 200));
		oldx = x; oldy = y;
	}
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
