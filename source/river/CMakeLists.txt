cmake_minimum_required(VERSION 3.2)

add_subdirectory(tethex)
add_subdirectory(triangle)
add_subdirectory(tetgen)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/tethex
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle
    ${CMAKE_CURRENT_SOURCE_DIR}/tetgen)

set(SOURCE_FILES 
    riversim.cpp
    riversim.hpp)

add_library(river ${SOURCE_FILES})
set_property(TARGET river PROPERTY CXX_STANDARD 17)
deal_ii_setup_target(river)

target_link_libraries(river ${Boost_LIBRARIES})
target_link_libraries(river libgmsh.so)
target_link_libraries(river tethex triangle tetgen)

install(TARGETS river DESTINATION ${riversim_INSTALL_LIB_DIR})
install(FILES riversim.hpp DESTINATION ${riversim_INSTALL_INCLUDE_DIR})