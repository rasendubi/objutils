CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(objutils_SRC src/objloader.c)

add_library(objutils SHARED ${objutils_SRC})

