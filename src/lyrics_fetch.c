#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "lyrics_fetch.h"
#include "mpd.h"

bool perform_download(CURL* curl) {
	char error_buf[CURL_ERROR_SIZE];
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

	CURLcode code = curl_easy_perform(curl);
	bool ok = code == CURLE_OK;
	if (!ok) {
		fprintf(
			stderr,
			"lyrics: could not download lyrics: (%s) %s\n",
			curl_easy_strerror(code),
			error_buf

		);
	}

	return ok;
}

void escape_song(CURL* curl, song_info* song) {
	song->title = curl_easy_escape(curl, song->title, 0);
	song->artist = curl_easy_escape(curl, song->artist, 0);
	song->album = curl_easy_escape(curl, song->album, 0);
}

void free_song(song_info* song) {
	free(song->title);
	free(song->artist);
	free(song->album);
}

void fetch_lyrics(CURL* curl, const char* instance, song_info song) {
	fprintf(stderr, "lyrics: downloading \"%s - %s\"\n", song.artist, song.title);

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "lyricizer");

	escape_song(curl, &song);
	char url[
		strlen(instance) +
		strlen(song.title) +
		strlen(song.artist) +
		strlen(song.album) +
		128
	];
	sprintf(
		url,
		"%s/api/get?artist_name=%s&track_name=%s&album_name=%s",
		instance, song.title, song.artist, song.album
	);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	perform_download(curl);

	curl_easy_reset(curl);
}
