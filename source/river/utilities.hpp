/*! 
  \file utilities.hpp
  
  Console printouts, version, title and program options processing.
*/

#pragma once

#include "cxxopts.hpp"

using namespace std;


namespace River{
    
    ///Prints to console program logo.
    void print_ascii_signature();

    ///Prints to console program Version.
    void print_version();

    ///Processing of program options.
    cxxopts::ParseResult process_program_options(int argc, char* argv[]);
    
}