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
        
        find_package(Boost COMPONENTS unit_test_framework REQUIRED)
        
        #I like to keep test files in a separate source directory called test
        file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
        
        #Run through each source
        foreach(testSrc ${TEST_SRCS})
                #Extract the filename without an extension (NAME_WE)
                get_filename_component(testName ${testSrc} NAME_WE)
        
                #Add compile target
                add_executable(${testName} ${testSrc})
                deal_ii_setup_target(${testName})
        
                #link to Boost libraries AND your targets and dependencies
                target_link_libraries(${testName} ${Boost_LIBRARIES} river)
        
                #Finally add it to test execution - 
                #Notice the WORKING_DIRECTORY and COMMAND
                add_test(NAME ${testName}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                         COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/${testName})
        endforeach(testSrc)
endif(BUILD_TEST)