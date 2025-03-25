#pragma once

typedef struct {
	char* host;
	unsigned short port;
	unsigned int request_delay;
	char* lrclib_instance;
} options;

options* load_options();
