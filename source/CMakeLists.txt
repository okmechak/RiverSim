cmake_minimum_required(VERSION 2.8.12)

set(BUILD_PROGRAM ON CACHE BOOL "Build(Compile) Option is handy for build only docs.")
set(BUILD_PYTHON_API ON CACHE BOOL "Generate python library")
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

    add_library(app app.hpp app.cpp)
    deal_ii_setup_target(app)
    target_link_libraries(app river)

    set(SOURCE_FILES 
        main.cpp)

    add_executable(riversim ${SOURCE_FILES})
    deal_ii_setup_target(riversim)
    target_link_libraries(riversim app)

    add_executable(meshgen meshgen.cpp)
    target_link_libraries(meshgen mesh io boundary_generator)

    install(TARGETS riversim DESTINATION riversim)
    install(TARGETS meshgen DESTINATION riversim)

    
    if(BUILD_PYTHON_API)
        if(NOT BUILD_SHARED_LIBS )
            message(FATAL_ERROR "Python API can't be build with static libraries, make shure You specified BUILD_SHARED_LIBS = true in cmake options" )
        endif()
        message("========================================")
        message("Python API setup")
        # Find python and Boost - both are required dependencies
        find_package(PythonLibs 3.8 REQUIRED)
        find_package(Boost COMPONENTS python38 REQUIRED)

        # Without this, any build libraries automatically have names "lib{x}.so"
        set(CMAKE_SHARED_MODULE_PREFIX "")

        # Add a shared module - modules are intended to be imported at runtime.
        # - This is where you add the source files
        add_library(riversimPY MODULE pythonapi.cpp)
        deal_ii_setup_target(riversimPY)

        # Set up the libraries and header search paths for this target
        target_link_libraries(riversimPY ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} app)
        target_include_directories(riversimPY PRIVATE ${PYTHON_INCLUDE_DIRS})
        set_target_properties(riversimPY
            PROPERTIES OUTPUT_NAME riversim)

        install(TARGETS riversimPY DESTINATION riversim)
    endif(BUILD_PYTHON_API)
endif(BUILD_PROGRAM)