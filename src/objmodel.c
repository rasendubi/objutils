#include "objmodel.h"

#include <stdlib.h>

obj_model *obj_model_new() {
	obj_model *model = malloc(sizeof(obj_model));
	model->geometric_vertices = g_array_new(FALSE, FALSE,
			sizeof(geometric_vertex));
	model->texture_vertices = g_array_new(FALSE, FALSE,
			sizeof(texture_vertex));
	model->vertex_normals = g_array_new(FALSE, FALSE,
			sizeof(vertex_normal));
	model->parameter_points = g_array_new(FALSE, FALSE,
			sizeof(parameter_point));
	return model;
}

void obj_model_free(obj_model *model) {
	g_array_free(model->geometric_vertices, TRUE);
	g_array_free(model->texture_vertices, TRUE);
	g_array_free(model->vertex_normals, TRUE);
	g_array_free(model->parameter_points, TRUE);
	free(model);
}

