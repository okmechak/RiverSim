cmake_minimum_required(VERSION 2.8.12)

#GeometryPrimitives
add_library(GeometryPrimitives SHARED GeometryPrimitives.hpp GeometryPrimitives.cpp) 


#boundary
add_library(boundary boundary.hpp boundary.cpp) 
target_link_libraries(boundary GeometryPrimitives)
install(TARGETS boundary DESTINATION riversim)

#tethex
add_library(tethex tethex.hpp tethex.cpp) 
target_link_libraries(tethex GeometryPrimitives)
install(TARGETS tethex DESTINATION riversim)

#tree
add_library(tree tree.hpp tree.cpp) 
target_link_libraries(tree boundary)
install(TARGETS tree DESTINATION riversim)

#physmodel
add_library(physmodel physmodel.hpp physmodel.cpp) 
target_link_libraries(physmodel tree tethex)
install(TARGETS physmodel DESTINATION riversim)

#io
add_library(io io.hpp io.cpp json.hpp version.hpp)
target_link_libraries(io physmodel)
install(TARGETS io DESTINATION riversim)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle physmodel)
install(TARGETS triangle DESTINATION riversim)

#boundary_generator
add_library(boundary_generator boundary_generator.hpp boundary_generator.cpp) 
target_link_libraries(boundary_generator physmodel)
install(TARGETS boundary_generator DESTINATION riversim)

#mesh
add_library(mesh mesh.hpp mesh.cpp) 
target_link_libraries(mesh tethex triangle)
install(TARGETS mesh DESTINATION riversim)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver physmodel)
install(TARGETS solver DESTINATION riversim)

#riversim
add_library(river riversim.hpp riversim.cpp)
deal_ii_setup_target(river)
target_link_libraries(river io solver mesh boundary_generator)
install(TARGETS river DESTINATION riversim)