cmake_minimum_required(VERSION 2.8.12)

set(BUILD_TEST ON CACHE BOOL "Build Boost Unit Tests")

if(BUILD_TEST)
    include_directories(${PROJECT_SOURCE_DIR}/source)
    include_directories(${PROJECT_SOURCE_DIR}/inlcude)

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

    #point
    add_executable(tpoint point.cpp)
    target_link_libraries(tpoint point ${Boost_LIBRARIES})
    add_test(
        NAME tpoint
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tpoint)
    
    #boundary
    add_executable(tboundary boundary.cpp)
    target_link_libraries(tboundary boundary ${Boost_LIBRARIES})
    add_test(
        NAME tboundary
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tboundary)

    #tree
    add_executable(ttree tree.cpp)
    target_link_libraries(ttree tree ${Boost_LIBRARIES})
    add_test(
        NAME ttree
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttree)
    
    #tethex
    add_executable(ttethex tethex.cpp)
    target_link_libraries(ttethex tethex ${Boost_LIBRARIES})
    add_test(
        NAME ttethex
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttethex)

    #triangle
    add_executable(ttriangle triangle.cpp)
    target_link_libraries(ttriangle triangle ${Boost_LIBRARIES})
    add_test(
        NAME ttriangle
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttriangle)

    #boundary_generator
    add_executable(tboundary_generator boundary_generator.cpp)
    target_link_libraries(tboundary_generator boundary_generator tethex ${Boost_LIBRARIES})
    add_test(
        NAME tboundary_generator
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tboundary_generator)

    #solver
    add_executable(tsolver solver.cpp)
    target_link_libraries(tsolver solver ${Boost_LIBRARIES})
    deal_ii_setup_target(tsolver)
    add_test(
        NAME tsolver
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tsolver)

    #river
    # add_executable(triver river.cpp)
    # target_link_libraries(triver river ${Boost_LIBRARIES})
    # add_test(
    #     NAME triver
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #     COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/triver)

    #io
    # add_executable(tio io.cpp)
    # target_link_libraries(tio io ${Boost_LIBRARIES})
    # add_test(
    #     NAME tio
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #     COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tio)

    #riversim
    # add_executable(triversim riversim.cpp)
    # target_link_libraries(triversim river ${Boost_LIBRARIES})
    # deal_ii_setup_target(triversim)
    # add_test(
    #     NAME triversim
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #     COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/triversim)
    
endif(BUILD_TEST)