cmake_minimum_required(VERSION 2.8.12)

project(riversim C CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Shared libs are preffered over static if ON.")

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

set(DEALII_DIR "/usr/local" CACHE STRING "Deal.II release SDK directory location.") 

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("=============pedantic build=============")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g3 -fPIC")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    message("============= speed optimized without compilation errors build =============")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -w -fPIC")
endif()

add_subdirectory(source)
enable_testing()
add_subdirectory(tests)