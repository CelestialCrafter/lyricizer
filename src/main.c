#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <mpd/client.h>

#include "options.h"
#include "mpd.h"
#include "lyrics_fetch.h"

int main() {
	// init
	options* opts = load_options();
	if (!opts) {
		fprintf(stderr, "main: could not load options\n");
		return 1;
	}

	fprintf(stderr, "main: using options: host %s, port %d\n", opts->host, opts->port);

	struct mpd_connection* mpd = init_mpd(opts->host, opts->port);
	if (!mpd) {
		fprintf(stderr, "main: could not initialize mpd\n");
		return 1;
	}

	CURL* curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "main: could not initialize curl\n");
		return 1;
	}

	// processing
	if (!fetch_songs(mpd)) {
		fprintf(stderr, "main: could not send track list commands\n");
		return 1;
	}

	song_info song;
	struct mpd_song* mpd_song;
	while ((mpd_song = next_song(mpd, &song)) != NULL) {
		if (song.artist && song.title && song.album) {
			download_lyrics(curl, opts->lrclib_instance, song);
			sleep(opts->request_delay);
		} else {
			fprintf(stderr, "mpd: ignoring song due to incomplete metadata\n");
		}

		mpd_song_free(mpd_song);
	}

	// cleanup
	mpd_connection_free(mpd);
	curl_easy_cleanup(curl);
	free(opts);

	return 0;
}

