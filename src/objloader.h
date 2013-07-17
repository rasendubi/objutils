#ifndef OBJUTILS_OBJLOADER_H
#define OBJUTILS_OBJLOADER_H

#include "objmodel.h"

/**
 * Reads .obj model from file with given pathname
 * \param path pathname to file to be read
 */
extern obj_model *load_obj(const char *path);

#endif /* OBJUTILS_OBJLOADER_H */
