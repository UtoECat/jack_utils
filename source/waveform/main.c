#include <jackutils.h>

// JackUtils library usage example
// Simple Gateway + generator

#include <stdlib.h>
#include <stdio.h>

int ports[3] = {0};

ju_buff_t* buff = NULL;
jack_nframes_t oldsize = 0;

// source of /|/|/| signal XD
size_t generator(void*, ju_sample_t* d, size_t c) {
	static signed char i = 0;
	*d = (i++)/128.0f;
	return 1;
}

// jack process callback
void process(ju_ctx_t* ctx, size_t len) {
	// output < input
	ju_port_write(ctx, ports[1], ju_port_read(ctx, ports[0]));
	// cache to buffer (lock to prevent data racing)
	ju_buff_lock(buff);
	ju_buff_addarr(buff, ju_port_read(ctx, ports[0]), len);
	ju_buff_setpos(buff, 0);
	ju_buff_unlock(buff);
	// generator
	ju_port_write_stream(ctx, ports[2], generator, NULL);
}

void check_buffer(ju_ctx_t* x) { // if buffer size is not enough - realloc it!
	if (oldsize < ju_length(x)) {
		oldsize = ju_length(x);
		fprintf(stderr, "buffer size changed to %li\n", oldsize);
		buff = ju_buff_realloc(buff, oldsize);
	}
}

void loop(ju_ctx_t* ctx) {
	ju_draw_begin(ctx, DEVICE_PROJECTION);

	// to prevent data racing
	ju_buff_lock(buff);
	ju_draw_samples(ju_buff_get(buff), ju_buff_length(buff),
		-1, 0, 2);
	ju_buff_unlock(buff);

	ju_draw_show(ctx);
	ju_draw_pool();
}



int main(void) {
	// create context
	ju_ctx_t* ctx = ju_ctx_init("waveform", NULL);
	// debug info
	printf("JACK Version : %s\nGLFW version : %s\n", ju_jack_info(),
		ju_glfw_info());
	// open ports
	ports[0] = ju_port_open(ctx, "input", JU_INPUT, 0),
	ports[1] = ju_port_open(ctx, "output", JU_OUTPUT, 0),
	// independed generator is terminal :p
	ports[2] = ju_port_open(ctx, "generator", JU_OUTPUT, JackPortIsTerminal);
	// open window and cache buffers
	ju_ctx_win(ctx, 640, 480);
	check_buffer(ctx);
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies or window will be closed xD
	ju_loop(ctx, loop, 0);
	// free context (window will be freed too in context freeing functuion)
	ju_ctx_unint(ctx);
	ju_buff_uninit(buff);
}
