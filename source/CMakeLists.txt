cmake_minimum_required(VERSION 2.8.12)

set(DEAL_II_PREFER_STATIC_LIBS ON CACHE BOOL "Static linkage of Deal.II library will be prefferable.")

message("========================================")
message("Deal II setup")

find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
deal_ii_initialize_cached_variables()

mark_as_advanced(deal.II_DIR DEALII_DIR DEAL_II_PREFER_STATIC_LIBS)

message("========================================")
message("riversim modules setup")
#point
add_library(point STATIC point.hpp point.cpp) 
install(TARGETS point DESTINATION riversim)

#boundary
add_library(boundary STATIC boundary.hpp boundary.cpp) 
target_link_libraries(boundary point)
install(TARGETS boundary DESTINATION riversim)

#rivers
add_library(rivers STATIC rivers.hpp rivers.cpp) 
target_link_libraries(rivers boundary)
install(TARGETS rivers DESTINATION riversim)

#region
add_library(region STATIC region.hpp region.cpp) 
target_link_libraries(region rivers)
install(TARGETS region DESTINATION riversim)

#tethex
add_library(tethex STATIC tethex.hpp tethex.cpp) 
target_link_libraries(tethex boundary)
install(TARGETS tethex DESTINATION riversim)

#triangle_c
add_library(triangle_c STATIC triangle_c.hpp triangle_c.cpp) 
target_link_libraries(triangle_c boundary)
install(TARGETS triangle_c DESTINATION riversim)

#triangle
add_library(triangle STATIC triangle.hpp triangle.cpp) 
target_link_libraries(triangle tethex triangle_c)
install(TARGETS triangle DESTINATION riversim)

#solver
add_library(solver STATIC solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver tethex boundary)
install(TARGETS solver DESTINATION riversim)

#model
add_library(model STATIC model.hpp model.cpp) 
target_link_libraries(model rivers solver region triangle)
install(TARGETS model DESTINATION riversim)

#io
add_library(io STATIC io.hpp io.cpp json.hpp)
target_link_libraries(io model)
install(TARGETS io DESTINATION riversim)

    
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