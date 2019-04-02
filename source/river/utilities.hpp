/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/** 
 * @file utilities.hpp
 * 
 * Console printouts, version, title and program options processing.
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