#include "../src/objloader.c"
#include <stdio.h>

void assert_vertex_eq(const vertex v1, const vertex v2) {
	g_assert_cmpfloat(v1.x, ==, v2.x);
	g_assert_cmpfloat(v1.y, ==, v2.y);
	g_assert_cmpfloat(v1.z, ==, v2.z);
	g_assert_cmpfloat(v1.w, ==, v2.w);
}

void test_geometric_vertices(void) {
	char *test_data[] = {
		"v 1.02 3 5.06",
		"v -1.02  4.15 7.20 10.5",
		" \t v \t 2.01 5.06   -17.3 14\t  "
	};
	vertex answer[] = {
		{ 1.02, 3, 5.06, 1.0 },
		{ -1.02, 4.15, 7.20, 10.5 },
		{ 2.01, 5.06, -17.3, 14 }
	};
	const unsigned int N = sizeof(test_data)/sizeof(test_data[0]);

	obj_model *model = obj_model_new();

	for (int i = 0; i < N; ++i) {
		g_assert(parse_line(model, test_data[i]));
	}

	g_assert(!parse_line(model, "v 1 2"));
	g_assert(!parse_line(model, "v 1 2 3 4 5"));

	g_assert_cmpuint(obj_n_geometric_vertices(model), ==, N);

	for (int i = 0; i < N; ++i) {
		assert_vertex_eq(obj_geometric_vertex(model, i), answer[i]);
	}

	obj_model_free(model);
}

int main(int argc, char *argv[]) {
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/objloader/vertices/geometric",
			test_geometric_vertices);
	return g_test_run();
}

