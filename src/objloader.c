#include "objloader.h"

#include <stdbool.h>

#include <glib.h>
#include <glib/gstdio.h>

bool parse_line(const gchar *line);

int load_obj(const char *path) {
	gchar *contents;
	GError *error;

	if (!g_file_get_contents(path, &contents, NULL, &error)) {
		fprintf(stderr, "%s\n", error->message);
		return -1;
	}

	gchar **lines = g_strsplit(contents, "\n", 0);
	g_free(contents);

	for (char **curline = lines; *curline; ++curline) {
		parse_line(*curline);
	}

	g_strfreev(lines);

	return 0;
}

bool parse_line(const gchar *line) {

	return true;
}

