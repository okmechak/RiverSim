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

    string bool_to_string(bool b)
    {
        if(b) return "true";
        else return "false";
    }

    cxxopts::ParseResult process_program_options(int argc, char *argv[])
    {
        using namespace cxxopts;

        const Model model;
        
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
            "\tAlso it can contain Boundary and Tree(this object can't be setuped without Boundary object).\n" +  
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
            value<string>()->default_value(model.prog_opt.output_file_name) )
        ("save-each-step", "Save each step of simulation in separate file: simdata_1.json, simdata_2.json.. ", 
            value<bool>()->default_value(bool_to_string(model.prog_opt.save_each_step)) )
        ("vtk", "Outputs VTK file of Deal.II solution", 
            value<bool>()->default_value(bool_to_string(model.prog_opt.save_vtk)) )
        ("input",
            "input simaultion data, boundary, tree, model parameters. It has very similar structure as output json of program.", 
            value<string>() );
        
        //prints and logs
        options.add_options("Logs")
        ("V,verbose", "print detailed log to terminal.", 
            value<bool>()->default_value(bool_to_string(model.prog_opt.verbose)) );

        //Simulation parameters
        options.add_options("Simulation parameters")
        ("n,number-of-steps", "Number of steps to simulate.", 
            value<unsigned>()->default_value(to_string(model.prog_opt.number_of_steps)))
        ("m,maximal-river-height", "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.", 
            value<double>()->default_value(to_string(model.prog_opt.maximal_river_height)))
        ("t,simulation-type", "Type of simulation: 0 - forward river growth, 1 - backward river growth, 2 - Used for development purposes.", 
            value<unsigned>()->default_value(to_string(model.prog_opt.simulation_type)))
        ("number-of-backward-steps", "Number of backward steps simulations used in backward simulation type.", 
            value<unsigned>()->default_value(to_string(model.prog_opt.number_of_backward_steps)));
        
        //Geometry parameters
        options.add_options("Boundary geometry parameters")
        ("dx", "dx - shift of initial river position from beginning of coordinates in X dirrections.", 
            value<double>()->default_value(to_string(model.dx)) )
        ("width", "Width of river rectangular region.", 
            value<double>()->default_value(to_string(model.width)) )
        ("height", "Height of river rectangular region.", 
            value<double>()->default_value(to_string(model.height)) )
        ("river-boundary-id", "Boundary condition Id set to river boundaries.", 
            value<unsigned>()->default_value(to_string(model.river_boundary_id)) );
        
        //Model parameters
        options.add_options("Model parameters")
        ("f,field-value", "Value of outter force used for Poisson equation(Right-hand side value)", 
            value<double>()->default_value(to_string(model.field_value)) )
        ("eta", "Power of a1^eta used in growth of river.", 
            value<double>()->default_value(to_string(model.eta)) )
        ("bifurcation-type", "Bifurcation method type. 0 - a(3)/a(1) > bifurcation_threshold, 1 - a1 > bifurcation_threshold, 2 - combines both conditions, 3 - no bifurcation at all.", 
            value<unsigned>()->default_value(to_string(model.bifurcation_type)) )
        ("b,bifurcation-threshold", "Bifuraction threshold for first bifurcation type: a(3)/a(1) < kcrit", 
            value<double>()->default_value(to_string(model.bifurcation_threshold)) )
        ("bifurcation-threshold-2", "Biffuraction threshold for second bifurcation type: a(1) > kcrit", 
            value<double>()->default_value(to_string(model.bifurcation_threshold_2)) )
        ("bifurcation-min-distance", "Minimal distance between adjacent bifurcation points. In other words, if lenght of branch is greater of specified value, only than it can biffurcate. Used for reducing numerical noise.", 
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
            value<double>()->default_value(to_string(model.mesh.exponant)) )
        ("mesh-sigma", "Parameter used in mesh refinment function. Shifts maximal slope region relatively to r0 value.", 
            value<double>()->default_value(to_string(model.mesh.sigma)) )
        ("A,mesh-max-area", "Constraints maximal area of triangle element", 
            value<double>()->default_value(to_string(model.mesh.max_area)) )
        ("mesh-min-area", "Constraints minimal area of triangle element.", 
            value<double>()->default_value(to_string(model.mesh.min_area)) )

        ("mesh-max-edge", "Constraints maximal edge lenght of triangle element.", 
            value<double>()->default_value(to_string(model.mesh.max_edge)) )
        ("mesh-min-edge", "Constraints minimal edge lenght of triangle element. It can has higher priority then adaptive function and other mesh constraints.", 
            value<double>()->default_value(to_string(model.mesh.min_edge)) )
        ("mesh-ratio", "Constraints ratio of triangle element. Relation of longest edge to shortes edge of triangle. Has similar effect and limitation as mesh-min-angle option. Value 2 correspond to minimal nagle 30. So be carefull with setting values less than 2.", 
            value<double>()->default_value(to_string(model.mesh.ratio)) )

        ("refinment-radius", "r0 - refinment radius from this formula. Corresponds to position of maximal slope.", 
            value<double>()->default_value(to_string(model.mesh.refinment_radius)) )
        ("q,mesh-min-angle", "Constraints minimal angle of triangle element.", 
            value<double>()->default_value(to_string(model.mesh.min_angle)) )
        ("e,eps", "Width of branch. Every branch is divided and joined into one big edge which covers all geometry. Width of divided branch corresponds to eps. Such solution is constrain of Deal.II library.", 
            value<double>()->default_value(to_string(model.mesh.eps)) )
        ("static-refinment-steps", "Number of mesh refinment steps used by Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(model.mesh.static_refinment_steps)) );

        options.add_options("Solver Parameters")
        ("quadrature-degree", "Quadrature polynomials degree used in numerical integration of Deal.II solver.", 
            value<unsigned>()->default_value(to_string(model.solver_params.quadrature_degree)))
        ("iteration-steps", "Number of iterations used by Deal.II solver.", 
            value<unsigned>()->default_value(to_string(model.solver_params.num_of_iterrations)))
        ("tol", "Tollerance used by Deal.II solver.", 
            value<double>()->default_value(to_string(model.solver_params.tollerance)))
        ("refinment-fraction", "Fraction(percent from total, 0.01 corresponds to 1%) of refined mesh elements using Deal.II adaptive mesh capabilities.", 
            value<double>()->default_value(to_string(model.solver_params.refinment_fraction)))
        ("adaptive-refinment-steps", "Number of refinment steps used by adaptive Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(model.solver_params.adaptive_refinment_steps)));

        
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

    void SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm, Model& model)
    {
        //program options
        if (vm.count("simulation-type"))
            model.prog_opt.simulation_type = vm["simulation-type"].as<unsigned>();
        if (vm.count("number-of-steps")) 
            model.prog_opt.number_of_steps = vm["number-of-steps"].as<unsigned>();
        if (vm.count("maximal-river-height"))
            model.prog_opt.maximal_river_height = vm["maximal-river-height"].as<double>();
        if (vm.count("number-of-backward-steps"))
            model.prog_opt.number_of_backward_steps = vm["number-of-backward-steps"].as<unsigned>();
        if (vm.count("vtk")) model.prog_opt.save_vtk = true;
        if (vm.count("verbose")) model.prog_opt.verbose = vm["verbose"].as<bool>();
        if(vm.count("output")) model.prog_opt.output_file_name = vm["output"].as<string>();
        if(vm.count("input")) model.prog_opt.input_file_name = vm["input"].as<string>();
        if(vm.count("save-each-step")) model.prog_opt.save_each_step = true;

        //mesh options
        if (vm.count("refinment-radius")) model.mesh.refinment_radius = vm["refinment-radius"].as<double>();
        if (vm.count("mesh-exp")) model.mesh.exponant = vm["mesh-exp"].as<double>();
        if (vm.count("mesh-sigma")) model.mesh.sigma = vm["mesh-sigma"].as<double>();
        if (vm.count("static-refinment-steps")) model.mesh.static_refinment_steps = vm["static-refinment-steps"].as<unsigned>();
        if (vm.count("mesh-max-area")) model.mesh.max_area = vm["mesh-max-area"].as<double>();
        if (vm.count("mesh-min-area")) model.mesh.min_area = vm["mesh-min-area"].as<double>();
        if (vm.count("mesh-min-angle")) model.mesh.min_angle = vm["mesh-min-angle"].as<double>();
        if (vm.count("mesh-max-edge")) model.mesh.max_edge = vm["mesh-max-edge"].as<double>();
        if (vm.count("mesh-min-edge")) model.mesh.min_edge = vm["mesh-min-edge"].as<double>();
        if (vm.count("mesh-ratio")) model.mesh.ratio = vm["mesh-ratio"].as<double>();
        if (vm.count("eps")) model.mesh.eps = vm["eps"].as<double>();
        
        //integration options
        if (vm.count("integration-radius")) model.integr.integration_radius = vm["integration-radius"].as<double>();
        if (vm.count("weight-radius")) model.integr.weigth_func_radius = vm["weight-radius"].as<double>();
        if (vm.count("weight-exp")) model.integr.exponant = vm["weight-exp"].as<double>();

        //solver options
        if (vm.count("tol")) model.solver_params.tollerance = vm["tol"].as<double>();
        if (vm.count("iteration-steps")) model.solver_params.num_of_iterrations = vm["iteration-steps"].as<unsigned>();
        if (vm.count("adaptive-refinment-steps")) model.solver_params.adaptive_refinment_steps = vm["adaptive-refinment-steps"].as<unsigned>();
        if (vm.count("refinment-fraction")) model.solver_params.refinment_fraction = vm["refinment-fraction"].as<double>();
        if (vm.count("quadrature-degree")) model.solver_params.quadrature_degree = vm["quadrature-degree"].as<unsigned>();

        //model parameters
        //geometry
        if (vm.count("dx")) model.dx = vm["dx"].as<double>();
        if (vm.count("width")) model.width = vm["width"].as<double>();
        if (vm.count("height")) model.height = vm["height"].as<double>();
        if (vm.count("river-boundary-id")) model.river_boundary_id = vm["river-boundary-id"].as<unsigned>();
    
        if (vm.count("field-value")) model.field_value = vm["field-value"].as<double>();
        if (vm.count("eta")) model.eta = vm["eta"].as<double>();
        if (vm.count("bifurcation-type")) model.bifurcation_type = vm["bifurcation-type"].as<unsigned>();
        if (vm.count("bifurcation-threshold")) model.bifurcation_threshold = vm["bifurcation-threshold"].as<double>();
        if (vm.count("bifurcation-threshold-2")) model.bifurcation_threshold_2 = vm["bifurcation-threshold-2"].as<double>();
        if (vm.count("bifurcation-angle")) model.bifurcation_angle = vm["bifurcation-angle"].as<double>();
        if (vm.count("bifurcation-min-distance")) model.bifurcation_min_dist = vm["bifurcation-min-distance"].as<double>();
        if (vm.count("growth-type")) model.growth_type = vm["growth-type"].as<unsigned>();
        if (vm.count("growth-threshold")) model.growth_threshold = vm["growth-threshold"].as<double>();
        if (vm.count("growth-min-distance")) model.growth_min_distance = vm["growth-min-distance"].as<double>();
        if (vm.count("ds")) model.ds = vm["ds"].as<double>();
    }

    void Save(const Model& model, const string file_name)
    {
        if(file_name.length() == 0)
            throw Exception("Save: File name is not set.");

        ofstream out(file_name + ".json");
        if(!out) throw Exception("Save: Can't create file for write");

        out.precision(16);

        //Branches
        json branches;
        for(auto& [branch_id, branch_pointer]: model.tree.branches_index)
        {
            vector<pair<double, double>> vertices;
            vertices.reserve(branch_pointer->Size());
            for(auto& vertice: *branch_pointer)
                vertices.push_back({vertice.x, vertice.y});

            branches.push_back({
                {"SourceVertice", {branch_pointer->SourcePoint().x , branch_pointer->SourcePoint().y}},
                {"SourceAngle", branch_pointer->SourceAngle()},
                {"Vertices", vertices},
                {"Id", branch_id}});
        }
        
        //Boundaries
        json jboundaries;
        for(auto& [boundary_id, simple_boundary]: model.border)
        {  
            vector<pair<double, double>> vertices;
            vertices.reserve(simple_boundary.lines.size());
            for(auto& vertice: simple_boundary.vertices)
                vertices.push_back({vertice.x, vertice.y});
            
            vector<tuple<t_vert_pos, t_vert_pos, unsigned>> lines;
            lines.reserve(simple_boundary.lines.size());
            for(auto& line : simple_boundary.lines)
                lines.push_back({line.p1, line.p2, line.boundary_id});

            jboundaries.push_back({
                {"Vertices", vertices},
                {"Lines", lines},
                {"InnerBoundary", simple_boundary.inner_boundary},
                {"Hole", {simple_boundary.hole.x, simple_boundary.hole.y}},
                {"Name", simple_boundary.name},
                {"Id", boundary_id}
            });   
        }

        //implementation with json
        json j = {
            {"Description", "RiverSim simulation data and state of program. All coordinates are in normal cartesian coordinate system and by default are x > 0 and y > 0. Default values of simulation assumes that coordinates values will be of order 0 - 200. Greater values demands a lot of time to run, small are not tested(Problem of scalling isn't resolved yet TODO)."},
            {"Version", version_string()},

            {"RuntimeInfo", {
                {"Description", "Units are in seconds."},
                {"StartDate",  model.timing.CreationtDate()},
                {"EndDate",  model.timing.CurrentDate()},
                {"TotalTime",  model.timing.Total()},
                {"EachCycleTime",  model.timing.records}}},

            {"Model", {
                {"Description", "All model parameters. Almost all options are described in program options: ./riversim -h. riverBoundaryId - value of boundary id of river(solution equals zero on river boundary) "},
                {"dx", model.dx},
                {"Width", model.width},
                {"Height", model.height},
                {"RiverBoundaryId", model.river_boundary_id},

                {"FieldValue", model.field_value},
                {"Eta", model.eta},
                {"BifurcationType", model.bifurcation_type},
                {"BifurcationThreshold", model.bifurcation_threshold},
                {"BifurcationThreshold2", model.bifurcation_threshold_2},
                {"BifurcationMinDistance", model.bifurcation_min_dist},
                {"BifurcationAngle", model.bifurcation_angle},
                {"GrowthType", model.growth_type},
                {"GrowthThreshold", model.growth_threshold},
                {"GrowthMinDistance", model.growth_min_distance},
                {"ds", model.ds},

                {"ProgramOptions", {
                    {"SimulationType", model.prog_opt.simulation_type},
                    {"NumberOfSteps", model.prog_opt.number_of_steps},
                    {"NumberOfBackwardSteps", model.prog_opt.number_of_backward_steps},
                    {"MaximalRiverHeight", model.prog_opt.maximal_river_height},
                    {"Verbose", model.prog_opt.verbose},
                    {"SaveVTK", model.prog_opt.save_vtk},
                    {"OutputFileName", model.prog_opt.output_file_name},
                    {"InputFileName", model.prog_opt.input_file_name},
                    {"SaveEachStep", model.prog_opt.save_each_step}}},

                {"Integration",{
                    {"Radius", model.integr.integration_radius},
                    {"Exponant", model.integr.exponant},
                    {"WeightRadius", model.integr.weigth_func_radius}}},

                {"Mesh", {
                    {"BranchWidth", model.mesh.eps},
                    {"Exponant", model.mesh.exponant},
                    {"RefinmentRadius", model.mesh.refinment_radius},
                    {"MinArea", model.mesh.min_area},
                    {"MaxArea", model.mesh.max_area},
                    {"MinAngle", model.mesh.min_angle},
                    {"MaxEdge", model.mesh.max_edge},
                    {"MinEdge", model.mesh.min_edge},
                    {"TriangleRatio", model.mesh.ratio},
                    {"Sigma", model.mesh.sigma},
                    {"StaticRefinmentSteps", model.mesh.static_refinment_steps}}},
        
                {"Solver", {
                    {"Tollerance", model.solver_params.tollerance},
                    {"IterationSteps", model.solver_params.num_of_iterrations},
                    {"QuadratureDegree", model.solver_params.quadrature_degree},
                    {"RefinmentFraction", model.solver_params.refinment_fraction},
                    {"AdaptiveRefinmentSteps", model.solver_params.adaptive_refinment_steps}}}}
            },
            
            {"Boundaries", jboundaries},
            {"BoundariesConditions",""},
            {"Sources", model.sources},
            {"Trees", {
                {"Description", "SourcesIds represents sources(or root) branches of each rivers(yes you can setup several rivers in one run). Relations is array{...} of next elements {source_branch_id, {left_child_branch_id, right_child_branch_id} it holds structure of river divided into separate branches. Order of left and right id is important."},
                {"SourceIds", model.tree.source_branches_id},
                {"Relations", model.tree.branches_relation},
                {"Branches", branches}}},
                
            {"GeometryDifference", {
                {"Description", "This structure holds info about backward river simulation. AlongBranches consist of five arrays for each branch: {branch_id: {1..}, {2..}, {3..}, {4..}, {5..}}, Where first consist of angles values allong branch(from tip to source), second - distance between tips, third - a(1) elements, forth - a(2) elements, fifth - a(3) elements. In case of --simulation-type=2, first item - integral value over whole region, second - disk integral over tip with r = 0.1, and rest are series params. BiffuractionPoints - is similar to previous object. It has same parameters but in bifurcation point. {source_branch_id: {lenght of non zero branch, which doesnt reached bifurcation point as its adjacent branch},{a(1)},{a(2)},{a(3)}}."},
                {"AlongBranches", model.geometry_difference.branches_series_params_and_geom_diff},
                {"BifuractionPoints", model.geometry_difference.branches_bifuraction_info}}}
        };

        out << setw(4) << j;
        out.close();
    }

    void Open(Model& model)
    {
        ifstream in(model.prog_opt.input_file_name);
        if(!in) throw Exception("Open. Can't create file for read.");

        json j;
        in >> j;

        if(j.count("Model"))
        {
            json jmdl = j.at("Model");
            
            if (jmdl.count("Width")) jmdl.at("Width").get_to(model.width);
            if (jmdl.count("Height")) jmdl.at("Height").get_to(model.height);
            if (jmdl.count("dx")) jmdl.at("dx").get_to(model.dx);
            if (jmdl.count("RiverBoundaryId")) jmdl.at("RiverBoundaryId").get_to(model.river_boundary_id);
            if (jmdl.count("NumberOfBackwardSteps")) jmdl.at("NumberOfBackwardSteps").get_to(model.prog_opt.number_of_backward_steps);
            
            if (jmdl.count("FieldValue")) jmdl.at("FieldValue").get_to(model.field_value);
            if (jmdl.count("Eta")) jmdl.at("Eta").get_to(model.eta);
            if (jmdl.count("BifurcationType")) jmdl.at("BifurcationType").get_to(model.bifurcation_type);
            if (jmdl.count("BifurcationThreshold")) jmdl.at("BifurcationThreshold").get_to(model.bifurcation_threshold);
            if (jmdl.count("BifurcationThreshold2")) jmdl.at("BifurcationThreshold2").get_to(model.bifurcation_threshold_2);
            if (jmdl.count("BifurcationMinDistance")) jmdl.at("BifurcationMinDistance").get_to(model.bifurcation_min_dist);
            if (jmdl.count("BifurcationAngle")) jmdl.at("BifurcationAngle").get_to(model.bifurcation_angle);
            if (jmdl.count("BifurcationThreshold")) jmdl.at("BifurcationThreshold").get_to(model.bifurcation_threshold);
            if (jmdl.count("GrowthType")) jmdl.at("GrowthType").get_to(model.growth_type);
            if (jmdl.count("GrowthThreshold")) jmdl.at("GrowthThreshold").get_to(model.growth_threshold);
            if (jmdl.count("GrowthMinDistance")) jmdl.at("GrowthMinDistance").get_to(model.growth_min_distance);
            if (jmdl.count("ds")) jmdl.at("ds").get_to(model.ds);
            
            if(jmdl.count("ProgramOptions"))
            {
                auto jprogopt = jmdl.at("ProgramOptions");

                if (jprogopt.count("SimulationType")) jprogopt.at("SimulationType").get_to(model.prog_opt.simulation_type);
                if (jprogopt.count("NumberOfSteps")) jprogopt.at("NumberOfSteps").get_to(model.prog_opt.number_of_steps);
                if (jprogopt.count("NumberOfBackwardSteps")) jprogopt.at("NumberOfBackwardSteps").get_to(model.prog_opt.number_of_backward_steps);
                if (jprogopt.count("MaximalRiverHeight")) jprogopt.at("MaximalRiverHeight").get_to(model.prog_opt.maximal_river_height);
                if (jprogopt.count("Verbose")) jprogopt.at("Verbose").get_to(model.prog_opt.verbose);
                if (jprogopt.count("SaveVTK")) jprogopt.at("SaveVTK").get_to(model.prog_opt.save_vtk);
                
                if (jprogopt.count("InputFileName")) jprogopt.at("InputFileName").get_to(model.prog_opt.input_file_name);
                //if (jprogopt.count("OutputFileName")) jprogopt.at("OutputFileName").get_to(model.prog_opt.output_file_name);
                if (jprogopt.count("SaveEachStep")) jprogopt.at("SaveEachStep").get_to(model.prog_opt.save_each_step);
            }

            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl.at("Mesh");

                if (jmesh.count("BranchWidth"))jmesh.at("BranchWidth").get_to(model.mesh.eps);
                if (jmesh.count("Exponant")) jmesh.at("Exponant").get_to(model.mesh.exponant);
                if (jmesh.count("MaxArea"))  jmesh.at("MaxArea").get_to(model.mesh.max_area);
                if (jmesh.count("MinArea"))  jmesh.at("MinArea").get_to(model.mesh.min_area);
                if (jmesh.count("MinAngle")) jmesh.at("MinAngle").get_to(model.mesh.min_angle);
                if (jmesh.count("RefinmentRadius"))      jmesh.at("RefinmentRadius").get_to(model.mesh.refinment_radius);
                if (jmesh.count("StaticRefinmentSteps")) jmesh.at("StaticRefinmentSteps").get_to(model.mesh.static_refinment_steps);
                if (jmesh.count("Sigma"))    jmesh.at("Sigma").get_to(model.mesh.sigma);
                if (jmesh.count("MaxEdge"))  jmesh.at("MaxEdge").get_to(model.mesh.max_edge);
                if (jmesh.count("MinEdge"))  jmesh.at("MinEdge").get_to(model.mesh.min_edge);
                if (jmesh.count("TriangleRatio")) jmesh.at("TriangleRatio").get_to(model.mesh.ratio);   
            }

            if(jmdl.count("Integration"))
            {
                auto jinteg = jmdl.at("Integration");

                if (jinteg.count("Radius")) jinteg.at("Radius").get_to(model.integr.integration_radius);
                if (jinteg.count("Exponant")) jinteg.at("Exponant").get_to(model.integr.exponant);
                if (jinteg.count("WeightRadius")) jinteg.at("WeightRadius").get_to(model.integr.weigth_func_radius);
            }

            if(jmdl.count("Solver"))
            {
                auto jsolver = jmdl.at("Solver");
                
                if (jsolver.count("QuadratureDegree")) jsolver.at("QuadratureDegree").get_to(model.solver_params.quadrature_degree);
                if (jsolver.count("RefinmentFraction")) jsolver.at("RefinmentFraction").get_to(model.solver_params.refinment_fraction);
                if (jsolver.count("AdaptiveRefinmentSteps")) jsolver.at("AdaptiveRefinmentSteps").get_to(model.solver_params.adaptive_refinment_steps);
                if (jsolver.count("tollerance")) jsolver.at("Tollerance").get_to(model.solver_params.tollerance);
                if (jsolver.count("IterationSteps")) jsolver.at("IterationSteps").get_to(model.solver_params.num_of_iterrations);
            }
        }

        if(j.count("Boundaries"))
        {
            auto jboundaries = j.at("Boundaries");
            model.border.clear();
            for(auto& [key, value] : jboundaries.items()) 
            {
                SimpleBoundary simple_boundary;
                t_boundary_id boundary_id;
                
                //vertices
                vector<pair<double, double>> vertices;
                value.at("Vertices").get_to(vertices);
                simple_boundary.vertices.reserve(vertices.size());
                for(auto& vertice: vertices)
                    simple_boundary.vertices.push_back({vertice.first, vertice.second});
                
                //lines
                vector<tuple<t_vert_pos, t_vert_pos, unsigned>> lines;
                value.at("Lines").get_to(lines);
                simple_boundary.lines.reserve(lines.size());
                for(auto& line: lines)
                    simple_boundary.lines.push_back({get<0>(line), get<1>(line), get<2>(line)});

                //rest of fields
                value.at("InnerBoundary").get_to(simple_boundary.inner_boundary);
                vector<double> hole;
                value.at("Hole").get_to(hole);
                simple_boundary.hole = {hole.at(0), hole.at(1)};
                value.at("Name").get_to(simple_boundary.name);
                value.at("Id").get_to(boundary_id);

                model.border[boundary_id] = simple_boundary;
                
                //evaluation of boundary outfit rectangle
                if(!simple_boundary.inner_boundary)
                {
                    //Get size of bounding box of border and update width and height values of module
                    auto xmax = simple_boundary.vertices.at(0).x,
                        xmin = simple_boundary.vertices.at(0).x,
                        ymax = simple_boundary.vertices.at(0).y,
                        ymin = simple_boundary.vertices.at(0).y;
                    for(auto&v: simple_boundary.vertices)
                    {
                        if(v.x > xmax)
                            xmax = v.x;
                        if(v.x < xmin)
                            xmin = v.x;
                        if(v.y > ymax)
                            ymax = v.y;
                        if(v.y < ymin)
                            ymin = v.y;
                    }
                    
                    //if width and height equals to default values
                    if(model.width == 1. && model.height == 1.)
                    {
                        model.width = xmax - xmin;
                        model.height = ymax - ymin;
                    }
                }
            }
        }

        if(j.count("Sources")) j.at("Sources").get_to(model.sources);

        if(j.count("Trees"))
        {
            if(!j.count("Boundaries"))
                throw Exception("Input json file contains Trees and do not contain Boundary. Make sure that you created corresponding Boundary object(Trees and Boundary should contain same source/branches ids - its values and number)");
            
            model.tree.Clear();

            auto jtrees = j.at("Trees");
            jtrees.at("SourceIds").get_to(model.tree.source_branches_id);
            jtrees.at("Relations").get_to(model.tree.branches_relation);

            
            for(auto& [key, value] : jtrees.at("Branches").items()) 
            {   
                vector<double> s_point;
                vector<pair<double, double>> coords;
                double source_angle;
                int id;

                value.at("SourceVertice").get_to(s_point);
                value.at("SourceAngle").get_to(source_angle);
                value.at("Vertices").get_to(coords);
                value.at("Id").get_to(id);
                
                BranchNew branch(River::Point{s_point.at(0), s_point.at(1)}, source_angle);
                branch.resize(coords.size());

                for(unsigned i = 0; i < coords.size(); ++i)
                {
                    branch[i] = River::Point{coords.at(i).first, coords.at(i).second};
                }

                try
                {
                    model.tree.AddBranch(branch, id);
                }
                catch (Exception& e)
                {   
                    cout << e.what() << endl;
                    cout << "tree io..ivalid insertion" << endl;
                    throw;
                }
            }
        }
        else if(j.count("Boundaries"))
            //If no tree provided but border is, than we reinitialize tree.. to current border.
            model.tree.Initialize(model.border.GetSourcesIdsPointsAndAngles(model.sources));

        if(j.count("GeometryDifference"))
        { 
            json jgd = j.at("GeometryDifference");

            jgd.at("AlongBranches").get_to(model.geometry_difference.branches_series_params_and_geom_diff);
            jgd.at("BifuractionPoints").get_to(model.geometry_difference.branches_bifuraction_info);
        }
    }

    Model InitializeModelObject(cxxopts::ParseResult & vm)
    {
        Model model;
        if(vm.count("input"))
        {
            model.prog_opt.input_file_name = vm["input"].as<string>();
            Open(model);
        }

        //parameters set from program options has higher priority over input file
        SetupModelParamsFromProgramOptions(vm, model);//..if there are so.

        if(model.border.empty())
            model.InitializeBorderAndTree();
        
        model.CheckParametersConsistency();

        return model;
    }
}//namespace River