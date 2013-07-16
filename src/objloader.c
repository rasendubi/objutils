#include "objloader.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>

static bool parse_line(obj_model *model, const gchar *line);

static bool handle_geometric_vertex(obj_model *model, gchar **tokens);
static bool handle_texture_vertex(obj_model *model, gchar **tokens);
static bool handle_vertex_normal(obj_model *model, gchar **tokens);
static bool handle_parameter_point(obj_model *model, gchar **tokens);

static bool handle_faces(obj_model *model, gchar **tokens);

static size_t strv_len(gchar **tokens);
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
	for (gchar **curline = lines; *curline; ++curline) {
		ret = parse_line(model, *curline);
	}

	g_strfreev(lines);

	return model;
}

static bool parse_line(obj_model *model, const gchar *line) {
	static const struct {
		char *command;
		bool (*handler)(obj_model *, gchar **);
	} handlers[] = {
		"v",  handle_geometric_vertex,
		"vt", handle_texture_vertex,
		"vn", handle_vertex_normal,
		"vp", handle_parameter_point,
		"f",  handle_faces
	};
	static const size_t n_handlers = sizeof(handlers)/sizeof(handlers[0]);

	gchar **tokens = g_strsplit_set(line, " \t", 0);
	strv_remove_empty(tokens);

	bool ret = false;

	gchar *command = tokens[0];

	if (!command)
		ret = true; // Empty line
	else if (command[0] == '#')
		ret = true; // This is comment, do nothing
	else {
		for (size_t i = 0; i < n_handlers; ++i)
			if (strcmp(handlers[i].command, command) == 0) {
				ret = handlers[i].handler(model, tokens);
				break;
			}
	}

	g_strfreev(tokens);
	return ret;
}

static bool handle_geometric_vertex(obj_model *model, gchar **tokens) {
	size_t n_args = strv_len(tokens) - 1;
	if (n_args < 3 || n_args > 4) {
		fprintf(stderr, "Can't handle 'v' with %zu args\n",
				n_args);
		return false;
	}

	geometric_vertex vertex = (geometric_vertex){ .w = 1.0 };
	vertex.x = strtod(tokens[1], NULL);
	vertex.y = strtod(tokens[2], NULL);
	vertex.z = strtod(tokens[3], NULL);
	if (n_args == 4)
		vertex.w = strtod(tokens[4], NULL);

	g_array_append_val(model->geometric_vertices, vertex);

	return true;
}

static bool handle_texture_vertex(obj_model *model, gchar **tokens) {
	size_t n_args = strv_len(tokens) - 1;
	if (n_args < 1 || n_args > 3) {
		fprintf(stderr, "Can't handle 'vt' with %zu args\n",
				n_args);
		return false;
	}

	texture_vertex vertex = (texture_vertex){};

	vertex.u = strtod(tokens[1], NULL);
	if (n_args > 1)
		vertex.v = strtod(tokens[2], NULL);
	if (n_args > 2)
		vertex.w = strtod(tokens[3], NULL);

	g_array_append_val(model->texture_vertices, vertex);

	return true;
}

static bool handle_vertex_normal(obj_model *model, gchar **tokens) {
	size_t n_args = strv_len(tokens) - 1;
	if (n_args != 3) {
		fprintf(stderr, "Can't handle 'vn' with %zu args\n",
				n_args);
		return false;
	}

	vertex_normal normal;
	normal.i = strtod(tokens[1], NULL);
	normal.j = strtod(tokens[2], NULL);
	normal.k = strtod(tokens[3], NULL);

	g_array_append_val(model->vertex_normals, normal);

	return true;
}

static bool handle_parameter_point(obj_model *model, gchar **tokens) {
	size_t n_args = strv_len(tokens) - 1;
	if (n_args < 2 || n_args > 3) {
		fprintf(stderr, "Can't handle 'vp' with %zu args\n",
				n_args);
		return false;
	}

	parameter_point param = { .w = 1.0 };
	param.u = strtod(tokens[1], NULL);
	param.v = strtod(tokens[2], NULL);
	if (n_args > 2)
		param.w = strtod(tokens[3], NULL);

	g_array_append_val(model->parameter_points, param);

	return true;
}

static inline long normalize(long index, long base) {
	return index >= 0 ? index : base + index + 1;
}

static inline bool is_str_empty(const char *str) {
	return str[0] == '\0';
}

static bool handle_faces(obj_model *model, gchar **tokens) {
	size_t n_args = strv_len(tokens) - 1;
	if (n_args < 3) {
		fprintf(stderr, "Can't handle 'f' with %zu args\n",
				n_args);
		return false;
	}

	obj_face face = {
		.points = malloc(sizeof(obj_face_point)*n_args),
		.len = n_args
	};

	bool allow_texture = true, allow_normal = true;

	for (int i = 0; i < n_args; ++i) {
		gchar **vertices = g_strsplit(tokens[i+1], "/", 0);
		size_t n_vertices = strv_len(vertices);

		bool is_geometric = !is_str_empty(vertices[0]);
		bool is_texture = n_vertices > 1 && !is_str_empty(vertices[1]);
		bool is_normal  = n_vertices > 2 && !is_str_empty(vertices[2]);

		if (i == 0) {
			allow_texture = is_texture;
			allow_normal = is_normal;
		}

		if (n_vertices > 3 || !is_geometric ||
				allow_texture != is_texture ||
				allow_normal != is_normal) {
			fprintf(stderr, "Can't handle point in 'f'\n");
			free(face.points);
			g_strfreev(vertices);
			return false;
		}

		face.points[i] = (obj_face_point){ .iv = 0, .ivt = 0, .ivn = 0 };
		face.points[i].iv = normalize(strtol(vertices[0], NULL, 10),
				obj_n_geometric_vertices(model));
		if (is_texture)
			face.points[i].ivt = normalize(strtol(vertices[1], NULL, 10),
					obj_n_texture_vertices(model));
		if (is_normal)
			face.points[i].ivn = normalize(strtol(vertices[2], NULL, 10),
					obj_n_vertex_normals(model));

		g_strfreev(vertices);
	}

	g_array_append_val(model->faces, face);

	return true;
}

static size_t strv_len(gchar **tokens) {
	size_t len = 0;
	for (gchar **token = tokens; *token; ++token)
		++len;
	return len;
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

