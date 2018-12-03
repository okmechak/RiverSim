cmake_minimum_required(VERSION 3.2)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})


set(SOURCE_FILES 
    tethex.cpp
    tethex.hpp)

add_library(tethex STATIC ${SOURCE_FILES})