cmake_minimum_required(VERSION 3.10)

project(riversim C CXX)

message("========================================")
message("Deal II setup")
set(deal.II_USE_STATIC_LIBS "ON")
find_package(deal.II REQUIRED HINTS $ENV{DEAL_II_DIR} ${dealii_DIRECTORY})
deal_ii_initialize_cached_variables()


message("========================================")
message("Boost setup")
IF (WIN32)
  # set stuff for windows
  message("WIN")
  set(BOOST_ROOT "$ENV{Boost_DIR}")
  set(Boost_USE_STATIC_LIBS "ON")
  set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
  set(BOOST_LIBRARYDIR "${BOOST_ROOT}/libs")
  #set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
ELSE()
  # set stuff for other systems
  set(BOOST_ROOT "/usr")
  set(Boost_USE_STATIC_LIBS "ON")
  set(BOOST_INCLUDEDIR "/usr/include")
  set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
ENDIF()

find_package(Boost COMPONENTS program_options log)
include_directories(${Boost_INCLUDE_DIRS})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/river/triangle
    ${CMAKE_CURRENT_SOURCE_DIR}/river/tetgen)

set(SOURCE_FILES 
    main.cpp)

add_executable(riversim ${SOURCE_FILES})
deal_ii_setup_target(riversim)

target_link_libraries(riversim river)