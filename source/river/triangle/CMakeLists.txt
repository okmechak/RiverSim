cmake_minimum_required(VERSION ${min_version})

set(SOURCE_FILES 
    triangle.c
    triangle.h)

add_library(triangle STATIC ${SOURCE_FILES})