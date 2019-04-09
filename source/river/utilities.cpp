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

#include "utilities.hpp"
#include <iostream>

namespace River
{

    string ProgramTitle = "RiverSim 2018";
    
    void print_ascii_signature()
    {
        cout << endl;
        cout << "     _)                    _)           " << endl;
        cout << "  __| |\\ \\   / _ \\  __| __| | __ `__ \\  " << endl;
        cout << " |    | \\ \\ /  __/ |  \\__ \\ | |   |   | " << endl;
        cout << "_|   _|  \\_/ \\___|_|  ____/_|_|  _|  _| " << endl;         
        cout << endl;            
    }


    void print_version()
    {
        cout << ProgramTitle+ " version: 1.0.0" << endl;
    }

    cxxopts::ParseResult process_program_options(int argc, char *argv[])
    {
        using namespace cxxopts;

        Options options(
            "riversim", ProgramTitle + " is used in my master work to simulate river growth according to \nLaplace model(for more details pls see references)");

        //declare supported options
        options.add_options()
        //different
        ("h,help", "produce help message")
        ("v,version", "print version string")
        ("suppress-signature", "suppress signature printing")
        ("long-description",
            "full documentation is placed here https://okmechak.github.io/RiverSim/")

        //file system interface
        ("output-mesh", "save mesh", value<string>()->default_value("mesh.msh"))
        ("o,output-sim", "save simulation data to file", value<string>()->default_value("simulation_data.json"))
        ("i,input-sim", "open simulation data", value<string>()->default_value(""))
        ("p,prefix", "prefix used in name of output file", value<string>()->default_value(""))
        ("input",
            "First option is boundary mesh file name(read documentation for detailed file format)."
            "Second[optional] - is the tree structure file", cxxopts::value<string>())

        //prints/logs
        ("V,verbose", "print detailed log to terminal")
        ("Q,quiet", "print detailed log to terminal")

        //obsolete
        ("g,geom-type", "Geometry type: 0 - Simple Box, 1 - Simple River, 2 - Single Tip - obsolete", value<int>()->default_value("0"))

        //Development flags
        ("s,simulate", "Run simulation in Deal.II", value<bool>()->default_value("true"))
        ("D,dev-mode", "Suppress standart flow", value<bool>()->default_value("false"))
        ("m,mesh", "Turn on/off mesh generation, development purposes only", value<bool>()->default_value("true"))
        ("t,test-flag", "Test flag for development purposes")

        //Model parameters
        ("f,field-value", "value of filed used for Poisson conditions", value<double>()->default_value("0.25"))
        ("n,number-of-steps", "Number of steps to simulate(-1 - infinity)", value<int>()->default_value("-1"))
        ("ds", "ds - minimal lenght of growing", value<double>()->default_value("0.01"))
        ("dx", "dx - shift of initial river position from beginning of coordinates", value<double>()->default_value("0.1"))
        ("b,biffurcation-threshold", "biffuraction threshold", value<double>()->default_value("-0.1"))
        ("width", "width of rectangular region", value<double>()->default_value("1"))
        ("height", "height of rectangular region", value<double>()->default_value("1"))
        ("A,mesh-max-area", "constraints maximal area of triangle element", value<double>()->default_value("0.01."))
        ("q,mesh-min-angle", "constraints minimal angle of triangle element", value<double>()->default_value("20."))
        ("e,eps", "Width of branch", value<double>()->default_value("0.000001"))
        ("r,ref-num", "Number of solver adaptive refinments", value<int>()->default_value("1"));

        options.parse_positional({"input"});

        auto result = options.parse(argc, argv);

        if (result.count("help"))
            cout << options.help({""}) << endl;

        if (result.count("version"))
            print_version();

        return result;
    }

} // namespace River