#include <jackutils.h>

// JackUtils library usage example
// Simple Gateway + generator

#include <stdlib.h>
#include <stdio.h>

int ports[3] = {0};

size_t generator(void*, ju_sample_t* d, size_t c) {
	static signed char i = 0;
	*d = (i++)/128.0f;
	return 1;
}

void process(ju_ctx_t* ctx, size_t len) {
	ju_port_write(ctx, ports[1], ju_port_read(ctx, ports[0]));
	ju_port_write_stream(ctx, ports[2], generator, NULL);
}

int main(void) {
	printf("Heloo World!\n");
	// create context
	ju_ctx_t* ctx = ju_ctx_init("test", NULL);

	// debug info
	printf("JACK Version : %s\n GLFW version : %s\n", ju_jack_info(),
		ju_glfw_info());
	
	// open ports
	ports[0] = ju_port_open(ctx, "input", JU_INPUT, 0),
	ports[1] = ju_port_open(ctx, "output", JU_OUTPUT, 0),
	// independed generator is terminal :p
	ports[2] = ju_port_open(ctx, "generator", JU_OUTPUT, JackPortIsTerminal);
	
	// start processing
	ju_start(ctx, process);

	// and wait 'till server dies xD
	ju_wait(ctx, NULL);

	// free context
	ju_ctx_unint(ctx);
}
