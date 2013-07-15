#include "objmodel.h"

#include <stdlib.h>

size_t obj_n_geometric_vertices(obj_model *model) {
	return model->geometric_vertices->len;
}

size_t obj_n_texture_vertices(obj_model *model) {
	return model->texture_vertices->len;
}

obj_model *obj_model_new() {
	obj_model *model = malloc(sizeof(obj_model));
	model->geometric_vertices = g_array_new(FALSE, FALSE,
			sizeof(geometric_vertex));
	model->texture_vertices = g_array_new(FALSE, FALSE,
			sizeof(texture_vertex));
	return model;
}

void obj_model_free(obj_model *model) {
	g_array_free(model->geometric_vertices, TRUE);
	g_array_free(model->texture_vertices, TRUE);
	free(model);
}

