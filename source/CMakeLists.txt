message("Tetgen setup")#FIXME remove these absolute paths
include_directories(/usr/local/tetgen)
link_directories(/usr/local/tetgen)

message("GMSH setup")#FIXME remove these absolute paths
include_directories(/usr/local/gmsh/include)
link_directories(/usr/local/gmsh/lib)

include_directories(source)

file(GLOB sources ${CMAKE_SOURCE_DIR}/source/*.cpp)

add_executable(riversim ${sources})

deal_ii_setup_target(riversim)
target_link_libraries(riversim ${Boost_LIBRARIES})
target_link_libraries(riversim libtetgen.so)#FIXME is there possibility not to use exact names?
target_link_libraries(riversim libgmsh.so)