cmake_minimum_required(VERSION 2.8.12)

set(DEAL_II_PREFER_STATIC_LIBS ON CACHE BOOL "Static linkage of Deal.II library will be prefferable.")

message("========================================")
message("Deal II setup")

find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
deal_ii_initialize_cached_variables()

mark_as_advanced(deal.II_DIR DEALII_DIR DEAL_II_PREFER_STATIC_LIBS)

message("========================================")
message("riversim modules setup")
add_subdirectory(river)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/river)

    
if(NOT BUILD_SHARED_LIBS )
    message(FATAL_ERROR "Python API can't be build with static libraries, make shure You specified BUILD_SHARED_LIBS = true in cmake options" )
endif()
message("========================================")
message("Python API setup")
# Find python and Boost - both are required dependencies
find_package(PythonLibs 3.8 REQUIRED)
find_package(Boost COMPONENTS python38 REQUIRED)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(riversimPY SHARED pythonapi.cpp)
deal_ii_setup_target(riversimPY)

# Set up the libraries and header search paths for this target
target_link_libraries(riversimPY io)
target_link_libraries(riversimPY ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_include_directories(riversimPY PRIVATE ${PYTHON_INCLUDE_DIRS})
set_target_properties(riversimPY
    PROPERTIES OUTPUT_NAME riversim)