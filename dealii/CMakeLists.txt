cmake_minimum_required(VERSION 3.0.0)

message("River growth project configuration started")

find_package(deal.II 8.5.1 REQUIRED)

deal_ii_initialize_cached_variables()

project(river_sim)

add_executable(river_sim main.cpp)

deal_ii_setup_target(river_sim)