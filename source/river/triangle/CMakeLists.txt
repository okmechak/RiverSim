cmake_minimum_required(VERSION 3.2)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES 
    triangle.c
    triangle.h)


add_library(triangle STATIC ${SOURCE_FILES})