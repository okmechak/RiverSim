cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES 
    tethex.cpp
    tethex.hpp)

add_library(tethex STATIC ${SOURCE_FILES})

install(TARGETS tethex DESTINATION rivsim)