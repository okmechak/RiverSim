cmake_minimum_required(VERSION ${min_version})

set(SOURCE_FILES 
    triangle.cpp
    triangle.hpp)

add_library(triangle STATIC ${SOURCE_FILES})