#include "../src/objloader.c"
#include <stdio.h>

void assert_vertex_eq(const geometric_vertex v1, const geometric_vertex v2) {
	g_assert_cmpfloat(v1.x, ==, v2.x);
	g_assert_cmpfloat(v1.y, ==, v2.y);
	g_assert_cmpfloat(v1.z, ==, v2.z);
	g_assert_cmpfloat(v1.w, ==, v2.w);
}

void assert_texture_vertex_eq(const texture_vertex v1,
		const texture_vertex v2) {
	g_assert_cmpfloat(v1.u, ==, v2.u);
	g_assert_cmpfloat(v1.v, ==, v2.v);
	g_assert_cmpfloat(v1.w, ==, v2.w);

}

void assert_vertex_normal_eq(const vertex_normal v1,
		const vertex_normal v2) {
	g_assert_cmpfloat(v1.i, ==, v2.i);
	g_assert_cmpfloat(v1.j, ==, v2.j);
	g_assert_cmpfloat(v1.k, ==, v2.k);
}

void assert_parse(obj_model *model,
		char *to_parse[], int n_to_parse) {
	for (int i = 0; i < n_to_parse; ++i) {
		g_assert(parse_line(model, to_parse[i]));
	}
}

void test_geometric_vertices(void) {
	char *test_data[] = {
		"v 1.02 3 5.06",
		" v -1.02  4.15 7.20 10.5",
		" \t v \t 2.01 5.06   -17.3 14\t  ",
		"v  2e-05 3.4e15  5.0e-007"
	};
	geometric_vertex answer[] = {
		{ 1.02, 3, 5.06, 1.0 },
		{ -1.02, 4.15, 7.20, 10.5 },
		{ 2.01, 5.06, -17.3, 14 },
		{ 2e-5, 3.4e15, 5.0e-7, 1.0 }
	};
	const unsigned int N = sizeof(test_data)/sizeof(test_data[0]);

	obj_model *model = obj_model_new();

	assert_parse(model, test_data, N);

	g_assert(!parse_line(model, "v 1 2"));
	g_assert(!parse_line(model, "v 1 2 3 4 5"));

	g_assert_cmpuint(obj_n_geometric_vertices(model), ==, N);

	for (int i = 0; i < N; ++i) {
		assert_vertex_eq(obj_geometric_vertex(model, i), answer[i]);
	}

	obj_model_free(model);
}

void test_texture_vertices(void) {
	char *test_data[] = {
		"vt 1.0 4.4 5.0",
		" vt  0.0  \t 1.6e-30\t ",
		"vt 0.1"
	};
	texture_vertex answer[] = {
		{ 1.0, 4.4, 5.0 },
		{ 0.0, 1.6e-30, 0 },
		{ 0.1, 0, 0 }
	};
	const unsigned int N = sizeof(test_data)/sizeof(test_data[0]);

	obj_model *model = obj_model_new();

	assert_parse(model, test_data, N);

	g_assert(!parse_line(model, "vt  "));
	g_assert(!parse_line(model, "vt 1 2 3 4"));

	g_assert_cmpuint(obj_n_texture_vertices(model), ==, N);

	for (int i = 0; i < N; ++i) {
		assert_texture_vertex_eq(obj_texture_vertex(model, i),
				answer[i]);
	}

	obj_model_free(model);
}

void test_vertex_normals(void) {
	char *test_data[] = {
		"  vn\t 1.2 3.4 5.6",
		"vn  4 5 -6e-017"
	};
	vertex_normal answer[] = {
		{ 1.2, 3.4, 5.6 },
		{ 4, 5, -6e-017 }
	};
	const unsigned int N = sizeof(test_data)/sizeof(test_data[0]);

	obj_model *model = obj_model_new();

	assert_parse(model, test_data, N);

	g_assert(!parse_line(model, "vn 1 2.3"));
	g_assert(!parse_line(model, "vn 1"));
	g_assert(!parse_line(model, "vn"));
	g_assert(!parse_line(model, "vn 4 5 6 7"));

	g_assert_cmpuint(obj_n_vertex_normals(model), ==, N);

	for (int i = 0; i < N; ++i) {
		assert_vertex_normal_eq(obj_vertex_normal(model, i),
				answer[i]);
	}

	obj_model_free(model);
}

int main(int argc, char *argv[]) {
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/objloader/vertices/geometric",
			test_geometric_vertices);
	g_test_add_func("/objloader/vertices/texture",
			test_texture_vertices);
	g_test_add_func("/objloader/vertices/normals",
			test_vertex_normals);
	return g_test_run();
}

