#include "objloader.h"

#include <stdbool.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>

static bool parse_line(obj_model *model, const gchar *line);
static bool handle_polygon_vertex(obj_model *model, gchar **tokens);

obj_model *load_obj(const char *path) {
	gchar *contents;
	GError *error;

	if (!g_file_get_contents(path, &contents, NULL, &error)) {
		fprintf(stderr, "%s\n", error->message);
		return NULL;
	}

	gchar **lines = g_strsplit(contents, "\n", 0);
	g_free(contents);

	obj_model *model = obj_model_new();

	bool ret = true;
	for (gchar **curline = lines; ret && *curline; ++curline) {
		ret = parse_line(model, *curline);
	}

	g_strfreev(lines);

	return model;
}

static bool parse_line(obj_model *model, const gchar *line) {
	gchar **tokens = g_strsplit_set(line, " \t", 0);

	bool ret = false;

	gchar *command = tokens[0];

	if (!command)
		ret = true; // Empty line
	else if (command[0] == '#')
		ret = true; // This is comment, do nothing
	else if (strcmp(command, "v") == 0)
		ret = handle_polygon_vertex(model, tokens);
	else {
		fprintf(stderr, "Can't handle command %s\n", command);
		ret = false;
	}

	g_strfreev(tokens);
	return ret;
}

static bool handle_polygon_vertex(obj_model *model, gchar **tokens) {
	return true;
}

