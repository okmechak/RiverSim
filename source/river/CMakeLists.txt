cmake_minimum_required(VERSION 3.2)

add_subdirectory(tethex)
add_subdirectory(triangle)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tethex
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle)

set(SOURCE_FILES 
    riversim.cpp
    riversim.hpp)

add_library(river ${SOURCE_FILES})
deal_ii_setup_target(river)

target_link_libraries(river ${Boost_LIBRARIES})
target_link_libraries(river libtetgen.so)#FIXME is there possibility not to use exact names?
target_link_libraries(river libgmsh.so)
target_link_libraries(river tethex triangle)

install(TARGETS river DESTINATION ${riversim_INSTALL_LIB_DIR})
install(FILES riversim.hpp DESTINATION ${riversim_INSTALL_INCLUDE_DIR})