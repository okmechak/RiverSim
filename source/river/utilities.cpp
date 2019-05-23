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
#include "physmodel.hpp"//default values
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

        const Model mdl;

        Options options(
            "r./iversim", ProgramTitle +" v" + version_string() + " is used to simulate river growth and some other calculations according to \n"
            "Laplace model(for more details pls see references e.g Piotr Morawiecki work.)\n" + 
            "Some documentation is placed also here https://okmechak.github.io/RiverSim/\n\n" + 
            "Standard ways of using program:\n"
            "\t\t./riversim [options(optionally)] [input_file(optionally)]\n" + 
            "\tForward river growth: \n" + 
            "\t\t./riversim -n 100 --eta=0.75 --save-each-step --biffurcation-threshold=-0.4 -o test\n"
            "\tYou can contiue simmulation later from last point(or any other) by typing:\n" + 
            "\t\t./riversim -n 100 -o test_continuation test_99.json\n" +
            "\tNow we can also run bacward simmulation using previous results, by using option \"-t\":\n" + 
            "\t\t./riversim -t 1 -n 99999 test_continuation.json\n" +
            "\tProgram also produces vtk and msh files from different stages of program running which can be viewed from Paraview and Gmsh.\n\n" + 
            "Creation of custom input file:\n" + 
            "\tInput file is of json format, and should contain at least next object: Model(for its format please see output json file).\n" + 
            "\tAlso it can contain Border and Tree(this object can't be setuped without Border object).\n" +  
            "\tIf some field isnt set by Model object they will be replaced by default program options.\n" + 
            "\tDescription of rest of objects You can find in output json file of program. It has description inside.\n" + 
            "\tFor any questions please mail me: oleg.kmechak@gmail.com.\n"
            );

        //declare supported options
        //basic
        options.add_options("Basic")
        ("h,help", "Produce help message.")
        ("v,version", "Print version string.")
        ("suppress-signature", "Suppress signature printing.");

        //file system interface
        options.add_options("File interface")
        ("o,output", "Name of simulation data and state of program data.", value<string>()->default_value("simdata"))
        ("save-each-step", "Save each step of simulation in separate file: simdata_1.json, simdata_2.json.. ")
        ("input",
            "input simaultion data, boundary, tree, model parameters. It has very similar structure as output of program.", cxxopts::value<string>());

        //prints and logs
        options.add_options("Logs")
        ("V,verbose", "print detailed log to terminal. Not implemented, TODO", value<bool>()->default_value("False"))
        ("Q,quiet", "print detailed log to terminal. Not implemented, TODO", value<bool>()->default_value("True"));

        //Simulation parameters
        options.add_options("Simulation parameters")
        ("n,number-of-steps", "Number of steps to simulate.", value<int>()->default_value("10"))
        ("t,simulation-type", "Type of simulation: 0 - forward river growth, 1 - backward river growth, 2 - Used for development purposes.", value<int>()->default_value("0"));
        
        //Geometry parameters
        options.add_options("Border geometry parameters")
        ("width", "Width of river rectangular region.", value<double>()->default_value(to_string(mdl.width)))
        ("height", "Height of river rectangular region.", value<double>()->default_value(to_string(mdl.width)))
        ("dx", "dx - shift of initial river position from beginning of coordinates in X dirrections.", value<double>()->default_value(to_string(mdl.dx)));

        //Model parameters
        options.add_options("Model parameters")
        ("c,boundary-condition", "0 - Poisson(indexes 0,1 and 3 corresponds to free boundary condition, 4 - to zero value on boundary), 1 - Laplacea(Indexes 1 and 3 - free condition, 2 - value one, and 4 - value zero.)", value<int>()->default_value(to_string(mdl.boundary_condition)))
        ("f,field-value", "Value of outter force used for Poisson equation(Right-hand side value)", value<double>()->default_value(to_string(mdl.field_value)))
        ("eta", "Power of a1^eta used in growth of river.", value<double>()->default_value(to_string(mdl.eta)))
        ("biffurcation-type", "Biffurcation method type. 0 - a(3)/a(1) > biffurcation_threshold, 1 - a1 > biffurcation_threshold, 2 - combines both conditions, 3 - no biffurcation at all.", value<int>()->default_value(to_string(mdl.biffurcation_type)))
        ("b,biffurcation-threshold", "Biffuraction threshold for first biffurcation type: a(3)/a(1) > kcrit", value<double>()->default_value(to_string(mdl.biffurcation_threshold)))
        ("biffurcation-threshold-2", "Biffuraction threshold for second biffurcation type: a(1) > kcrit", value<double>()->default_value(to_string(mdl.biffurcation_threshold_2)))
        ("biffurcation-min-distance", "Minimal distance between adjacent biffurcation points. In other words, if lenght of branch is greater of specified value, only than it can biffurcate. Used for reducing numerical noise.", value<double>()->default_value(to_string(mdl.biffurcation_min_dist)))
        ("biffurcation-angle", "Angle between branches in biffurcation point. Default is Pi/5 which is theoretical value.", value<double>()->default_value(to_string(mdl.biffurcation_angle)))
        ("growth-type", "Specifies growth type used for evaluation of next point(its dirrection and lenght): 0 - using arctan(a2/a1) method, 1 - by preavuating {dx, dy}. For more details please Piotr Morawiecki", value<int>()->default_value(to_string(mdl.growth_type)))
        ("growth-threshold", "Growth of branch stops if a(1) < growth-threshold.", value<double>()->default_value(to_string(mdl.growth_threshold)))
        ("growth-min-distance", "Growth of branch will be with constant speed(ds by each step) if its lenght is less then this value. uUsed for reducing numerical noise", value<double>()->default_value(to_string(mdl.growth_min_distance)))
        ("ds", "ds - value of growth proportionality: dl = ds*a(1)^eta", value<double>()->default_value(to_string(mdl.ds)));

        //Integration parameters
        options.add_options("Series parameters integral")
        ("weight-exp", "Parameter used in integration weight function. For more details please see FreeFem implementation.", value<double>()->default_value(to_string(mdl.integr.exponant)))
        ("integration-radius", "Radius of integration around tips for evaluation of series parameters", value<double>()->default_value(to_string(mdl.integr.integration_radius)))
        ("weight-radius", "Parameter used in integration weight function. Weight radius parameter. For more details please see FreeFem implementation.", value<double>()->default_value(to_string(mdl.integr.weigth_func_radius)));

        //Mesh parameters
        options.add_options("Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).")
        ("mesh-exp", "Parameter used in mesh refinment function. Reduce(small values) or increase(large values) slope on edge.", 
            value<double>()->default_value(to_string(mdl.mesh.exponant)))
        ("mesh-sigma", "Parameter used in mesh refinment function. Shifts maximal slope region relatively to r0 value.", 
            value<double>()->default_value(to_string(mdl.mesh.sigma)))
        ("A,mesh-max-area", "Constraints maximal area of triangle element", 
            value<double>()->default_value(to_string(mdl.mesh.max_area)))
        ("mesh-min-area", "Constraints minimal area of triangle element.", 
            value<double>()->default_value(to_string(mdl.mesh.min_area)))
        ("refinment-radius", "r0 - refinment radius from this formula. Corresponds to position of maximal slope.", 
            value<double>()->default_value(to_string(mdl.mesh.refinment_radius)))
        ("q,mesh-min-angle", "Constraints minimal angle of triangle element.", 
            value<double>()->default_value(to_string(mdl.mesh.min_angle)))
        ("e,eps", "Width of branch. Every branch is divided and joined into one big edge which covers all geometry. Width of divided branch corresponds to eps. Such solution is constrain of Deal.II library.", 
            value<double>()->default_value(to_string(mdl.mesh.eps)))
        ("static-refinment-steps", "Number of mesh refinment steps used by Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(mdl.mesh.static_refinment_steps)));

        options.add_options("Solver Parameters")
        ("quadrature-degree", "Quadrature polynomials degree used in numerical integration of Deal.II solver.", 
            value<int>()->default_value(to_string(mdl.solver_params.quadrature_degree)))
        ("refinment-fraction", "Fraction(percent from total, 0.01 corresponds to 1%) of refined mesh elements using Deal.II adaptive mesh capabilities.", 
            value<double>()->default_value(to_string(mdl.solver_params.refinment_fraction)))
        ("adaptive-refinment-steps", "Number of refinment steps used by adaptive Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(mdl.solver_params.adaptive_refinment_steps)));


        options.parse_positional({"input"});

        auto result = options.parse(argc, argv);

        if (result.count("help"))
            cout << options.help({
                "Basic", 
                "File interface", 
                "Logs",
                "Simulation parameters",
                "Border geometry parameters",
                "Model parameters",
                "Series parameters integral",
                "Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).",
                "Solver Parameters"}) 
                << endl;

        if (result.count("version"))
            print_version();

        return result;
    }

} // namespace River