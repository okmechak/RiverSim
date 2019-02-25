cmake_minimum_required(VERSION 3.2)

add_subdirectory(tethex)
add_subdirectory(triangle)

include_directories( #FIXME: why I should include in every cmakelist file
    ${CMAKE_CURRENT_SOURCE_DIR}/tethex
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle)

set(SOURCE_FILES 
    riversim.cpp
    riversim.hpp
    common.hpp
    common.cpp
    geometry.hpp
    geometry.cpp
    mesh.hpp
    mesh.cpp
    solver.hpp
    solver.cpp
    utilities.hpp
    utilities.cpp)

add_library(river STATIC ${SOURCE_FILES})
set_property(TARGET river PROPERTY CXX_STANDARD 17)
deal_ii_setup_target(river)

target_link_libraries(river ${Boost_LIBRARIES})
target_link_libraries(river tethex triangle)