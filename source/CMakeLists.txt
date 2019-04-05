cmake_minimum_required(VERSION ${min_version})

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs")

if(BUILD_PROGRAM)

    message("========================================")
    message("Deal II setup")
    set(DEALII_DIR "/usr/local/dealii" CACHE STRING "Deal.II SDK directory location")
    set(deal.II_USE_STATIC_LIBS "ON")
    find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
    deal_ii_initialize_cached_variables()

    mark_as_advanced(deal.II_DIR)


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