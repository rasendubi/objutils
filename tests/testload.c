#include <stdio.h>
#include "../src/objloader.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Exactly one argument expected\n");
		return 1;
	}

	obj_model *model = load_obj(argv[1]);
	puts("Done\n");
	obj_model_free(model);
}
