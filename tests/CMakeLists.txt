include_directories(${PROJECT_SOURCE_DIR}/source/river)

message("========================================")
message("GMSH setup")
include_directories(${gmsh_DIRECTORY}/include)
link_directories(${gmsh_DIRECTORY}/lib)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)


#I like to keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)


#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link to Boost libraries AND your targets and dependencies
        target_link_libraries(${testName} ${Boost_LIBRARIES} river)

        #I like to move testing binaries into a testBin directory
        #set_target_properties(${testName} PROPERTIES 
        #    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)

        #Finally add it to test execution - 
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                 COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/tests/${testName})
endforeach(testSrc)