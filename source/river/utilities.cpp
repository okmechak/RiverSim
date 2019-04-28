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
#include <string>

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

    string version_string()
    {
        return to_string(VERSION_MAJOR) + "." 
            + to_string(VERSION_MINOR) + "."
            + to_string(VERSION_PATCH);
    }

    void print_version()
    {
        cout << ProgramTitle+ " version: " 
            + version_string() << endl;
    }

    cxxopts::ParseResult process_program_options(int argc, char *argv[])
    {
        using namespace cxxopts;

        Options options(
            "riversim", ProgramTitle + " is used in my master work to simulate river growth according to \n"
            "Laplace model(for more details pls see references)\n" + 
            "full documentation is placed here https://okmechak.github.io/RiverSim/\n");

        //declare supported options
        //basic
        options.add_options("Basic")
        ("h,help", "Produce help message.")
        ("v,version", "Print version string.")
        ("suppress-signature", "Suppress signature printing.");

        //file system interface
        options.add_options("File interface")
        ("o,output", "Name of simulation data and state of program data.", value<string>()->default_value("simdata"))
        ("save-each-step", "Save each step of simulation if separate file.")
        ("input",
            "input simaultion data, boundary, tree, model parameters.", cxxopts::value<string>());

        //prints and logs
        options.add_options("Logs")
        ("V,verbose", "print detailed log to terminal.", value<bool>()->default_value("False"))
        ("Q,quiet", "print detailed log to terminal.", value<bool>()->default_value("True"))

        //Development flags
        ("t,test-flag", "Test flag for development purposes.", value<vector<double>>()->default_value("1020 30 4 5"));

        //Simulation parameters
        options.add_options("Simulation parameters")
        ("n,number-of-steps", "Number of steps to simulate(-1 - infinity).", value<int>()->default_value("10"))
        ("simulation-type", "Forward river growth or backward river growth. 0 - Forward, 1 - Backward.", value<int>()->default_value("0"));
        
        //Geometry parameters
        options.add_options("Geometry parameters")
        ("width", "Width of rectangular region.", value<double>()->default_value("1"))
        ("height", "Height of rectangular region.", value<double>()->default_value("1"))
        ("dx", "dx - shift of initial river position from beginning of coordinates.", value<double>()->default_value("0.2"));

        //Model parameters
        options.add_options("Model parameters")
        ("c,boundary-condition", "0 - Poisson, 1 - Laplacea", value<int>()->default_value("0"))
        ("f,field-value", "Value of filed used for Poisson conditions.", value<double>()->default_value("0.25"))
        ("eta", "Power of a1^eta.", value<double>()->default_value("0"))
        ("biffurcation-type", "Biffurcation method type. 0 - a3/a1 > biffurcation_threshold, 1 - a1 > biffurcation_threshold, 2 - combines both conditions, 3 - no biffurcation at all.", value<int>()->default_value("2"))
        ("b,biffurcation-threshold", "Biffuraction threshold for a(3)/a(1) > kcrit", value<double>()->default_value("-0.1"))
        ("biffurcation-threshold-2", "Biffuraction threshold for a(1) > kcrit", value<double>()->default_value("0.002"))
        ("biffurcation-min-distance", "Minimal distance between adjacent biffurcation points. Reduces numerical noise", value<double>()->default_value("0.025"))
        ("biffurcation-angle", "Biffurcation angle between branches. default Pi/5 or Pi/10?? FIXME", value<double>()->default_value("0.62831853071795864"))
        ("growth-type", "0 - arctan(a2/a1), 1 - {dx, dy}.", value<int>()->default_value("0"))
        ("growth-threshold", "Growth of branch will be done only if a1 > growth-threshold.", value<double>()->default_value("0."))
        ("growth-min-distance", "Distance of constant tip growing after biffurcation point. Reduces numerical noise", value<double>()->default_value("0.02"))
        ("ds", "ds - minimal lenght of growing", value<double>()->default_value("0.004"));

        //Integration parameters
        options.add_options("Series parameters integral")
        ("weight-exp", "Parameter used in integration.", value<double>()->default_value("2"))
        ("integration-radius", "Integration radius around tips for series parameters", value<double>()->default_value("0.03"))
        ("weight-radius", "Weight radius parameter.", value<double>()->default_value("0.01"));

        //Mesh parameters
        options.add_options("Mesh refinment parameters")
        ("mesh-exp", "Parameter used in mesh refinment: 1 + min_area - exp(-(r/ro)^{mesh-exp}).", 
            value<double>()->default_value("500."))
        ("e,eps", "Width of branch", value<double>()->default_value("1e-5"))
        ("q,mesh-min-angle", "Constraints minimal angle of triangle element", 
            value<double>()->default_value("30."))
        ("A,mesh-max-area", "Constraints maximal area of triangle element", 
            value<double>()->default_value("10."))
        ("mesh-min-area", "Constraints minimal area of triangle element. Aplied at refinment: 1 + min_area - exp(-(r/ro)^{mesh-exp}).", 
            value<double>()->default_value("1e-11"))
        ("refinment-radius", "r0 - refinment radius from this formula: 1 + min_area - exp(-(r/ro)^{mesh-exp})", 
            value<double>()->default_value("0.2"));

        options.add_options("Solver Parameters")
        ("quadrature-degree", "quadrature polynomials degree used in numerical integration of Solver.", value<int>()->default_value("2"))
        ("refinment-fraction", "Fraction of refined mesh elements.", value<double>()->default_value("0.01"))
        ("refinment-steps", "Number of refinment steps.", value<int>()->default_value("3"));


        options.parse_positional({"input"});

        auto result = options.parse(argc, argv);

        if (result.count("help"))
            cout << options.help({
                "Basic", 
                "File interface", 
                "Logs",
                "Simulation parameters",
                "Geometry parameters",
                "Model parameters",
                "Series parameters integral",
                "Mesh refinment parameters",
                "Solver Parameters"}) 
                << endl;

        if (result.count("version"))
            print_version();

        return result;
    }

} // namespace River