cmake_minimum_required(VERSION 2.8)

#tethex
add_library(tethex tethex.hpp tethex.cpp) 

#common
add_library(GeometryPrimitives GeometryPrimitives.hpp GeometryPrimitives.cpp) 

#border
add_library(border border.hpp border.cpp) 
target_link_libraries(border GeometryPrimitives)

#tree
add_library(tree tree.hpp tree.cpp) 
target_link_libraries(tree GeometryPrimitives)

#physmodel
add_library(physmodel physmodel.hpp physmodel.cpp) 
target_link_libraries(physmodel GeometryPrimitives tree border tethex)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle physmodel)

#io
add_library(io io.hpp io.cpp json.hpp version.hpp)
target_link_libraries(io physmodel tree border)

#mesh
add_library(mesh mesh.hpp mesh.cpp) 
target_link_libraries(mesh tethex physmodel triangle)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver physmodel)

#river
add_library(river riversim.hpp riversim.cpp)
deal_ii_setup_target(river)
target_link_libraries(river tethex GeometryPrimitives physmodel border triangle io solver tree mesh)