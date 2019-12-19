cmake_minimum_required(VERSION 2.8)

#tethex
add_library(tethex tethex.hpp tethex.cpp) 

#common
add_library(common common.hpp common.cpp) 

#utilities
add_library(utilities utilities.hpp utilities.cpp version.hpp)

#physmodel
add_library(physmodel physmodel.hpp physmodel.cpp) 
target_link_libraries(physmodel common)

#border
add_library(border border.hpp border.cpp) 
target_link_libraries(border common)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle physmodel)

#tree
add_library(tree tree.hpp tree.cpp) 
target_link_libraries(tree tethex physmodel border)

#io
add_library(io io.hpp io.cpp json.hpp)
target_link_libraries(io utilities physmodel tree border)

#mesh
add_library(mesh mesh.hpp mesh.cpp) 
target_link_libraries(mesh tethex physmodel triangle)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver tethex physmodel)

#river
add_library(river riversim.hpp riversim.cpp)
deal_ii_setup_target(river)
target_link_libraries(river tethex common physmodel border triangle io solver tree mesh)