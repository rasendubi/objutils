CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_C_FLAGS "-Wall -pedantic ${CMAKE_C_FLAGS} -std=c11")

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIRS})

set(objutils_SRC 
	src/objloader.c
	src/objmodel.c
)
add_library(objutils SHARED ${objutils_SRC})
target_link_libraries(objutils ${GLIB2_LIBRARIES})

set(testobjloader_SRC 
	tests/objloadertest.c
	src/objmodel.c
)
add_executable(testobjloader ${testobjloader_SRC})
target_link_libraries(testobjloader ${GLIB2_LIBRARIES})

set(testload_SRC
	tests/testload.c
)
add_executable(testload ${testload_SRC})
target_link_libraries(testload objutils)

set(objrender_SRC
	src/objrender.c
)
add_executable(objrender ${objrender_SRC})
target_link_libraries(objrender objutils ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

enable_testing()

macro(add_glib_test EXE TEST_NAME)
	add_test(${TEST_NAME} ${EXE} -p ${TEST_NAME})
endmacro(add_glib_test)

add_glib_test(testobjloader /objloader/vertices/geometric)
add_glib_test(testobjloader /objloader/vertices/texture)
add_glib_test(testobjloader /objloader/vertices/normals)
add_glib_test(testobjloader /objloader/vertices/parameters)
add_glib_test(testobjloader /objloader/faces)

