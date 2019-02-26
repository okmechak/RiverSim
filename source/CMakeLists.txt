cmake_minimum_required(VERSION 3.10)

project(riversim C CXX)

message("========================================")
message("Deal II setup")
set(deal.II_USE_STATIC_LIBS "ON")
find_package(deal.II REQUIRED HINTS $ENV{DEAL_II_DIR} ${dealii_DIRECTORY})
deal_ii_initialize_cached_variables()


message("========================================")
message("GMSH setup")
include_directories(${gmsh_DIRECTORY}/include)
link_directories(${gmsh_DIRECTORY}/lib)
message("========================================")
message("River, Tethex, Triangle setup")
add_subdirectory(river)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/river
    ${CMAKE_CURRENT_SOURCE_DIR}/river/tethex
    ${CMAKE_CURRENT_SOURCE_DIR}/river/triangle)

set(SOURCE_FILES 
    main.cpp)

add_executable(riversim ${SOURCE_FILES})
deal_ii_setup_target(riversim)

target_link_libraries(riversim river)