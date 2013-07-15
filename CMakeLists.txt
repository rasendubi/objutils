CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -std=c11)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})

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

