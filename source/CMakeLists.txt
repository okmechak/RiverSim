message("Tetgen setup")
include_directories(/usr/local/tetgen)
link_directories(/usr/local/tetgen)

message("GMSH setup")
include_directories(/usr/local/gmsh/include)
link_directories(/usr/local/gmsh/lib)



include_directories(source)


file(GLOB sources ${CMAKE_SOURCE_DIR}/source/*.cpp)

add_executable(riversim ${sources})

deal_ii_setup_target(riversim)
target_link_libraries(riversim ${Boost_LIBRARIES})
target_link_libraries(riversim libtetgen.a)
target_link_libraries(riversim libgmsh.a)