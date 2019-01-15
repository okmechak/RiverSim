#pragma once

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;

namespace po = boost::program_options;


namespace River{
    
    void print_ascii_signature();
    void print_version();
    po::variables_map process_program_options(int argc, char* argv[]);
    
}