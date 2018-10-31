cmake_minimum_required(VERSION 3.2)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES 
    triangle.c
    triangle.h)


add_library(triangle ${SOURCE_FILES})

install(TARGETS triangle DESTINATION ${riversim_INSTALL_LIB_DIR})
install(FILES triangle.h DESTINATION ${riversim_INSTALL_INCLUDE_DIR})