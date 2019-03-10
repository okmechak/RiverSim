cmake_minimum_required(VERSION ${min_version})

set(SOURCE_FILES 
    tethex.cpp
    tethex.hpp)

add_library(tethex STATIC ${SOURCE_FILES})