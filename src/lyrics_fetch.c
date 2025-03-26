#include <stdlib.h>
#include <string.h>

#include "lyrics_fetch.h"
#include "mpd.h"

// some code stolen from https://curl.se/libcurl/c/getinmemory.html

struct memory {
    char *raw_memory;
    size_t size;
};

static size_t write_memory_callback(char* data, size_t size, size_t nmemb, void* userdata) {
    struct memory* chunk = (struct memory*) userdata;
    size_t real_size = size * nmemb;

	// reallocate memory
    char* ptr = realloc(chunk->raw_memory, chunk->size + real_size + 1);
    if (!ptr) {
        fprintf(stderr, "lyrics: not enough memory\n");
        return 0;
    }
    chunk->raw_memory = ptr;

	// copy new data into memory
    memcpy(
		&(chunk->raw_memory[chunk->size]),
		data,
		real_size
	);
    chunk->size += real_size;
    chunk->raw_memory[chunk->size] = 0;

    return real_size;
}

char* fetch_lyrics(CURL* curl, const char* instance, song_info song) {
	// generate url
    char* title = curl_easy_escape(curl, song.title, 0);
    char* artist = curl_easy_escape(curl, song.artist, 0);
    char* album = curl_easy_escape(curl, song.album, 0);

    char url[
        strlen(instance) +
        strlen(title) +
        strlen(artist) +
        strlen(album) +
        64
    ];

    sprintf(
        url,
        "%s/api/get?artist_name=%s&track_name=%s&album_name=%s",
        instance, artist, title, album
    );

	curl_free(title);
	curl_free(artist);
	curl_free(album);

	// set opts
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "lyricizer");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    char error_buf[CURL_ERROR_SIZE];
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

    struct memory chunk = {
		.raw_memory = NULL,
		.size = 0
	};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

	// perform
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        fprintf(
            stderr,
            "lyrics: could not fetch lyrics: (%s) %s\n",
            curl_easy_strerror(code),
            error_buf
        );
		goto cleanup;
	}

	long response_code;
	if (
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code) != CURLE_OK ||
		response_code < 200 ||
		response_code > 299
	) {
		fprintf(stderr, "lyrics: unsuccessful http status (%d)\n", response_code);
		goto cleanup;
	}

cleanup:
    curl_easy_reset(curl);
	return chunk.raw_memory;
}
