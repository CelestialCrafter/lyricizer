#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <mpd/client.h>

#include "options.h"
#include "mpd.h"
#include "lyrics_fetch.h"
#include "lyrics_save.h"

bool process_song(options* opts, struct mpd_connection* mpd, CURL* curl) {
	song_info song;
	struct mpd_song* mpd_song = next_song(mpd, opts->music_dir, &song);
	if (!mpd_song) {
		return false;
	}

	if (song.artist && song.title && song.album) {
		fprintf(stderr, "main: processing \"%s - %s\"\n", song.artist, song.title);

		char path[PATH_MAX];
		lrcify_path(path, song.path);
		if (access(path, F_OK) == 0) {
			fprintf(stderr, "main: lyrics already exist\n");
			goto cleanup_song;
		}

		char* response = fetch_lyrics(curl, opts->lrclib_instance, song);
		sleep(opts->request_delay);
		if (!response) goto cleanup_song;

		save_lyrics(path, response, song);
		free(response);
	} else {
		fprintf(stderr, "main: ignoring song due to incomplete metadata\n");
	}

cleanup_song:
	mpd_song_free(mpd_song);
	free(song.path);

	return true;
}

int main() {
	int retcode = 0;

	CURL* curl = NULL;
	options* opts = NULL;
	struct mpd_connection* mpd = NULL;

	curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "main: could not initialize curl\n");
		retcode = 1;
		goto cleanup;
	}

	opts = load_options();
	if (!opts) {
		retcode = 1;
		goto cleanup;
	}

	mpd = init_mpd(opts->host, opts->port);
	if (!mpd) {
		retcode = 1;
		goto cleanup;
	}

	if (!fetch_songs(mpd)) {
		retcode = 1;
		goto cleanup;
	}

	while (process_song(opts, mpd, curl)) {}

cleanup:
	if (mpd) mpd_connection_free(mpd);
	if (opts) options_free(opts);
	curl_easy_cleanup(curl);

	return retcode;
}

