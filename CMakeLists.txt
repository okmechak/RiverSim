message("===============================")
message("======river simulation=========")
message("===============================")

#setting name of project and target
set(TARGET "river")

cmake_minimum_required(VERSION 3.0.0)

find_package(deal.II REQUIRED) 
if(NOT ${deal.II_FOUND})
    message(FATAL_ERROR "\n Could not find DEALII")
endif()

message(STATUS "Found deal.II version ${DEAL_II_PACKAGE_VERSION} at '${deal.II_DIR}'")


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(EXISTS ${CMAKE_SOURCE_DIR}/doc/CMakeLists.txt)
    set(RIVER_HAVE_DOC_DIR TRUE)
endif()


deal_ii_initialize_cached_variables()
project(${target} CXX C)

message("===============================")

#configure boost
if(NOT "${BOOST_DIR}" STREQUAL "")
    set(BOOST_ROOT "${BOOST_DIR}")
endif()

if(DEAL_II_STATIC_EXECUTABLE STREQUAL "ON")
    set(Boost_USE_STATIC_LIBS "ON")
endif()


if(DEAL_II_WITH_THREADS STREQUAL "ON")
    set(Bost_USE_MULTITHREADED TRUE)
endif()

find_package(Boost COMPONENTS filesystem iostreams serialization system thread)
if(NOT Boost_FOUND AND Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS FALSE)
    find_package(Boost COMPONENTS filesystem iostreams serialization system thread)
endif()

message("===============================")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
    message(STATUS "Install river to deafult: ${CMAKE_INSTALL_PREFIX}")
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

ADD_SUBDIRECTORY(include)

file(GLOB RIVER_SRC
    "source/*.cpp")

include_directories(
    ${CMAKE_BINARY_DIR}/include/
    ${CMAKE_SOURCE_DIR}/include/
    ${Boost_INCLUDE_DIRS}   
)