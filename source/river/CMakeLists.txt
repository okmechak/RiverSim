cmake_minimum_required(VERSION 2.8)


#common
add_library(GeometryPrimitives GeometryPrimitives.hpp GeometryPrimitives.cpp) 

#tethex
add_library(tethex tethex.hpp tethex.cpp) 
target_link_libraries(tethex GeometryPrimitives)

#boundary
add_library(boundary boundary.hpp boundary.cpp) 
target_link_libraries(boundary GeometryPrimitives)

#tree
add_library(tree tree.hpp tree.cpp) 
target_link_libraries(tree GeometryPrimitives boundary)

#physmodel
add_library(physmodel physmodel.hpp physmodel.cpp) 
target_link_libraries(physmodel GeometryPrimitives tree boundary tethex)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle physmodel)

#io
add_library(io io.hpp io.cpp json.hpp version.hpp)
target_link_libraries(io physmodel tree boundary)

#mesh
add_library(mesh mesh.hpp mesh.cpp) 
target_link_libraries(mesh tethex physmodel triangle)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver physmodel)

#boundary_generator
add_library(boundary_generator boundary_generator.hpp boundary_generator.cpp) 
target_link_libraries(boundary_generator GeometryPrimitives tethex physmodel boundary tree)

#riversim
add_library(river riversim.hpp riversim.cpp)
deal_ii_setup_target(river)
target_link_libraries(river tethex GeometryPrimitives physmodel boundary triangle io solver tree mesh boundary_generator)

#river
add_library(riv river.hpp river.cpp)
deal_ii_setup_target(riv)
target_link_libraries(riv tethex GeometryPrimitives physmodel border triangle io solver tree mesh boundary_generator)