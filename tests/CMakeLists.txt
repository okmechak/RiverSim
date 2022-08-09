cmake_minimum_required(VERSION 2.8.12)

message(${PROJECT_SOURCE_DIR})
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

#rivers
add_executable(trivers rivers.cpp)
target_link_libraries(trivers rivers ${Boost_LIBRARIES})
add_test(
    NAME trivers
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/trivers)

#region
add_executable(tregion region.cpp)
target_link_libraries(tregion region ${Boost_LIBRARIES})
add_test(
    NAME tregion
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tregion)
    
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
add_executable(tio io.cpp)
target_link_libraries(tio io ${Boost_LIBRARIES})
add_test(
    NAME tio
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tio)
    
#solver
add_executable(tsolver solver.cpp)
target_link_libraries(tsolver solver ${Boost_LIBRARIES})
deal_ii_setup_target(tsolver)
add_test(
    NAME tsolver
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/tsolver)