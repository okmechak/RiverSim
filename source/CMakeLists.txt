include_directories(/usr/local/include)
include_directories(source)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB sources ${CMAKE_SOURCE_DIR}/source/*.cpp)

add_executable(riversim ${sources})
target_link_libraries(riversim ${Boost_LIBRARIES})
deal_ii_setup_target(riversim)