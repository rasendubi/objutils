#include "objloader.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>

typedef struct strings_array {
	gchar **strings;
	unsigned count, capacity;
} strings_array;
static inline strings_array *strings_array_new() {
	const unsigned initial_capacity = 8;
	strings_array *arr = malloc(sizeof(strings_array));
	arr->strings = malloc(sizeof(gchar**)*initial_capacity);
	arr->capacity = initial_capacity;
	arr->count = 0;
	return arr;
}
static inline void strings_array_add(strings_array *array, gchar *str) {
	if (array->count == array->capacity) {
		array->capacity *= 2;
		array->strings = realloc(array->strings,
				array->capacity*sizeof(gchar**));
	}
	array->strings[array->count++] = str;
}
static inline void strings_array_free(strings_array *arr) {
	free(arr->strings);
	free(arr);
}

static bool parse_line(obj_model *model, gchar *line);

static bool handle_geometric_vertex(obj_model *, gchar **, size_t);
static bool handle_texture_vertex(obj_model *, gchar **, size_t);
static bool handle_vertex_normal(obj_model *, gchar **, size_t);
static bool handle_parameter_point(obj_model *, gchar **, size_t);

static bool handle_faces(obj_model *, gchar **, size_t);

/**
 * Splits string on spaces and tabs in-place
 * \param line line to split
 * \return returns pointer to dynamically allocated strings_array,
 *		have to be freed with strings_array_free
 */
static strings_array *split_spaces_inplace(gchar *line);

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

	for (gchar **curline = lines; *curline; ++curline) {
		parse_line(model, *curline);
	}

	g_strfreev(lines);

	return model;
}

static bool parse_line(obj_model *model, gchar *line) {
	static const struct {
		char *command;
		bool (*handler)(obj_model *, gchar **, size_t);
	} handlers[] = {
		{ "v",  handle_geometric_vertex },
		{ "vt", handle_texture_vertex },
		{ "vn", handle_vertex_normal },
		{ "vp", handle_parameter_point },
		{ "f",  handle_faces }
	};
	static const size_t n_handlers = sizeof(handlers)/sizeof(handlers[0]);

	strings_array *token_array = split_spaces_inplace(line);
	gchar **tokens = token_array->strings;
	size_t n_args = token_array->count - 1;

	bool ret = false;

	gchar *command = tokens[0];

	if (!command || command[0] == '#')
		ret = true;
	else {
		for (size_t i = 0; i < n_handlers; ++i)
			if (strcmp(handlers[i].command, command) == 0) {
				ret = handlers[i].handler(model, tokens, n_args);
				break;
			}
	}

	strings_array_free(token_array);
	return ret;
}

static bool handle_geometric_vertex(obj_model *model, gchar **tokens,
		size_t n_args) {
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

static bool handle_texture_vertex(obj_model *model, gchar **tokens,
		size_t n_args) {
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

static bool handle_vertex_normal(obj_model *model, gchar **tokens,
		size_t n_args) {
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

static bool handle_parameter_point(obj_model *model, gchar **tokens,
		size_t n_args) {
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

static bool handle_faces(obj_model *model, gchar **tokens,
		size_t n_args) {
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
		size_t n_vertices = g_strv_length(vertices);

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

static strings_array *split_spaces_inplace(gchar *line) {
	strings_array *result = strings_array_new();
	for (gchar *c = line; *c;) {
		while (*c == ' ' || *c == '\t')
			++c;
		if (*c) {
			strings_array_add(result, c);
			do {
				++c;
			} while (*c && *c != ' ' && *c != '\t');
			if (*c == '\0') break;
			*c = '\0';
			++c;
		}
	}
	return result;
}

