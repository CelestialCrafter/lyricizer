#include <stdio.h>
#include <curl.h>
#include <mpd/client.h>

#include "options.h"
#include "mpd.h"

int main() {
	curl_global_init(CURL_GLOBAL_ALL);

	options* opts = load_options();
	if (!opts) {
		fprintf(stderr, "main: could not load options\n");
		return 1;
	}

	printf("main: using options: host %s, port %d\n", opts->host, opts->port);

	struct mpd_connection* conn = init_mpd(opts->host, opts->port);
	if (!conn) {
		fprintf(stderr, "main: could not initialize mpd\n");
		return 1;
	}

	if (!fetch_songs(conn)) {
		fprintf(stderr, "main: could not send track list commands\n");
		return 1;
	}

	process_songs(conn, download_lyrics);

	// cleanup
	mpd_connection_free(conn);

	return 0;
}

