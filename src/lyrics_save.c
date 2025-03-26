#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <json-c/json.h>

#include "lyrics_save.h"
#include "mpd.h"


void lrcify_path(char* new, const char* path) {
    strcpy(new, path);

    char* last_dot = strrchr(new, '.');
    char* last_slash = strrchr(new, '/');
    
    if (last_dot != NULL && 
        (last_slash == NULL || last_dot > last_slash) &&
        last_dot != new) {
        *last_dot = '\0';
    }
    
    strcat(new, ".lrc");
}

char* prepend_to_lines(const char* prefix, const char* text) {
	size_t prefix_len = strlen(prefix);

    size_t newline_count = 0;
    for (const char* p = text; *p; ++p) {
        if (*p == '\n') newline_count++;
    }

    char* result = malloc(strlen(text) + (newline_count + 1) * prefix_len + 1);
    if (!result) return NULL;

    char* write = result;
    const char* read = text;
    while (*read) {
        // prepend prefix
        if (read == text || *(read - 1) == '\n') {
            strcpy(write, prefix);
            write += prefix_len;
        }

        // copy till end
        while (*read && *read != '\n') {
            *write++ = *read++;
        }

        // copy newline if it exists
        if (*read == '\n') {
            *write++ = *read++;
        }
    }
    *write = '\0';

    return result;
}

void save_lyrics(const char* path, const char* response, song_info song) {
	FILE* file = NULL;
	struct json_object* parsed_json = NULL;

	// json parsing
	parsed_json = json_tokener_parse(response);
	if (!parsed_json) {
		fprintf(stderr, "lyrics: could to parse lrclib response\n");
		goto cleanup;
	}

	struct json_object* synced_lyrics;
	struct json_object* plain_lyrics;
	bool has_synced = json_object_object_get_ex(parsed_json, "syncedLyrics", &synced_lyrics);
	bool has_plain = json_object_object_get_ex(parsed_json, "plainLyrics", &plain_lyrics);

	// writing
	if (!has_synced && !has_plain) {
		fprintf(stderr, "lyrics: no lyrics found\n");
		goto cleanup;
	}

	file = fopen(path, "w");
	if (!file) {
		fprintf(stderr, "lyrics: could not open %s\n", path);
		goto cleanup;
	}

	fprintf(
		file,
		"[ar:%s]\n[al:%s]\n[ti:%s]\n", 
		song.artist, song.album, song.title
	);

	if (has_synced) {
		fprintf(file, json_object_get_string(synced_lyrics));
	} else {
		char* transformed = prepend_to_lines("[00:00.00] ", json_object_get_string(plain_lyrics));
		if (!transformed) {
			fprintf(stderr, "lyrics: out of memory\n");
			goto cleanup;
		}

		fprintf(file, transformed);
		free(transformed);
	}

cleanup:
	if (file) fclose(file);
	json_object_put(parsed_json);
}
