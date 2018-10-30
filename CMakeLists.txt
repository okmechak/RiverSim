cmake_minimum_required(VERSION 3.0.0)
project(riversim C CXX)

message("========================================")
message("Deal II setup")
set(deal.II_USE_STATIC_LIBS "OFF")
find_package(deal.II REQUIRED HINTS /usr/local/dealii)
deal_ii_initialize_cached_variables()


message("========================================")
message("Boost setup")
set(BOOST_ROOT "/usr")
set(Boost_USE_STATIC_LIBS "ON")
set(BOOST_INCLUDEDIR "/usr/include")
set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
find_package(Boost COMPONENTS program_options log REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(source)