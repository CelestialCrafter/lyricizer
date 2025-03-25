#include <stdlib.h>
#include <toml.h>

#include "options.h"

static const char* OPTIONS_PATH = "options.toml";

void parse_options(options* opts, toml_table_t* table) {
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
	FILE* file = fopen(OPTIONS_PATH, "r");
	if (!file) {
		fprintf(stderr, "options: could not open %s\n", OPTIONS_PATH);
		return NULL;
	}

	char error_buf[256];
	toml_table_t* table = toml_parse_file(file, error_buf, sizeof(error_buf));
	fclose(file);

	if (!table) {
		fprintf(stderr, "options: could not parse %s: %s\n", OPTIONS_PATH, error_buf);
		return NULL;
	}

	options* opts = (options*) calloc(1, sizeof(options));
	opts->request_delay = 1;
	opts->lrclib_instance = "https://lrclib.net";

	if (!opts) {
		fprintf(stderr, "options: out of memory\n");
		return NULL;
	}

	parse_options(opts, table);

	return opts;
}

