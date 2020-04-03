cmake_minimum_required(VERSION 2.8)

set(BUILD_TEST ON CACHE BOOL "Build Boost Unit Tests")

if(BUILD_TEST)
    include_directories(${PROJECT_SOURCE_DIR}/source/river)
    include_directories(${PROJECT_SOURCE_DIR}/source/river/tethex)

    message("========================================")
    message("Deal II setup")
    set(deal.II_USE_STATIC_LIBS "ON")
    find_package(deal.II REQUIRED HINTS ${DEALII_DIR})
    deal_ii_initialize_cached_variables()

    message("========================================")
    message("Boost.Test setup")    
    find_package(Boost REQUIRED COMPONENTS unit_test_framework REQUIRED)
    
    message("========================================")
    message("Adding executables tests")

    add_executable(tGeometryPrimitives GeometryPrimitives.cpp)
    target_link_libraries(tGeometryPrimitives GeometryPrimitives ${Boost_LIBRARIES})
    add_test(
        NAME tGeometryPrimitives
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tGeometryPrimitives)

    add_executable(tethex_test tethex.cpp)
    target_link_libraries(tethex_test tethex ${Boost_LIBRARIES})
    add_test(
        NAME tethex_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tethex)
    
    add_executable(boundary_test boundary.cpp)
    target_link_libraries(boundary_test boundary ${Boost_LIBRARIES})
    add_test(
        NAME boundary_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/boundary)

    add_executable(tree_test tree.cpp)
    target_link_libraries(tree_test tree ${Boost_LIBRARIES})
    add_test(
        NAME tree_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tree)

    add_executable(io_test io.cpp)
    target_link_libraries(io_test io ${Boost_LIBRARIES})
    add_test(
        NAME io_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/io)

    add_executable(physmodel_test physmodel.cpp)
    target_link_libraries(physmodel_test physmodel ${Boost_LIBRARIES})
    deal_ii_setup_target(physmodel_test)
    add_test(
        NAME physmodel_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/physmodel)
endif(BUILD_TEST)