cmake_minimum_required(VERSION 3.2)

set(${CMAKE_BUILD_TYPE} "Debug")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
set(riversim_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(riversim_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX})
set(riversim_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(tetgen_DIRECTORY /usr/local/tetgen)
set(gmsh_DIRECTORY /usr/local/gmsh)

include_directories(
    ${gmsh_DIRECTORY}/include
    ${tetgen_DIRECTORY}
)

link_directories(
    ${tetgen_DIRECTORY}
    ${gmsh_DIRECTORY}/lib
    )


add_subdirectory(source)