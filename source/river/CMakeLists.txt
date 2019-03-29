cmake_minimum_required(VERSION ${min_version})

add_subdirectory(tethex)
#FIXME:
#add_subdirectory(triangle)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tethex)
    #${CMAKE_CURRENT_SOURCE_DIR}/triangle)FIXME

set(SOURCE_FILES
    triangle.hpp#FIXME
    triangle.cpp
    riversim.cpp
    riversim.hpp
    common.hpp
    common.cpp
    geometry.hpp
    geometry.cpp
    geometrynew.hpp
    geometrynew.cpp
    mesh.hpp
    mesh.cpp
    solver.hpp
    solver.cpp
    utilities.hpp
    utilities.cpp
    physmodel.hpp
    physmodel.cpp
    border.hpp
    border.cpp
    cxxopts.hpp)

add_library(river STATIC ${SOURCE_FILES})
deal_ii_setup_target(river)

#FIXME
target_link_libraries(river tethex ${GMSH_LIB}) #triangle 