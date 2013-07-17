#include "../src/objloader.c"

#define array_size(arr) (sizeof(arr)/sizeof((arr)[0]))
#define assertion_message(...) \
	g_assertion_message_expr(G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
			g_strjoin("", __VA_ARGS__, NULL) )

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
		char *test = g_strdup(tests[i]);
		bool good = parse_line(model, test);
		if (!good) {
			assertion_message("Failed to parse line: \"",
					tests[i], "\"");
		}
		free(test);
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
		char *test = g_strdup(tests[i]);
		g_assert(!parse_line(model, test));
		free(test);
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

void assert_face_equal(const obj_face f1, const obj_face f2) {
	g_assert_cmpuint(f1.len, ==, f2.len);
	for (int i = 0; i < f1.len; ++i) {
		g_assert_cmpuint(f1.points[i].iv, ==,
				f2.points[i].iv);
		g_assert_cmpuint(f1.points[i].ivt, ==,
				f2.points[i].ivt);
		g_assert_cmpuint(f1.points[i].ivn, ==,
				f2.points[i].ivn);
	}
}

void test_faces(void) {
	char *test_data[] = {
		"v 1 2 0",
		"v 2 1 0",
		"v 2 -1 0",
		"v 1 -2 0",
		"v -1 -2 0",
		"v -2 -1 0",
		"v -2 1 0",
		"v -1 2 0",

		"vt 1",
		"vt 2",
		"vt 3",
		"vt 4",
		"vt 5",
		"vt 6",
		"vt 7",
		"vt 8",

		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",
		"vn 0 0 1",

		"f 1 2 5",
		"f 1 3 -4 -2",
		"f 1 -7 3 -5 5 -3 7 8",

		"f 1/1 2/-7 5/-4",
		"f 1/-8 3/3 -4/5 -2/-2",
		"f 1/8 -7/7 3/-3 -5/-4 5/-5 -3/-6 7/2 8/-8",

		"f 1/1/8 2/-7/-2 5/-4/4",
		"f 1/-8/8 3/3/-3 -4/5/-5 -2/-2/-7",
		"f 1/8/-8 -7/7/2 3/-3/3 -5/-4/4 5/-5/-4 -3/-6/6 7/2/7 8/-8/8",

		"f 1//8 2//-2 5//4",
		"f 1//8 3//-3 -4//-5 -2//-7",
		"f 1//-8 -7//2 3//3 -5//4 5//-4 -3//6 7//7 8//8"
	};
	obj_face answer[] = {
		{ (obj_face_point[]){{1}, {2}, {5}}, 3 },
		{ (obj_face_point[]){{1}, {3}, {5}, {7}}, 4 },
		{ (obj_face_point[]){{1}, {2}, {3}, {4},
					    {5}, {6}, {7}, {8}}, 8 },

		{ (obj_face_point[]){{1,1}, {2,2}, {5,5}}, 3 },
		{ (obj_face_point[]){{1,1}, {3,3}, {5,5}, {7,7}}, 4 },
		{ (obj_face_point[]){{1,8}, {2,7}, {3,6}, {4,5}, {5,4},
						{6,3}, {7,2}, {8,1}}, 8 },

		{ (obj_face_point[]){{1,1,8}, {2,2,7}, {5,5,4}}, 3 },
		{ (obj_face_point[]){{1,1,8}, {3,3,6}, {5,5,4}, {7,7,2}}, 4 },
		{ (obj_face_point[]){{1,8,1}, {2,7,2}, {3,6,3}, {4,5,4}, {5,4,5},
						{6,3,6}, {7,2,7}, {8,1,8}}, 8 },

		{ (obj_face_point[]){{1,0,8}, {2,0,7}, {5,0,4}}, 3 },
		{ (obj_face_point[]){{1,0,8}, {3,0,6}, {5,0,4}, {7,0,2}}, 4 },
		{ (obj_face_point[]){{1,0,1}, {2,0,2}, {3,0,3}, {4,0,4}, {5,0,5},
						{6,0,6}, {7,0,7}, {8,0,8}}, 8 },
	};

	char *bad_data[] = {
		"f 1 2",
		"f 1 2 3/4",
		"f 1/// 2 3",
		"f 1/1/1/1 2 3",
		"f /1/2 2 3"
	};

	obj_model *model = obj_model_new();

	assert_parse(model, test_data);
	assert_parse_fail(model, bad_data);

	g_assert_cmpuint(obj_n_faces(model), ==, array_size(answer));

	for (int i = 0; i < array_size(answer); ++i) {
		assert_face_equal(obj_face(model, i), answer[i]);
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
	g_test_add_func("/objloader/faces",
			test_faces);
	return g_test_run();
}

