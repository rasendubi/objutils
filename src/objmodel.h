#ifndef OBJUTILS_OBJMODEL_H
#define OBJUTILS_OBJMODEL_H

#include <glib.h>

typedef struct geometric_vertex {
	double x, y, z, w;
} geometric_vertex;

typedef struct obj_model {
	GArray *geometric_vertices;
} obj_model;

#define obj_geometric_vertex(model, i) \
	g_array_index((model)->geometric_vertices, geometric_vertex, i)
extern size_t obj_n_geometric_vertices(obj_model *);

extern obj_model *obj_model_new();
extern void obj_model_free(obj_model *);

#endif /* OBJUTILS_OBJMODEL_H */

