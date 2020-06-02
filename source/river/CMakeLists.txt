cmake_minimum_required(VERSION 2.8.12)

#GeometryPrimitives
add_library(GeometryPrimitives GeometryPrimitives.hpp GeometryPrimitives.cpp) 


#boundary
add_library(boundary boundary.hpp boundary.cpp) 
target_link_libraries(boundary GeometryPrimitives)

#tethex
add_library(tethex tethex.hpp tethex.cpp) 
target_link_libraries(tethex GeometryPrimitives)

#tree
add_library(tree tree.hpp tree.cpp) 
target_link_libraries(tree boundary)

#physmodel
add_library(physmodel physmodel.hpp physmodel.cpp) 
target_link_libraries(physmodel tree tethex)

#io
add_library(io io.hpp io.cpp json.hpp version.hpp)
target_link_libraries(io physmodel)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle physmodel)

#boundary_generator
add_library(boundary_generator boundary_generator.hpp boundary_generator.cpp) 
target_link_libraries(boundary_generator physmodel)

#mesh
add_library(mesh mesh.hpp mesh.cpp) 
target_link_libraries(mesh tethex triangle)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver physmodel)

#riversim
add_library(river riversim.hpp riversim.cpp)
deal_ii_setup_target(river)
target_link_libraries(river io solver mesh boundary_generator)