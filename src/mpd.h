#pragma once

#include <mpd/client.h>

typedef struct {
	char* artist;
	char* title;
	char* album;
} song_info;

struct mpd_connection* init_mpd(char* host, unsigned short port);

bool fetch_songs(struct mpd_connection* conn);

struct mpd_song* next_song(struct mpd_connection* mpd, song_info* song);

