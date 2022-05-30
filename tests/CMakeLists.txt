cmake_minimum_required(VERSION 2.8.12)

set(BUILD_TEST ON CACHE BOOL "Build Boost Unit Tests")

if(BUILD_TEST)
    include_directories(${PROJECT_SOURCE_DIR}/source/river)

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
    #add_executable(ttree tree.cpp)
    #target_link_libraries(ttree tree ${Boost_LIBRARIES})
    #add_test(
    #    NAME ttree
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttree)

    #region
    add_executable(tregion region.cpp)
    target_link_libraries(tregion region ${Boost_LIBRARIES})
    add_test(
        NAME tregion
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tregion)
    
    #tethex
    #add_executable(ttethex tethex.cpp)
    #target_link_libraries(ttethex tethex ${Boost_LIBRARIES})
    #add_test(
    #    NAME ttethex
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttethex)

    #triangle
    #add_executable(ttriangle triangle.cpp)
    #target_link_libraries(ttriangle mesh boundary_generator ${Boost_LIBRARIES})
    #add_test(
    #    NAME ttriangle
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/ttriangle)

    

    #physmodel
    #add_executable(tphysmodel physmodel.cpp)
    #target_link_libraries(tphysmodel physmodel ${Boost_LIBRARIES})
    #add_test(
    #    NAME tphysmodel
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tphysmodel)

    #io
    #add_executable(tio io.cpp)
    #target_link_libraries(tio io ${Boost_LIBRARIES})
    #add_test(
    #    NAME tio
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tio)
    
    #boundary_generator
    #add_executable(tboundary_generator boundary_generator.cpp)
    #target_link_libraries(tboundary_generator boundary_generator ${Boost_LIBRARIES})
    #add_test(
    #    NAME tboundary_generator
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tboundary_generator)

    #solver
    #add_executable(tsolver solver.cpp)
    #target_link_libraries(tsolver solver mesh boundary_generator ${Boost_LIBRARIES})
    #deal_ii_setup_target(tsolver)
    #add_test(
    #    NAME tsolver
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tsolver)

    #riversim
    #add_executable(triversim riversim.cpp)
    #target_link_libraries(triversim river ${Boost_LIBRARIES})
    #deal_ii_setup_target(triversim)
    #add_test(
    #    NAME triversim
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/triversim)

    #riversim_new
    #add_executable(triversim_new riversim_new.cpp)
    #target_link_libraries(triversim_new river ${Boost_LIBRARIES})
    #deal_ii_setup_target(triversim_new)
    #add_test(
    #    NAME triversim_new
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/triversim)
    
endif(BUILD_TEST)