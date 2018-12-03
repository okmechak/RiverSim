#pragma once

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;

namespace po = boost::program_options;


namespace River{
    
    void printAsciiSignature();

    po::variables_map processProgramOptions(int argc, char* argv[]);
    
}