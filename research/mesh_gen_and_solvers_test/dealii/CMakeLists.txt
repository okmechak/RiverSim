cmake_minimum_required(VERSION 3.10)

project(research C CXX)

set(CMAKE_BUILD_TYPE "Release")

message("========================================")
message("Deal II setup")
set(deal.II_USE_STATIC_LIBS "OFF")
find_package(deal.II REQUIRED HINTS /usr/local/dealii)
deal_ii_initialize_cached_variables()

set(SOURCE_FILES main.cpp)

add_executable(research main.cpp)
deal_ii_setup_target(research)