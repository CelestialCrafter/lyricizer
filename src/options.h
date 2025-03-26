#pragma once

typedef struct {
	char* music_dir;
	char* host;
	unsigned short port;
	unsigned int request_delay;
	char* lrclib_instance;
} options;

options* load_options();

void options_free(options* opts);

