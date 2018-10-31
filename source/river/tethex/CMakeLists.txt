cmake_minimum_required(VERSION 3.2)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR})


set(SOURCE_FILES 
    tethex.cpp
    tethex.hpp)

add_library(tethex ${SOURCE_FILES})

install(TARGETS tethex DESTINATION ${riversim_INSTALL_LIB_DIR})
install(FILES tethex.hpp DESTINATION ${riversim_INSTALL_INCLUDE_DIR})