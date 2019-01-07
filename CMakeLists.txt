cmake_minimum_required(VERSION 3.10)


set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
set(riversim_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(riversim_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX})
set(riversim_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(gmsh_DIRECTORY /usr/local/gmsh)
set(dealii_DIRECTORY /usr/local/dealii)

enable_testing()

#add_subdirectory(docs)
add_subdirectory(tests)
add_subdirectory(source)