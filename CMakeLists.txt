set(min_version 3.10)
cmake_minimum_required(VERSION ${min_version})

project(riversim C CXX)


add_subdirectory(source)
add_subdirectory(docs)
enable_testing()
add_subdirectory(tests)