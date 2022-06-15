/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

#include "io.hpp"
#include "json.hpp"
#include "version.hpp"
///\cond
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <tuple>
///\endcond



using namespace std;
using json = nlohmann::json;

namespace River
{
    string ProgramTitle()
    {
        return "RiverSim 2018";
    }  

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
        cout << ProgramTitle() + " version: " 
            + version_string() << endl;
    }

    void print(const bool flag, const string str)
    {
    	if (flag) cout << str << endl;
    }

    string bool_to_string(bool b)
    {
        if (b) 
            return "true";
        else 
            return "false";
    }

    cxxopts::ParseResult process_program_options(int argc, char *argv[])
    {
        using namespace cxxopts;

        const Model model;
        const ProgramOptions prog_opt;
        
        Options options(
            "./riversim", ProgramTitle() +" v" + version_string() + " is used to simulate river growth and some other calculations according to \n"
            "Laplace model(for more details pls see references e.g Piotr Morawiecki work.)\n" + 
            "Some documentation is placed also here https://okmechak.github.io/RiverSim/\n\n" + 
            "Standard ways of using program:\n"
            "\t\t./riversim [options(optionally)] [input_file(optionally)]\n" + 
            "\tForward river growth: \n" + 
            "\t\t./riversim -n 100 --eta=0.75 --save-each-step --bifurcation-threshold=-0.4 -o test\n"
            "\tYou can contiue simmulation later from last point(or any other) by typing:\n" + 
            "\t\t./riversim -n 100 -o test_continuation test_99.json\n" +
            "\tNow we can also run bacward simmulation using previous results, by using option \"-t\":\n" + 
            "\t\t./riversim -t 1 -n 99999 test_continuation.json\n" +
            "\tProgram also produces vtk and msh files from different stages of program running which can be viewed from Paraview and Gmsh.\n\n" + 
            "Creation of custom input file:\n" + 
            "\tInput file is of json format, and should contain at least next object: Model(for its format please see output json file).\n" + 
            "\tAlso it can contain Boundary and Rivers(this object can't be setuped without Boundary object).\n" +  
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
        ("o,output", "Name of simulation data and state of program data.", 
            value<string>()->default_value(prog_opt.output_file_name) )
        ("save-each-step", "Save each step of simulation in separate file: simdata_1.json, simdata_2.json.. ", 
            value<bool>()->default_value(bool_to_string(prog_opt.save_each_step)) )
        ("vtk", "Outputs VTK file of Deal.II solution", 
            value<bool>()->default_value(bool_to_string(prog_opt.save_vtk)) )
        ("input",
            "input simaultion data, boundary, rivers, model parameters. It has very similar structure as output json of program.", 
            value<string>() );
        
        //prints and logs
        options.add_options("Logs")
        ("V,verbose", "print detailed log to terminal.", 
            value<bool>()->default_value(bool_to_string(prog_opt.verbose)) )
        ("D,debug", "save additional output files for each simulation step.", 
            value<bool>()->default_value(bool_to_string(prog_opt.debug)) );

        //Simulation parameters
        options.add_options("Simulation parameters")
        ("n,number-of-steps", "Number of steps to simulate.", 
            value<unsigned>()->default_value(to_string(model.number_of_steps)))
        ("m,maximal-river-height", "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.", 
            value<double>()->default_value(to_string(model.maximal_river_height)))
        ("t,simulation-type", "Type of simulation: 0 - forward linear river growth, 1 - forward non linear river growth, 2 - backward, 3 - test", 
            value<unsigned>()->default_value(to_string(model.simulation_type)))
        ("number-of-backward-steps", "Number of backward steps simulations used in backward simulation type.", 
            value<unsigned>()->default_value(to_string(model.number_of_backward_steps)));
        
        //Geometry parameters
        options.add_options("Boundary geometry parameters")
        ("dx", "dx - shift of initial river position from beginning of coordinates in X dirrections.", 
            value<double>()->default_value(to_string(model.dx)))
        ("width", "Width of river rectangular region.", 
            value<double>()->default_value(to_string(model.width)))
        ("height", "Height of river rectangular region.", 
            value<double>()->default_value(to_string(model.height)))
        ("river-boundary-id", "Boundary condition Id set to river boundaries.", 
            value<unsigned>()->default_value(to_string(model.river_boundary_id)))
        ("river-width", "Width of branch. Every branch is divided and joined into one big edge which covers all geometry. Width of divided branch corresponds to eps. Such solution is constrain of Deal.II library.", 
            value<double>()->default_value(to_string(model.river_width)) );
        
        //Model parameter
        options.add_options("Model parameters")
        ("eta", "Power of a1^eta used in growth of river.", 
            value<double>()->default_value(to_string(model.eta)) )
        ("b,bifurcation-type", "Bifurcation method type, 0 - no bif, 1 - a(3)/a(1) > bifurcation_threshold, 2 - a1 > bifurcation_threshold, 3 - combines both conditions.", 
            value<unsigned>()->default_value(to_string(model.bifurcation_type)) )
        ("bifurcation-threshold", "Bifuraction threshold.", 
            value<double>()->default_value(to_string(model.bifurcation_threshold)) )
        ("bifurcation-min-distance", "Minimal distance between adjacent bifurcation points. In other words, branch can bifurcate only when it is longer than minimal distance. Used for reducing numerical noise.", 
            value<double>()->default_value(to_string(model.bifurcation_min_dist)) )
        ("bifurcation-angle", "Angle between branches in bifurcation point. Default is Pi/5 which is theoretical value.", 
            value<double>()->default_value(to_string(model.bifurcation_angle)) )
        ("growth-type", "Specifies growth type used for evaluation of next point(its dirrection and lenght): 0 - using arctan(a2/a1) method, 1 - by preavuating {dx, dy}. For more details please Piotr Morawiecki", 
            value<unsigned>()->default_value(to_string(model.growth_type)) )
        ("growth-threshold", "Growth of branch stops if a(1) < growth-threshold.", 
            value<double>()->default_value(to_string(model.growth_threshold)) )
        ("growth-min-distance", "Growth of branch will be with constant speed(ds by each step) if its lenght is less then this value. uUsed for reducing numerical noise", 
            value<double>()->default_value(to_string(model.growth_min_distance)) )
        ("ds", "ds - value of growth proportionality: dl = ds*a(1)^eta", 
            value<double>()->default_value(to_string(model.ds)) );
        
        //Integration parameters
        options.add_options("Series parameters integral")
        ("weight-exp", "Parameter used in integration weight function. For more details please see FreeFem implementation.", 
            value<double>()->default_value(to_string(model.integr.exponant)) )
        ("integration-radius", "Radius of integration around tips for evaluation of series parameters", 
            value<double>()->default_value(to_string(model.integr.integration_radius)) )
        ("weight-radius", "Parameter used in integration weight function. Weight radius parameter. For more details please see FreeFem implementation.", 
            value<double>()->default_value(to_string(model.integr.weigth_func_radius)) );

        //Mesh parameters
        options.add_options("Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).")
        ("mesh-exp", "Parameter used in mesh refinment function. Reduce(small values) or increase(large values) slope on edge.", 
            value<double>()->default_value(to_string(model.mesh_params.exponant)) )
        ("mesh-sigma", "Parameter used in mesh refinment function. Shifts maximal slope region relatively to r0 value.", 
            value<double>()->default_value(to_string(model.mesh_params.sigma)) )
        ("A,mesh-max-area", "Constraints maximal area of triangle element", 
            value<double>()->default_value(to_string(model.mesh_params.max_area)) )
        ("mesh-min-area", "Constraints minimal area of triangle element.", 
            value<double>()->default_value(to_string(model.mesh_params.min_area)) )
        ("mesh-max-edge", "Constraints maximal edge lenght of triangle element.", 
            value<double>()->default_value(to_string(model.mesh_params.max_edge)) )
        ("mesh-min-edge", "Constraints minimal edge lenght of triangle element. It can has higher priority then adaptive function and other mesh constraints.", 
            value<double>()->default_value(to_string(model.mesh_params.min_edge)) )
        ("mesh-ratio", "Constraints ratio of triangle element. Relation of longest edge to shortes edge of triangle. Has similar effect and limitation as mesh-min-angle option. Value 2 correspond to minimal nagle 30. So be carefull with setting values less than 2.", 
            value<double>()->default_value(to_string(model.mesh_params.ratio)) )
        ("refinment-radius", "r0 - refinment radius from this formula. Corresponds to position of maximal slope.", 
            value<double>()->default_value(to_string(model.mesh_params.refinment_radius)) )
        ("q,mesh-min-angle", "Constraints minimal angle of triangle element.", 
            value<double>()->default_value(to_string(model.mesh_params.min_angle)) )
        ("smoothness-degree", "Smoothnes minimal degree. This value sets threshold for degree between adjacent points below which it should be ignored. This creates smaller mesh.", 
            value<double>()->default_value(to_string(model.mesh_params.smoothness_degree)) )
        ("ignored-smoothness-length", "Smoothnes minimal length. This value sets threshold for length where smoothnest near tip will be ignored. Ideally it should be bigger then integration radius.", 
            value<double>()->default_value(to_string(model.mesh_params.ignored_smoothness_length)) );
        
        
        //Solver parameters
        options.add_options("Solver Parameters")
        ("quadrature-degree", "Quadrature polynomials degree used in numerical integration of Deal.II solver.", 
            value<unsigned>()->default_value(to_string(model.solver_params.quadrature_degree)))
        ("iteration-steps", "Number of iterations used by Deal.II solver.", 
            value<unsigned>()->default_value(to_string(model.solver_params.num_of_iterrations)))
        ("tol", "Tollerance used by Deal.II solver.", 
            value<double>()->default_value(to_string(model.solver_params.tollerance)))
        ("refinment-fraction", "Fraction(percent from total, 0.01 corresponds to 1%) of refined mesh elements using Deal.II adaptive mesh capabilities.", 
            value<double>()->default_value(to_string(model.solver_params.refinment_fraction)))
        ("static-refinment-steps", "Number of mesh refinment steps used by Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(model.solver_params.static_refinment_steps)) )
        ("adaptive-refinment-steps", "Number of refinment steps used by adaptive Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(model.solver_params.adaptive_refinment_steps)))
        ("max-dist", "Used by non-euler solver.", 
            value<double>()->default_value(to_string(model.solver_params.max_distance)))
        ("f,field-value", "Value of outter force used for Poisson equation(Right-hand side value)", 
            value<double>()->default_value(to_string(model.solver_params.field_value)) );
        
        options.parse_positional({"input"});
        
        auto result = options.parse(argc, argv);
        
        if (result.count("help"))
        {
            cout.precision(17);
            cout << options.help({
                "Basic", 
                "File interface", 
                "Logs",
                "Simulation parameters",
                "Boundary geometry parameters",
                "Model parameters",
                "Series parameters integral",
                "Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).",
                "Solver Parameters"}) 
                << endl;
        }

        if (result.count("version"))
            print_version();

        if (!result.count("suppress-signature") && result.count("verbose"))
            print_ascii_signature();
        
        return result;
    }

    Model getModel(const cxxopts::ParseResult& vm)
    {
        Model model;
        //program options
        if (vm.count("simulation-type"))
            model.simulation_type = vm["simulation-type"].as<unsigned>();
        if (vm.count("number-of-steps")) 
            model.number_of_steps = vm["number-of-steps"].as<unsigned>();
        if (vm.count("maximal-river-height"))
            model.maximal_river_height = vm["maximal-river-height"].as<double>();
        if (vm.count("number-of-backward-steps"))
            model.number_of_backward_steps = vm["number-of-backward-steps"].as<unsigned>();

        //mesh options
        if (vm.count("refinment-radius")) model.mesh_params.refinment_radius = vm["refinment-radius"].as<double>();
        if (vm.count("mesh-exp")) model.mesh_params.exponant = vm["mesh-exp"].as<double>();
        if (vm.count("mesh-sigma")) model.mesh_params.sigma = vm["mesh-sigma"].as<double>();
        if (vm.count("mesh-max-area")) model.mesh_params.max_area = vm["mesh-max-area"].as<double>();
        if (vm.count("mesh-min-area")) model.mesh_params.min_area = vm["mesh-min-area"].as<double>();
        if (vm.count("mesh-min-angle")) model.mesh_params.min_angle = vm["mesh-min-angle"].as<double>();
        if (vm.count("mesh-max-edge")) model.mesh_params.max_edge = vm["mesh-max-edge"].as<double>();
        if (vm.count("mesh-min-edge")) model.mesh_params.min_edge = vm["mesh-min-edge"].as<double>();
        if (vm.count("mesh-ratio")) model.mesh_params.ratio = vm["mesh-ratio"].as<double>();
        if (vm.count("smoothness-degree")) model.mesh_params.smoothness_degree = vm["smoothness-degree"].as<double>();
        if (vm.count("ignored-smoothness-length")) model.mesh_params.ignored_smoothness_length = vm["ignored-smoothness-length"].as<double>();
        
        //integration options
        if (vm.count("integration-radius")) model.integr.integration_radius = vm["integration-radius"].as<double>();
        if (vm.count("weight-radius")) model.integr.weigth_func_radius = vm["weight-radius"].as<double>();
        if (vm.count("weight-exp")) model.integr.exponant = vm["weight-exp"].as<double>();

        //solver options
        if (vm.count("tol")) model.solver_params.tollerance = vm["tol"].as<double>();
        if (vm.count("iteration-steps")) model.solver_params.num_of_iterrations = vm["iteration-steps"].as<unsigned>();
        if (vm.count("adaptive-refinment-steps")) model.solver_params.adaptive_refinment_steps = vm["adaptive-refinment-steps"].as<unsigned>();
        if (vm.count("static-refinment-steps")) model.solver_params.static_refinment_steps = vm["static-refinment-steps"].as<unsigned>();
        if (vm.count("refinment-fraction")) model.solver_params.refinment_fraction = vm["refinment-fraction"].as<double>();
        if (vm.count("quadrature-degree")) model.solver_params.quadrature_degree = vm["quadrature-degree"].as<unsigned>();
        if (vm.count("max-dist")) model.solver_params.max_distance = vm["max-dist"].as<double>();
        if (vm.count("field-value")) model.solver_params.field_value = vm["field-value"].as<double>();

        //model parameters
        //geometry
        if (vm.count("dx")) model.dx = vm["dx"].as<double>();
        if (vm.count("width")) model.width = vm["width"].as<double>();
        if (vm.count("height")) model.height = vm["height"].as<double>();
        if (vm.count("river-boundary-id")) model.river_boundary_id = vm["river-boundary-id"].as<unsigned>();
        if (vm.count("river-width")) model.river_width = vm["river-width"].as<double>();
    
        if (vm.count("eta")) model.eta = vm["eta"].as<double>();
        if (vm.count("bifurcation-type")) model.bifurcation_type = vm["bifurcation-type"].as<unsigned>();
        if (vm.count("bifurcation-threshold")) model.bifurcation_threshold = vm["bifurcation-threshold"].as<double>();
        if (vm.count("bifurcation-angle")) model.bifurcation_angle = vm["bifurcation-angle"].as<double>();
        if (vm.count("bifurcation-min-distance")) model.bifurcation_min_dist = vm["bifurcation-min-distance"].as<double>();
        if (vm.count("growth-type")) model.growth_type = vm["growth-type"].as<unsigned>();
        if (vm.count("growth-threshold")) model.growth_threshold = vm["growth-threshold"].as<double>();
        if (vm.count("growth-min-distance")) model.growth_min_distance = vm["growth-min-distance"].as<double>();
        if (vm.count("ds")) model.ds = vm["ds"].as<double>();

        if(vm.count("input")) Open(model, vm["input"].as<string>());

        model.CheckParametersConsistency();

        return model;
    }

    ProgramOptions getProgramOptions(const cxxopts::ParseResult& vm)
    {
        ProgramOptions prog_opt;   

        prog_opt.save_vtk = vm.count("vtk");
        if (vm.count("verbose")) prog_opt.verbose = vm["verbose"].as<bool>();
        if (vm.count("debug")) prog_opt.debug = vm["debug"].as<bool>();
        if (vm.count("output")) prog_opt.output_file_name = vm["output"].as<string>();
        if (vm.count("input")) prog_opt.input_file_name = vm["input"].as<string>();
        prog_opt.save_each_step = vm.count("save-each-step");

        return prog_opt;
    }

    //ProgramOptions
    ostream& operator<<(ostream& write, const ProgramOptions & po)
    {
        write << "\t save_vtk = "                 << po.save_vtk         << endl;
        write << "\t verbose = "                  << po.verbose          << endl;
        write << "\t output_file_name = "         << po.output_file_name << endl;
        write << "\t input_file_name = "          << po.input_file_name  << endl;
        write << "\t save each step = "              << po.save_each_step   << endl; 
        return write;
    }

    bool ProgramOptions::operator==(const ProgramOptions& po) const
    {
        return 
            save_vtk == po.save_vtk
            && save_each_step == po.save_each_step
            && verbose == po.verbose
            //&& output_file_name == po.output_file_name
            && input_file_name == po.input_file_name;
    }

    //point
    //Point
    void to_json(json& j, const Point& p) 
    {
        j = json{
            {"x", p.x}, 
            {"y", p.y}};
    }
    void from_json(const json& j, Point& p) 
    {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    //Polar
    void to_json(json& j, const Polar& p) 
    {
        j = json{
            {"r", p.r}, 
            {"phi", p.phi}};
    }
    void from_json(const json& j, Polar& p) 
    {
        j.at("r").get_to(p.r);
        j.at("phi").get_to(p.phi);
    }

    //Boundary
    //BoundaryCondition
    void to_json(json& j, const BoundaryCondition& bc) 
    {
        string type_str = "Dirichlet";
        if (bc.type == NEUMAN)
            type_str = "Neuman";

        j = json{
            {"type", type_str}, 
            {"value", bc.value}};
    }
    void from_json(const json& j, BoundaryCondition& bc) 
    {
        string type_str;

        j.at("type").get_to(type_str);

        if(type_str == "Dirichlet")
            bc.type = DIRICHLET;
        else if(type_str == "Neuman")
            bc.type = NEUMAN;
        else
            throw Exception("Unknown boundary type: " + type_str);

        j.at("value").get_to(bc.value);
    }

    //boundary conditions
    void to_json(json& j, const BoundaryConditions& bc)
    {
        j = json{t_BoundaryConditions(bc)};
    }

    void from_json(const json& j, BoundaryConditions& bc)
    {
        t_BoundaryConditions bc_simple;
        j.get_to(bc_simple);
        for(const auto&[key, value]: bc_simple)
            bc[key] = value;
    }

    //Line
    void to_json(json& j, const Line& line) 
    {
        j = json{
            {"p1", line.p1}, 
            {"p2", line.p2},
            {"boundary_id", line.boundary_id}};
    }

    void from_json(const json& j, Line& line) 
    {
        j.at("p1").get_to(line.p1);
        j.at("p2").get_to(line.p2);
        j.at("boundary_id").get_to(line.boundary_id);
    }

    //Boundary
    void to_json(json& j, const Boundary& boundary) 
    {
        j = json{
            {"vertices", boundary.vertices}, 
            {"lines", boundary.lines}};
    }
    void from_json(const json& j, Boundary& boundary) 
    {
        j.at("vertices").get_to(boundary.vertices);
        j.at("lines").get_to(boundary.lines);
    }

    //Region
    void to_json(json& j, const Region& region) 
    {
        j = json{(t_Region)region};
    }
    void from_json(const json& j, Region& region) 
    {
        t_Region b;
        j.get_to(b);
        for(const auto&[key, value]: b)
            region[key] = value;
    }

    //Sources
    void to_json(json& j, const Sources& sources) 
    {
        j = json{(t_Sources)sources};
    }
    void from_json(const json& j, Sources& sources) 
    {
        t_Sources s;
        j.get_to(s);
        for(const auto&[key, value]: s)
            sources[key] = value;
    }

    //Rivers
    //Branch
    void to_json(json& j, const Branch& branch) 
    {
        j = json{
            {"vertices", (t_PointList)branch.vertices}, 
            {"lines", (t_LineList)branch.lines},
            {"source_angle", branch.SourceAngle()}
        };
    }

    void from_json(const json& j, Branch& branch) 
    {
        j.at("vertices").get_to(branch.vertices);
        j.at("lines").get_to(branch.lines);
        double source_angle;
        j.at("source_angle").get_to(source_angle);
        branch.SetSourceAngle(source_angle);
    }

    //Rivers
    void to_json(json& j, const Rivers& rivers) 
    {
        json branches;
        for(const auto&[id, branch]: rivers)
        {
            branches.push_back({
                {"id", id},
                {"branch", branch}
            });
        }

        j = json{
            {"branches", branches}, 
            {"relations", rivers.branches_relation}};
    }
    void from_json(const json& j, Rivers& rivers) 
    {
        for(const auto&[key, value]: j.at("branches").items())
        {
            t_branch_id id;
            value.at("id").get_to(id);

            Branch branch;
            value.at("branch").get_to(branch);
            
            rivers.AddBranch(branch, id);
        }

        j.at("relations").get_to(rivers.branches_relation);
    }

    //PhysModel
    //SeriesParameters
    
    void to_json(json& j, const SeriesParameters& id_series_params) 
    {
        j = json{(t_SeriesParameters)id_series_params};
    }
    void from_json(const json& j, SeriesParameters& id_series_params) 
    {
        t_SeriesParameters sp;
        j.get_to(sp);
        for(const auto&[key, value]: sp)
            id_series_params[key] = value;
    }

    //SimulationData
    
    //BackwardData
    void to_json(json& j, const BackwardData& data) 
    {
        j = json{
            {"a1", data.a1},
            {"a2", data.a2},
            {"a3", data.a3} ,
            {"init_point", data.init},
            {"backward_point", data.backward},
            {"backward_forward_point", data.backward_forward},
            {"branch_lenght_diff", data.branch_lenght_diff}};
    }
    void from_json(const json& j, BackwardData& data) 
    {
        j.at("a1").get_to(data.a1);
        j.at("a2").get_to(data.a2);
        j.at("a3").get_to(data.a3);

        j.at("init_point").get_to(data.init);
        j.at("backward_point").get_to(data.backward);
        j.at("backward_forward_point").get_to(data.backward_forward);

        j.at("branch_lenght_diff").get_to(data.branch_lenght_diff);
    }
    
    //ProgramOptions
    void to_json(json& j, const ProgramOptions& data) 
    {
        j = json{
            //{"simulation_type", data.simulation_type},
            //{"number_of_steps", data.number_of_steps},
            //{"maximal_river_height", data.maximal_river_height},
            //{"number_of_backward_steps", data.number_of_backward_steps},
            {"save_vtk", data.save_vtk},
            {"save_each_step", data.save_each_step},
            {"verbose", data.verbose},
            {"debug", data.debug},
            {"output_file_name", data.output_file_name},
            {"input_file_name", data.input_file_name}};
    }
    void from_json(const json& j, ProgramOptions& data) 
    {
        //if(j.count("simulation_type")) j.at("simulation_type").get_to(data.simulation_type);
        //if(j.count("number_of_steps")) j.at("number_of_steps").get_to(data.number_of_steps);
        //if(j.count("maximal_river_height")) j.at("maximal_river_height").get_to(data.maximal_river_height);
        //if(j.count("number_of_backward_steps")) j.at("number_of_backward_steps").get_to(data.number_of_backward_steps);
        if(j.count("save_vtk")) j.at("save_vtk").get_to(data.save_vtk);
        if(j.count("save_each_step")) j.at("save_each_step").get_to(data.save_each_step);
        if(j.count("verbose")) j.at("verbose").get_to(data.verbose);
        if(j.count("debug")) j.at("debug").get_to(data.debug);
        if(j.count("input_file_name")) j.at("input_file_name").get_to(data.input_file_name);
        //j.at("output_file_name").get_to(data.output_file_name);
    }

    //MeshParams
    void to_json(json& j, const MeshParams& data) 
    {
        j = json{
            {"refinment_radius", data.refinment_radius},
            {"exponant", data.exponant},
            {"sigma", data.sigma},
            {"min_area", data.min_area},
            {"max_area", data.max_area},
            {"min_angle", data.min_angle},
            {"max_edge", data.max_edge},
            {"min_edge", data.min_edge},
            {"ratio", data.ratio},
            {"smoothness_degree", data.smoothness_degree},
            {"ignored_smoothness_length", data.ignored_smoothness_length}};
    }

    void from_json(const json& j, MeshParams& data) 
    {
        if(j.count("refinment_radius")) j.at("refinment_radius").get_to(data.refinment_radius);
        if(j.count("exponant")) j.at("exponant").get_to(data.exponant);
        if(j.count("sigma")) j.at("sigma").get_to(data.sigma);
        if(j.count("min_area")) j.at("min_area").get_to(data.min_area);
        if(j.count("max_area")) j.at("max_area").get_to(data.max_area);
        if(j.count("min_angle")) j.at("min_angle").get_to(data.min_angle);
        if(j.count("max_edge")) j.at("max_edge").get_to(data.max_edge);
        if(j.count("min_edge")) j.at("min_edge").get_to(data.min_edge);
        if(j.count("ratio")) j.at("ratio").get_to(data.ratio);
        if(j.count("smoothness_degree")) j.at("smoothness_degree").get_to(data.smoothness_degree);
        if(j.count("ignored_smoothness_length")) j.at("ignored_smoothness_length").get_to(data.ignored_smoothness_length);
    }

    //IntegrationParams
    void to_json(json& j, const IntegrationParams& data) 
    {
        j = json{
            {"weigth_func_radius", data.weigth_func_radius},
            {"integration_radius", data.integration_radius},
            {"exponant", data.exponant}};
    }
    void from_json(const json& j, IntegrationParams& data) 
    {
        if(j.count("weigth_func_radius")) j.at("weigth_func_radius").get_to(data.weigth_func_radius);
        if(j.count("integration_radius")) j.at("integration_radius").get_to(data.integration_radius);
        if(j.count("exponant")) j.at("exponant").get_to(data.exponant);
    }

    //SolverParams
    void to_json(json& j, const SolverParams& data) 
    {
        j = json{
            {"tollerance", data.tollerance},
            {"num_of_iterrations", data.num_of_iterrations},
            {"adaptive_refinment_steps", data.adaptive_refinment_steps},
            {"static_refinment_steps", data.static_refinment_steps},
            {"refinment_fraction", data.refinment_fraction},
            {"quadrature_degree", data.quadrature_degree},
            {"renumbering_type", data.renumbering_type},
            {"max_distance", data.max_distance},
            {"field_value", data.field_value}};
    }
    
    void from_json(const json& j, SolverParams& data) 
    {
        if(j.count("tollerance")) j.at("tollerance").get_to(data.tollerance);
        if(j.count("num_of_iterrations")) j.at("num_of_iterrations").get_to(data.num_of_iterrations);
        if(j.count("adaptive_refinment_steps")) j.at("adaptive_refinment_steps").get_to(data.adaptive_refinment_steps);
        if(j.count("static_refinment_steps")) j.at("static_refinment_steps").get_to(data.static_refinment_steps);
        if(j.count("refinment_fraction")) j.at("refinment_fraction").get_to(data.refinment_fraction);
        if(j.count("quadrature_degree")) j.at("quadrature_degree").get_to(data.quadrature_degree);
        if(j.count("renumbering_type")) j.at("renumbering_type").get_to(data.renumbering_type);
        if(j.count("max_distance")) j.at("max_distance").get_to(data.max_distance);
        if(j.count("field_value")) j.at("field_value").get_to(data.field_value);
    }

    //Model
    void to_json(json& j, const Model& data) 
    {
        j = json{
            {"version", version_string()},
            {"region", data.region},
            {"sources", data.sources},
            {"rivers", data.rivers},
            {"boundary_conditions", data.boundary_conditions},
            {"mesh_parameters", data.mesh_params},
            {"integration_parameters", data.integr},
            {"solver_parameters", data.solver_params},
            {"series_parameters", data.series_parameters},
            {"simulation_data", data.sim_data},
            {"backward_data", data.backward_data},
            {"simulation_type", data.simulation_type},
            {"number_of_steps", data.number_of_steps},
            {"maximal_river_height", data.maximal_river_height},
            {"number_of_backward_steps", data.number_of_backward_steps},
            {"dx", data.dx},
            {"width", data.width},
            {"height", data.height},
            {"river_boundary_id", data.river_boundary_id},
            {"river_width", data.river_width},
            {"eta", data.eta},
            {"bifurcation_type", data.bifurcation_type},
            {"bifurcation_threshold", data.bifurcation_threshold},
            {"bifurcation_min_dist", data.bifurcation_min_dist},
            {"bifurcation_angle", data.bifurcation_angle},
            {"growth_type", data.growth_type},
            {"growth_threshold", data.growth_threshold},
            {"growth_min_distance", data.growth_min_distance},
            {"ds", data.ds}};
    }
    void from_json(const json& j, Model& data) 
    {   
        if(j.count("region")) j.at("region").get_to(data.region);
        if(j.count("sources")) j.at("sources").get_to(data.sources);
        if(j.count("rivers")) j.at("rivers").get_to(data.rivers);
        if(j.count("boundary_conditions")) j.at("boundary_conditions").get_to(data.boundary_conditions);

        if(j.count("mesh_parameters")) j.at("mesh_parameters").get_to(data.mesh_params);
        if(j.count("integration_parameters")) j.at("integration_parameters").get_to(data.integr);
        if(j.count("solver_parameters")) j.at("solver_parameters").get_to(data.solver_params);
        
        if(j.count("series_parameters")) j.at("series_parameters").get_to(data.series_parameters);
        if(j.count("simulation_data")) j.at("simulation_data").get_to(data.sim_data);
        if(j.count("backward_data")) j.at("backward_data").get_to(data.backward_data);
        if(j.count("simulation_type")) j.at("simulation_type").get_to(data.simulation_type);
        if(j.count("number_of_steps")) j.at("number_of_steps").get_to(data.number_of_steps);
        if(j.count("maximal_river_height")) j.at("maximal_river_height").get_to(data.maximal_river_height);
        if(j.count("number_of_backward_steps")) j.at("number_of_backward_steps").get_to(data.number_of_backward_steps);
        if(j.count("dx")) j.at("dx").get_to(data.dx);
        if(j.count("width")) j.at("width").get_to(data.width);
        if(j.count("height")) j.at("height").get_to(data.height);
        if(j.count("river_boundary_id")) j.at("river_boundary_id").get_to(data.river_boundary_id);
        if(j.count("river_width")) j.at("river_width").get_to(data.river_width);
        if(j.count("eta")) j.at("eta").get_to(data.eta);
        if(j.count("ds")) j.at("ds").get_to(data.ds);
        if(j.count("bifurcation_type")) j.at("bifurcation_type").get_to(data.bifurcation_type);
        if(j.count("bifurcation_threshold")) j.at("bifurcation_threshold").get_to(data.bifurcation_threshold);
        if(j.count("bifurcation_min_dist")) j.at("bifurcation_min_dist").get_to(data.bifurcation_min_dist);
        if(j.count("bifurcation_angle")) j.at("bifurcation_angle").get_to(data.bifurcation_angle);
        if(j.count("growth_type")) j.at("growth_type").get_to(data.growth_type);
        if(j.count("growth_threshold")) j.at("growth_threshold").get_to(data.growth_threshold);
        if(j.count("growth_min_distance")) j.at("growth_min_distance").get_to(data.growth_min_distance);
    }

    void Save(const Model& model, const string file_name)
    {
        if(file_name.length() == 0)
            throw Exception("Save: File name is not set.");

        ofstream out(file_name + ".json");
        if(!out) throw Exception("Save: Can't create file for write");

        out.precision(16);

        json j = {{"model", model}};

        out << setw(4) << j;

        out.close();        
    }

    void Open(Model& model, const string file_name)
    {
        ifstream in(file_name);
        if(!in) throw Exception("Open. Can't create file for read. : " + file_name);

        json j;
        in >> j;

        j.at("model").get_to(model);

        if (model.sources.empty())
            throw Exception("Open: sources from input file are empty");

        if (model.boundary_conditions.empty())
            throw Exception("Open: boundary_conditions from input file are empty");

        if (model.region.empty())
            throw Exception("Open: boundaries from input file are empty");

        if (model.rivers.empty())
            model.rivers.Initialize(model.region.GetSourcesIdsPointsAndAngles(model.sources));
    }
}//namespace River