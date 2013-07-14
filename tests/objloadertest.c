#include "../src/objloader.c"
#include <stdio.h>

int main(void) {
	obj_model *model = load_obj("rapide.obj");
	if (model)
		obj_model_free(model);
}

