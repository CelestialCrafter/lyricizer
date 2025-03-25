#include <stdio.h>
#include <mpd/client.h>

#include "mpd.h"

struct mpd_connection* init_mpd(char* host, unsigned short port) {
	struct mpd_connection *mpd = mpd_connection_new(host, port, 0);
	if (!mpd) {
		fprintf(stderr, "mpd: out of memory\n");
		return NULL;
	}

	if (mpd_connection_get_error(mpd) != MPD_ERROR_SUCCESS) {
		fprintf(stderr, "mpd: %s\n", mpd_connection_get_error_message(mpd));
		mpd_connection_free(mpd);
		return NULL;
	}

	return mpd;
}

bool fetch_songs(struct mpd_connection* mpd) {
	if (!mpd_command_list_begin(mpd, false)) {
		fprintf(stderr, "mpd: could not begin list\n");
		return false;
	}

	if (!mpd_send_clear_tag_types(mpd)) {
		fprintf(stderr, "mpd: could not clear tag types\n");
		return false;
	}

	enum mpd_tag_type tag_types[3] = {
		MPD_TAG_ARTIST,
		MPD_TAG_ALBUM,
		MPD_TAG_TITLE
	};

	if (!mpd_send_enable_tag_types(mpd, tag_types, 3)) {
		fprintf(stderr, "mpd: could not enable tag types\n");
		return false;
	}

	if (!mpd_send_list_all_meta(mpd, NULL)) {
		fprintf(stderr, "mpd: could not list all meta\n");
		return false;
	}

	if (!mpd_command_list_end(mpd)) {
		fprintf(stderr, "mpd: could not end list\n");
		return false;
	}

	return true;
}

struct mpd_song* next_song(struct mpd_connection* mpd, song_info* song) {
	struct mpd_song* mpd_song = mpd_recv_song(mpd);
	if (!mpd_song) {
		return NULL;
	}

	song->artist = mpd_song_get_tag(mpd_song, MPD_TAG_ARTIST, 0);
	song->title = mpd_song_get_tag(mpd_song, MPD_TAG_TITLE, 0);
	song->album = mpd_song_get_tag(mpd_song, MPD_TAG_ALBUM, 0);
		
	return mpd_song;
}

