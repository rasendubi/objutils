#include "../src/objloader.c"

#define array_size(arr) (sizeof(arr)/sizeof((arr)[0]))

/**
 * Asserts, that all strings are consumed by parse_line
 * \param model obj_model*
 * \param tests char *[] should be statically sized array
 */
#define assert_parse(model, tests) \
	__assert_parse(model, tests, array_size(tests))

void __assert_parse(obj_model *model,
		char *tests[], int n_tests) {
	for (int i = 0; i < n_tests; ++i) {
		g_assert(parse_line(model, tests[i]));
	}
}

/**
 * Asserts that all strings are rejected by parse_line
 * \param model obj_model*
 * \param tests char *[] should be statically sized array
 */
#define assert_parse_fail(model, tests) \
	__assert_parse_fail(model, tests, array_size(tests))

void __assert_parse_fail(obj_model *model,
		char *tests[], int n_tests) {
	for (int i = 0; i < n_tests; ++i) {
		g_assert(!parse_line(model, tests[i]));
	}
}

void assert_geometric_vertex_equal(const geometric_vertex v1,
		const geometric_vertex v2) {
	g_assert_cmpfloat(v1.x, ==, v2.x);
	g_assert_cmpfloat(v1.y, ==, v2.y);
	g_assert_cmpfloat(v1.z, ==, v2.z);
	g_assert_cmpfloat(v1.w, ==, v2.w);
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

	char *bad_data[] = {
		"v 1 2",
		"v 1 2 3 4 5"
	};

	obj_model *model = obj_model_new();

	assert_parse(model, test_data);
	assert_parse_fail(model, bad_data);

	g_assert_cmpuint(obj_n_geometric_vertices(model), ==,
			array_size(answer));

	for (int i = 0; i < array_size(answer); ++i) {
		assert_geometric_vertex_equal(obj_geometric_vertex(model, i+1),
				answer[i]);
	}

	obj_model_free(model);
}

void assert_texture_vertex_equal(const texture_vertex v1,
		const texture_vertex v2) {
	g_assert_cmpfloat(v1.u, ==, v2.u);
	g_assert_cmpfloat(v1.v, ==, v2.v);
	g_assert_cmpfloat(v1.w, ==, v2.w);

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

	char *bad_data[] = {
		"vt  ",
		"vt 1 2 3 4"
	};

	obj_model *model = obj_model_new();

	assert_parse(model, test_data);
	assert_parse_fail(model, bad_data);

	g_assert_cmpuint(obj_n_texture_vertices(model), ==,
			array_size(answer));
	for (int i = 0; i < array_size(answer); ++i) {
		assert_texture_vertex_equal(obj_texture_vertex(model, i+1),
				answer[i]);
	}

	obj_model_free(model);
}

void assert_vertex_normal_equal(const vertex_normal v1,
		const vertex_normal v2) {
	g_assert_cmpfloat(v1.i, ==, v2.i);
	g_assert_cmpfloat(v1.j, ==, v2.j);
	g_assert_cmpfloat(v1.k, ==, v2.k);
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

	char *bad_data[] = {
		"vn 1 2.3",
		"vn 1",
		"vn",
		"vn 4 5 6 7"
	};

	obj_model *model = obj_model_new();

	assert_parse(model, test_data);
	assert_parse_fail(model, bad_data);

	g_assert_cmpuint(obj_n_vertex_normals(model), ==,
			array_size(answer));
	for (int i = 0; i < array_size(answer); ++i) {
		assert_vertex_normal_equal(obj_vertex_normal(model, i+1),
				answer[i]);
	}

	obj_model_free(model);
}

void assert_parameter_point_equal(const parameter_point p1,
		const parameter_point p2) {
	g_assert_cmpfloat(p1.u, ==, p2.u);
	g_assert_cmpfloat(p1.v, ==, p2.v);
	g_assert_cmpfloat(p1.w, ==, p2.w);
}

void test_parameter_points(void) {
	char *test_data[] = {
		"vp 1.2 3 4",
		"vp  2  -4e-14"
	};
	parameter_point answer[] = {
		{ 1.2, 3, 4 },
		{ 2, -4e-14, 1.0}
	};

	char *bad_data[] = {
		"vp",
		"vp 2",
		"vp 3 4 5 6",
	};

	obj_model *model = obj_model_new();

	assert_parse(model, test_data);
	assert_parse_fail(model, bad_data);

	g_assert_cmpuint(obj_n_parameter_points(model), ==,
			array_size(answer));
	for (int i = 0; i < array_size(answer); ++i) {
		assert_parameter_point_equal(obj_parameter_point(model, i+1),
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
	g_test_add_func("/objloader/vertices/parameters",
			test_parameter_points);
	return g_test_run();
}

