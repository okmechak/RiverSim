#pragma once

#include <iostream>
#include "cxxopts.hpp"

using namespace std;


namespace River{
    
    void print_ascii_signature();
    void print_version();
    cxxopts::ParseResult process_program_options(int argc, char* argv[]);
    
}