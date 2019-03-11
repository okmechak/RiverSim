# first we can indicate the documentation build as an option and set it to ON by default
set(BUILD_DOC ON CACHE BOOL "Build Doxygen documentation")

# check if Doxygen is installed
if(BUILD_DOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        message("Doxygen build started")
        set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doxy/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        message(${CMAKE_CURRENT_BINARY_DIR})
  
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  
  
      # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(BUILD_DOC)