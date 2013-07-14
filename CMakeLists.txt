CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -std=c11)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(GIO REQUIRED gio-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${GIO_INCLUDE_DIRS})

set(objutils_SRC src/objloader.c)
set(testobjloader_SRC tests/objloadertest.c)

add_library(objutils SHARED ${objutils_SRC})
target_link_libraries(objutils ${GIO_LIBRARIES} ${GLIB2_LIBRARIES})

add_executable(testobjloader ${testobjloader_SRC})
target_link_libraries(testobjloader ${GIO_LIBRARIES} ${GLIB2_LIBRARIES})

