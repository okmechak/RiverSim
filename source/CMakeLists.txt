cmake_minimum_required(VERSION ${min_version})

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs")

if(BUILD_PROGRAM)
    set(GMSH_DIR "/usr/local/gmsh" CACHE STRING "gmsh SDK directory location")
    set(DEALII_DIR "/usr/local/dealii" CACHE STRING "Deal.II SDK directory location")

    message("========================================")
    message("Deal II setup")
    set(deal.II_USE_STATIC_LIBS "ON")
    find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
    deal_ii_initialize_cached_variables()


    message("========================================")
    message("GMSH setup")
    find_path(GMSH_HEADER gmsh.h HINTS ${GMSH_DIR}/include /usr/include /usr/local/include)
    find_library(GMSH_LIB gmsh HINTS ${GMSH_DIR}/lib /usr/lib /usr/local/lib)
    include_directories(${GMSH_HEADER})
    get_filename_component(GMSH_LIB_PATH ${GMSH_LIB} DIRECTORY)
    link_directories(${GMSH_LIB_PATH})

    mark_as_advanced(GMSH_HEADER GMSH_LIB GMSH_LIB_PATH deal.II_DIR)


    message("========================================")
    message("River, Tethex, Triangle setup")
    add_subdirectory(river)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/river
        ${CMAKE_CURRENT_SOURCE_DIR}/river/tethex
        ${CMAKE_CURRENT_SOURCE_DIR}/river/triangle)

    set(SOURCE_FILES 
        main.cpp)

    add_executable(riversim ${SOURCE_FILES})
    deal_ii_setup_target(riversim)

    target_link_libraries(riversim river ${GMSH_LIB})
endif(BUILD_PROGRAM)