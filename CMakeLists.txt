set(min_version 3.8)
cmake_minimum_required(VERSION ${min_version})

project(riversim C CXX)

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

if(CMAKE_BUILD_TYPE MATCHES Debug)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
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