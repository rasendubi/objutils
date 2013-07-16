#ifndef OBJUTILS_OBJMODEL_H
#define OBJUTILS_OBJMODEL_H

#include <glib.h>

typedef struct geometric_vertex {
	double x, y, z, w;
} geometric_vertex;

typedef struct texture_vertex {
	double u, v, w;
} texture_vertex;

typedef struct vertex_normal {
	double i, j, k;
} vertex_normal;

typedef struct parameter_point {
	double u, v, w;
} parameter_point;

typedef struct obj_face_point {
	long iv, ivt, ivn;
} obj_face_point;

typedef struct obj_face {
	obj_face_point *points;
	size_t len;
} obj_face;

typedef struct obj_model {
	GArray *geometric_vertices;
	GArray *texture_vertices;
	GArray *vertex_normals;
	GArray *parameter_points;
	GArray *faces;
} obj_model;

#define obj_geometric_vertex(model, i) \
	g_array_index((model)->geometric_vertices, geometric_vertex, (i) - 1)
static inline size_t obj_n_geometric_vertices(obj_model *model) {
	return model->geometric_vertices->len;
}

#define obj_texture_vertex(model, i) \
	g_array_index((model)->texture_vertices, texture_vertex, (i) - 1)
static inline size_t obj_n_texture_vertices(obj_model *model) {
	return model->texture_vertices->len;
}

#define obj_vertex_normal(model, i) \
	g_array_index((model)->vertex_normals, vertex_normal, (i) - 1)
static inline size_t obj_n_vertex_normals(obj_model *model) {
	return model->vertex_normals->len;
}

#define obj_parameter_point(model, i) \
	g_array_index((model)->parameter_points, parameter_point, (i) - 1)
static inline size_t obj_n_parameter_points(obj_model *model) {
	return model->parameter_points->len;
}

#define obj_face(model, i) \
	g_array_index((model)->faces, obj_face, i)
static inline size_t obj_n_faces(obj_model *model) {
	return model->faces->len;
}

extern obj_model *obj_model_new();
extern void obj_model_free(obj_model *);

#endif /* OBJUTILS_OBJMODEL_H */

