#include "objmodel.h"

#include <stdlib.h>

obj_model *obj_model_new() {
	obj_model *model = malloc(sizeof(obj_model));
	model->vertices = g_array_new(FALSE, FALSE, sizeof(vertex));
	return model;
}

void obj_model_free(obj_model *model) {
	g_array_free(model->vertices, TRUE);
	free(model);
}

