cmake_minimum_required(VERSION 3.16)

project(riversim 
    VERSION 2.8.7 
    DESCRIPTION "Simulation of river growth using model based on Laplace equation"
    HOMEPAGE_URL "https://okmechak.github.io/RiverSim/"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Shared libs are preffered over static if ON.")

# Set a default build type if none was specified
set(DEFAULT_BUILD_TYPE "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release")
endif()

set(DEALII_REL_DIR "/mnt/c/users/ofcra/lib/dealii_shared" CACHE STRING "Deal.II release SDK directory location.") 
set(DEALII_DEB_DIR "/mnt/c/users/ofcra/lib/dealii_d_shared" CACHE STRING "Deal.II debug SDK directory location.") 

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("=============pedantic build=============")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g3")
    set(DEALII_DIR ${DEALII_DEB_DIR})
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    message("============= speed optimized without compilation errors build =============")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w")
    set(DEALII_DIR ${DEALII_REL_DIR})
endif()

add_subdirectory(source)
add_subdirectory(docsrc)
add_subdirectory(scripts)
enable_testing()
add_subdirectory(tests)

install(
    DIRECTORY 
        sample_trees
    DESTINATION 
        riversim)

install(
    DIRECTORY 
        examples
    DESTINATION 
        riversim)

install(
    FILES
        examples/python_api.ipynb
    DESTINATION 
        riversim)