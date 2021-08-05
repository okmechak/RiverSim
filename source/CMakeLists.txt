cmake_minimum_required(VERSION 2.8.12)

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs.")
set(BUILD_PYTHON_API ON CACHE BOOL "Generate python library")
set(DEAL_II_PREFER_STATIC_LIBS ON CACHE BOOL "Static linkage of Deal.II library will be prefferable.")

if(BUILD_PROGRAM)

    set(CMAKE_CXX_STANDARD 17)

    message("========================================")
    message("Deal II setup")

    find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
    deal_ii_initialize_cached_variables()

    mark_as_advanced(deal.II_DIR DEALII_DIR DEAL_II_PREFER_STATIC_LIBS)

    message("========================================")
    message("River, Tethex, Triangle setup")

    # point
    add_library(point ../include/point.hpp point.cpp)
    target_include_directories(point PUBLIC ../include)
    install(TARGETS point)

    # boundary
    add_library(boundary ../include/boundary.hpp boundary.cpp) 
    target_link_libraries(boundary PUBLIC point)
    target_include_directories(boundary PUBLIC ../include)
    install(TARGETS boundary)

    # tree
    add_library(tree ../include/tree.hpp tree.cpp) 
    target_link_libraries(tree PUBLIC boundary)
    target_include_directories(tree PUBLIC ../include)
    install(TARGETS tree)

    # tethex
    add_library(tethex ../include/tethex.hpp tethex.cpp) 
    target_link_libraries(tethex PUBLIC point)
    target_include_directories(tethex PUBLIC ../include)
    install(TARGETS tethex)

    # triangle
    add_library(triangle ../include/triangle.hpp triangle.cpp) 
    target_link_libraries(triangle PUBLIC point)
    target_include_directories(triangle PUBLIC ../include)
    install(TARGETS triangle)

    # mesh
    add_library(mesh ../include/mesh.hpp mesh.cpp) 
    target_link_libraries(mesh PUBLIC tethex triangle)
    target_include_directories(mesh PUBLIC ../include)
    install(TARGETS mesh)

    # boundary_generator
    add_library(boundary_generator ../include/boundary_generator.hpp boundary_generator.cpp) 
    target_link_libraries(boundary_generator PUBLIC tree)
    target_include_directories(boundary_generator PUBLIC ../include)
    install(TARGETS boundary_generator)

    # solver
    add_library(solver ../include/solver.hpp solver.cpp) 
    deal_ii_setup_target(solver)
    target_link_libraries(solver tethex)
    target_include_directories(solver PUBLIC ../include)
    install(TARGETS solver)

    #river
    # add_library(river ../include/river.hpp river.cpp) 
    # target_link_libraries(river PUBLIC tree solver boundary_generator)
    # deal_ii_setup_target(river)
    # target_include_directories(river PUBLIC ../include)
    # install(TARGETS river)
    
    # # io
    # add_library(io ../include/io.hpp io.cpp ../include/json.hpp ../include/version.hpp)
    # target_link_libraries(io river)
    # target_include_directories(io PUBLIC ../include)
    # install(TARGETS io)

    # # app
    # add_library(app ../include/app.hpp app.cpp)
    # deal_ii_setup_target(app)
    # target_link_libraries(app river)
    # target_include_directories(app PUBLIC ../include)
    # install(TARGETS app)

    # # executable program
    # add_executable(riversim main.cpp)
    
    # target_link_libraries(riversim app)

    # add_executable(meshgen meshgen.cpp)
    # target_link_libraries(meshgen mesh io boundary_generator)

    # install(TARGETS riversim)
    # install(TARGETS meshgen)

    # if(BUILD_PYTHON_API)
    #     if(NOT BUILD_SHARED_LIBS )
    #         message(FATAL_ERROR "Python API can't be build with static libraries, make shure You specified BUILD_SHARED_LIBS = true in cmake options" )
    #     endif()
    #     message("========================================")
    #     message("Python API setup")
    #     # Find python and Boost - both are required dependencies
    #     find_package(PythonLibs 3.8 REQUIRED)
    #     find_package(Boost COMPONENTS python38 REQUIRED)

    #     # Without this, any build libraries automatically have names "lib{x}.so"
    #     set(CMAKE_SHARED_MODULE_PREFIX "")

    #     # Add a shared module - modules are intended to be imported at runtime.
    #     # - This is where you add the source files
    #     add_library(riversimPY MODULE pythonapi.cpp)
    #     deal_ii_setup_target(riversimPY)

    #     # Set up the libraries and header search paths for this target
    #     target_link_libraries(riversimPY ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} app)
    #     target_include_directories(riversimPY PRIVATE ${PYTHON_INCLUDE_DIRS})
    #     set_target_properties(riversimPY
    #         PROPERTIES OUTPUT_NAME riversim)

    #     install(TARGETS riversim)

    # endif(BUILD_PYTHON_API)
endif(BUILD_PROGRAM)