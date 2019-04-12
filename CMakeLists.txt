set(min_version 3.10)
cmake_minimum_required(VERSION ${min_version})

project(riversim C CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Shared libs are preffered over static if ON.")
set(CMAKE_BUILD_TYPE Release)

add_subdirectory(source)
add_subdirectory(doxy)
enable_testing()
add_subdirectory(tests)