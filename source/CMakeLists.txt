cmake_minimum_required(VERSION 3.10)

project(riversim C CXX)

message("========================================")
message("Deal II setup")
set(deal.II_USE_STATIC_LIBS "OFF")
find_package(deal.II REQUIRED HINTS ${dealii_DIRECTORY})
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

message("========================================")
message("River, Tethex, Triangle")
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

install(TARGETS riversim DESTINATION ${riversim_INSTALL_BIN_DIR})