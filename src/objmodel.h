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

typedef struct obj_model {
	GArray *geometric_vertices;
	GArray *texture_vertices;
	GArray *vertex_normals;
} obj_model;

#define obj_geometric_vertex(model, i) \
	g_array_index((model)->geometric_vertices, geometric_vertex, i)
static inline size_t obj_n_geometric_vertices(obj_model *model) {
	return model->geometric_vertices->len;
}

#define obj_texture_vertex(model, i) \
	g_array_index((model)->texture_vertices, texture_vertex, i)
static inline size_t obj_n_texture_vertices(obj_model *model) {
	return model->texture_vertices->len;
}

#define obj_vertex_normal(model, i) \
	g_array_index((model)->vertex_normals, vertex_normal, i)
static inline size_t obj_n_vertex_normals(obj_model *model) {
	return model->vertex_normals->len;
}

extern obj_model *obj_model_new();
extern void obj_model_free(obj_model *);

#endif /* OBJUTILS_OBJMODEL_H */

