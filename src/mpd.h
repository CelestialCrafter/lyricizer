#pragma once

#include <mpd/client.h>

typedef struct {
	const char* artist;
	const char* title;
	const char* album;
	char* path;
} song_info;

struct mpd_connection* init_mpd(char* host, unsigned short port);

bool fetch_songs(struct mpd_connection* conn);

struct mpd_song* next_song(struct mpd_connection* mpd, char* music_dir, song_info* song);

