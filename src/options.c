#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <toml.h>

#include "options.h"

void parse_options(options* opts, toml_table_t* table) {
	toml_datum_t music_dir = toml_string_in(table, "music_dir");
	if (music_dir.ok) {
		opts->music_dir = music_dir.u.s;
	}

	toml_datum_t host = toml_string_in(table, "host");
	if (host.ok) {
		opts->host = host.u.s;
	}

	toml_datum_t port = toml_int_in(table, "port");
	if (port.ok) {
		opts->port = port.u.i;
	}

	toml_datum_t delay = toml_int_in(table, "request_delay");
	if (delay.ok) {
		opts->request_delay = delay.u.i;
	}

	toml_datum_t instance = toml_int_in(table, "lrclib_instance");
	if (instance.ok) {
		opts->lrclib_instance = instance.u.s;
	}

}

options* load_options() {
	options* retval = NULL;
	toml_table_t* table = NULL;
	FILE* file = NULL;

	const char* home = getenv("HOME");
	if (!home) {
		fprintf(stderr, "options: no HOME environment variable set\n");
		goto cleanup;
	}

	char path[PATH_MAX];
	strcpy(path, home);
	strcat(path, "/.config/lyricizer.toml");

	file = fopen(path, "a+");
	if (!file) {
		fprintf(stderr, "options: could not open %s\n", path);
		goto cleanup;
	}

	char error_buf[256];
	table = toml_parse_file(file, error_buf, sizeof(error_buf));
	if (!table) {
		fprintf(stderr, "options: could not parse options: %s\n", error_buf);
		goto cleanup;
	}

	options* opts = (options*) calloc(1, sizeof(options));
	if (!opts) {
		fprintf(stderr, "options: out of memory\n");
		goto cleanup;
	}

	parse_options(opts, table);

	if (!opts->music_dir) {
		fprintf(stderr, "options: music dir option not set\n");
		goto cleanup;
	}

	if (!opts->lrclib_instance) {
		char* ptr = malloc(32);
		if (!ptr) {
			fprintf(stderr, "options: out of memory\n");
			goto cleanup;
		}

		strcpy(ptr, "https://lrclib.net");
		opts->lrclib_instance = ptr;
	}

	if (!opts->request_delay) {
		opts->request_delay = 1;
	}

	fprintf(
		stderr,
		"options: using: host %s, port %d, music_dir %s, request_delay %d, irclib_instance %s\n",
		opts->host, opts->port, opts->music_dir, opts->request_delay, opts->lrclib_instance
	);

	retval = opts;

cleanup:
	toml_free(table);
	if (file) fclose(file);
	return retval;
}

void options_free(options* opts) {
	free(opts->music_dir);
	free(opts->host);
	free(opts->lrclib_instance);
	free(opts);
}

