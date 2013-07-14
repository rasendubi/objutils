#ifndef OBJUTILS_OBJMODEL_H
#define OBJUTILS_OBJMODEL_H

#include <glib.h>

typedef struct vertex {
	double x, y, z, w;
} vertex;

typedef struct obj_model {
	GArray *vertices;
} obj_model;

extern obj_model *obj_model_new();
extern void obj_model_free(obj_model *);

#endif /* OBJUTILS_OBJMODEL_H */
