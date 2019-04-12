cmake_minimum_required(VERSION ${min_version})

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs.")
set(DEAL_II_PREFER_STATIC_LIBS ON CACHE BOOL "Static linkage of Deal.II library will be prefferable.")

if(BUILD_PROGRAM)

    message("========================================")
    message("Deal II setup")

    set(DEALII_REL_DIR "/usr/local/dealii_release" CACHE STRING "Deal.II release SDK directory location.") 
    set(DEALII_DEB_DIR "/usr/local/dealii_debug" CACHE STRING "Deal.II debug SDK directory location.") 

    if(CMAKE_BUILD_TYPE MATCHES Release)
        set(DEALII_DIR ${DEALII_REL_DIR})
    elseif(CMAKE_BUILD_TYPE MATCHES Debug)
        set(DEALII_DIR ${DEALII_DEB_DIR})
    endif() 

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
endif(BUILD_PROGRAM)