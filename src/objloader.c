#include "objloader.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>

static bool parse_line(obj_model *model, const gchar *line);
static bool handle_geometric_vertex(obj_model *model, gchar **tokens);

static void strv_remove_empty(gchar **tokens);

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
	strv_remove_empty(tokens);

	bool ret = false;

	gchar *command = tokens[0];

	if (!command)
		ret = true; // Empty line
	else if (command[0] == '#')
		ret = true; // This is comment, do nothing
	else if (strcmp(command, "v") == 0)
		ret = handle_geometric_vertex(model, tokens);
	else {
		fprintf(stderr, "Can't handle command %s\n", command);
		ret = false;
	}

	g_strfreev(tokens);
	return ret;
}

static bool handle_geometric_vertex(obj_model *model, gchar **tokens) {
	int len = 0;
	for (gchar **token = tokens; *token; ++token)
		++len;

	if (len < 4 || len > 5) {
		fprintf(stderr, "Can't handle 'v' with %i args\n",
				len - 1);
		return false;
	}

	vertex vert = (vertex){ .w = 1.0 };
	vert.x = strtod(tokens[1], NULL);
	vert.y = strtod(tokens[2], NULL);
	vert.z = strtod(tokens[3], NULL);
	if (len == 5)
		vert.w = strtod(tokens[4], NULL);

	g_array_append_val(model->geometric_vertices, vert);

	return true;
}

static void strv_remove_empty(gchar **tokens) {
	int new_pos = 0;

	for (gchar **token = tokens; *token; ++token)
		if (**token)
			tokens[new_pos++] = *token;
		else
			free(*token);
	tokens[new_pos] = NULL;
}

