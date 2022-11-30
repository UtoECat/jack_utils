// JackUtils library Utility : JackTCP server :D
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#include <jackutils.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define PROGRAM_NAME "udpserver"
#define PROGRAM_VERSION 0.1
#define PROGRAM_USAGE "udpserver [-h] [-v] [-p PORT]"
#define PROGRAM_HELP  "Copyright (C) UtoECat 2022. All rights reserved!\n This program is free software. GNU GPL 3.0 License! No any Warrianty!"
#include <ju_args.h>
#include <ju_resample.h>

#include <threads.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

struct client {
	struct client* next;
	// inet info
	struct sockaddr_in info;
	int socket;
	// multithreading
	mtx_t lock;
	// auth info
	bool   logged_in;
	size_t data_rate;
	// jack resources
	int jport; // jack port
} *clients = NULL;

// jack loop <> main loop data exchange buffer :)
ju_buff_t buff;
jack_nframes_t oldsize = 0;
static void check_buffer(ju_ctx_t* x) {
	if (oldsize < ju_length(x)) {
		oldsize = ju_length(x);
		ju_buff_resize(&buff, oldsize * sizeof(float));
	}
}

// jack sender callback
static void process_sender(ju_ctx_t* ctx, size_t len) {
	// cache to buffer (lock to prevent data racing)
	ju_buff_lock(&buff);
	check_buffer(ctx);
	ju_buff_append(&buff, ju_port_read(ctx, port), len*sizeof(float));
	ju_buff_unlock(&buff);
}

static void loop(ju_ctx_t* ctx, ju_win_t* w);
static void setport (char, const char* portstr) {
	short port = atoi(portstr);
}

int main(int argc, char** argv) {
	ja_parse(argc, argv, setport, "p:");
	// create context
	ju_ctx_t* ctx = ju_ctx_init("waveform", NULL);
	printf("JACK Version : %s\n", ju_jack_info());
	// open ports
	port = ju_port_open(ctx, "input", JU_INPUT, 0);
	// open window and cache buffers
	ju_win_t* w = ju_win_open(640, 480);
	ju_win_title(w, ju_get_name(ctx));
	oldsize = ju_length(ctx);
	ju_buff_init(&buff, oldsize * sizeof(float));
	// start processing
	ju_start(ctx, process);
	// and wait 'till server dies or window will be closed xD
	while (!ju_win_should_close(w) && ju_is_online(ctx, 0)) {
		loop(ctx, w);
	}
	// free context (and window... and buffer xD)
	// context will be already stopped in case of server disconnect
	ju_win_close(w);
	ju_ctx_uninit(ctx);
	ju_buff_uninit(&buff);
}

static void loop(ju_ctx_t* ctx, ju_win_t* w) {
	double dt = ju_draw_begin(w);
	w_wh_t ws = ju_win_size(w);

	// to prevent data racing
	ju_buff_lock(&buff);
	size_t sz = oldsize;
	float tmp[sz];
	memcpy(tmp, ju_buff_data(&buff), sizeof(float) * sz);
	ju_buff_unlock(&buff);
	glColor3f(0.2,0,0);
	ju_draw_grid(20,20,0,0,ws.w, ws.h);
	glColor3f(1,1,1);
	ju_draw_samples(tmp, sz, 0, ws.h/2, ws.w, ws.h/2);
	ju_win_pool_events();
}
