cmake_minimum_required(VERSION 2.8.12)

#point
add_library(point point.hpp point.cpp) 
install(TARGETS point DESTINATION riversim)

#boundary
add_library(boundary boundary.hpp boundary.cpp) 
target_link_libraries(boundary point)
install(TARGETS boundary DESTINATION riversim)

#rivers
add_library(rivers rivers.hpp rivers.cpp) 
target_link_libraries(rivers boundary)
install(TARGETS rivers DESTINATION riversim)

#region
add_library(region region.hpp region.cpp) 
target_link_libraries(region rivers)
install(TARGETS region DESTINATION riversim)

#tethex
add_library(tethex tethex.hpp tethex.cpp) 
target_link_libraries(tethex boundary)
install(TARGETS tethex DESTINATION riversim)

#triangle_c
add_library(triangle_c triangle_c.hpp triangle_c.cpp) 
target_link_libraries(triangle_c boundary)
install(TARGETS triangle_c DESTINATION riversim)

#triangle
add_library(triangle triangle.hpp triangle.cpp) 
target_link_libraries(triangle tethex triangle_c)
install(TARGETS triangle DESTINATION riversim)

#solver
add_library(solver solver.hpp solver.cpp) 
deal_ii_setup_target(solver)
target_link_libraries(solver tethex boundary)
install(TARGETS solver DESTINATION riversim)

#model
add_library(model model.hpp model.cpp) 
target_link_libraries(model rivers solver region triangle)
install(TARGETS model DESTINATION riversim)

#io
add_library(io io.hpp io.cpp json.hpp version.hpp)
target_link_libraries(io model)
install(TARGETS io DESTINATION riversim)

#riversolver
add_library(riversolver riversolver.hpp riversolver.cpp) 
target_link_libraries(riversolver io)
install(TARGETS riversolver DESTINATION riversim)
