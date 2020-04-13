cmake_minimum_required(VERSION 2.8.12)

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs.")
set(DEAL_II_PREFER_STATIC_LIBS ON CACHE BOOL "Static linkage of Deal.II library will be prefferable.")

if(BUILD_PROGRAM)

    message("========================================")
    message("Deal II setup")

    find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
    deal_ii_initialize_cached_variables()

    mark_as_advanced(deal.II_DIR DEALII_DIR DEAL_II_PREFER_STATIC_LIBS)

    message("========================================")
    message("River, Tethex, Triangle setup")
    add_subdirectory(river)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/river
        ${CMAKE_CURRENT_SOURCE_DIR}/river/tethex)

    set(SOURCE_FILES 
        main.cpp)

    add_executable(riversim ${SOURCE_FILES})
    deal_ii_setup_target(riversim)
    target_link_libraries(riversim river)

    add_executable(riversim_perfomance perfomance.cpp)
    deal_ii_setup_target(riversim_perfomance)
    target_link_libraries(riversim_perfomance riv)

    add_executable(meshgen meshgen.cpp)
    target_link_libraries(meshgen mesh io boundary_generator)

    install(TARGETS riversim DESTINATION riversim)
    install(TARGETS riversim_perfomance DESTINATION riversim)
    install(TARGETS meshgen DESTINATION riversim)
endif(BUILD_PROGRAM)