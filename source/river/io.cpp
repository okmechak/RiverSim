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

        const Model mdl;
        
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
        ("o,output", "Name of simulation data and state of program data.", 
            value<string>()->default_value(mdl.prog_opt.output_file_name) )
        ("save-each-step", "Save each step of simulation in separate file: simdata_1.json, simdata_2.json.. ", 
            value<bool>()->default_value(bool_to_string(mdl.prog_opt.save_each_step)) )
        ("vtk", "Outputs VTK file of Deal.II solution", 
            value<bool>()->default_value(bool_to_string(mdl.prog_opt.save_vtk)) )
        ("input",
            "input simaultion data, boundary, tree, model parameters. It has very similar structure as output json of program.", 
            value<string>() );
        
        //prints and logs
        options.add_options("Logs")
        ("V,verbose", "print detailed log to terminal.", 
            value<bool>()->default_value(bool_to_string(mdl.prog_opt.verbose)) );

        //Simulation parameters
        options.add_options("Simulation parameters")
        ("n,number-of-steps", "Number of steps to simulate.", 
            value<unsigned>()->default_value(to_string(mdl.prog_opt.number_of_steps)))
        ("m,maximal-river-height", "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.", 
            value<double>()->default_value(to_string(mdl.prog_opt.maximal_river_height)))
        ("t,simulation-type", "Type of simulation: 0 - forward river growth, 1 - backward river growth, 2 - Used for development purposes.", 
            value<unsigned>()->default_value(to_string(mdl.prog_opt.simulation_type)))
        ("number-of-backward-steps", "Number of backward steps simulations used in backward simulation type.", 
            value<unsigned>()->default_value(to_string(mdl.prog_opt.number_of_backward_steps)));
        
        //Geometry parameters
        options.add_options("Border geometry parameters")
        ("dx", "dx - shift of initial river position from beginning of coordinates in X dirrections.", 
            value<double>()->default_value(to_string(mdl.dx)) )
        ("width", "Width of river rectangular region.", 
            value<double>()->default_value(to_string(mdl.width)) )
        ("height", "Height of river rectangular region.", 
            value<double>()->default_value(to_string(mdl.height)) );
        
        //Model parameters
        options.add_options("Model parameters")
        ("c,boundary-condition", "0 - Poisson(indexes 0,1 and 3 corresponds to free boundary condition, 4 - to zero value on boundary), 1 - Laplace(Indexes 1 and 3 - free condition, 2 - value one, and 4 - value zero.)", 
            value<unsigned>()->default_value(to_string(mdl.boundary_condition)) )
        ("f,field-value", "Value of outter force used for Poisson equation(Right-hand side value)", 
            value<double>()->default_value(to_string(mdl.field_value)) )
        ("eta", "Power of a1^eta used in growth of river.", 
            value<double>()->default_value(to_string(mdl.eta)) )
        ("bifurcation-type", "Bifurcation method type. 0 - a(3)/a(1) > bifurcation_threshold, 1 - a1 > bifurcation_threshold, 2 - combines both conditions, 3 - no bifurcation at all.", 
            value<unsigned>()->default_value(to_string(mdl.bifurcation_type)) )
        ("b,bifurcation-threshold", "Bifuraction threshold for first bifurcation type: a(3)/a(1) < kcrit", 
            value<double>()->default_value(to_string(mdl.bifurcation_threshold)) )
        ("bifurcation-threshold-2", "Biffuraction threshold for second bifurcation type: a(1) > kcrit", 
            value<double>()->default_value(to_string(mdl.bifurcation_threshold_2)) )
        ("bifurcation-min-distance", "Minimal distance between adjacent bifurcation points. In other words, if lenght of branch is greater of specified value, only than it can biffurcate. Used for reducing numerical noise.", 
            value<double>()->default_value(to_string(mdl.bifurcation_min_dist)) )
        ("bifurcation-angle", "Angle between branches in bifurcation point. Default is Pi/5 which is theoretical value.", 
            value<double>()->default_value(to_string(mdl.bifurcation_angle)) )
        ("growth-type", "Specifies growth type used for evaluation of next point(its dirrection and lenght): 0 - using arctan(a2/a1) method, 1 - by preavuating {dx, dy}. For more details please Piotr Morawiecki", 
            value<unsigned>()->default_value(to_string(mdl.growth_type)) )
        ("growth-threshold", "Growth of branch stops if a(1) < growth-threshold.", 
            value<double>()->default_value(to_string(mdl.growth_threshold)) )
        ("growth-min-distance", "Growth of branch will be with constant speed(ds by each step) if its lenght is less then this value. uUsed for reducing numerical noise", 
            value<double>()->default_value(to_string(mdl.growth_min_distance)) )
        ("ds", "ds - value of growth proportionality: dl = ds*a(1)^eta", 
            value<double>()->default_value(to_string(mdl.ds)) );
        
        //Integration parameters
        options.add_options("Series parameters integral")
        ("weight-exp", "Parameter used in integration weight function. For more details please see FreeFem implementation.", 
            value<double>()->default_value(to_string(mdl.integr.exponant)) )
        ("integration-radius", "Radius of integration around tips for evaluation of series parameters", 
            value<double>()->default_value(to_string(mdl.integr.integration_radius)) )
        ("weight-radius", "Parameter used in integration weight function. Weight radius parameter. For more details please see FreeFem implementation.", 
            value<double>()->default_value(to_string(mdl.integr.weigth_func_radius)) );

        //Mesh parameters
        options.add_options("Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).")
        ("mesh-exp", "Parameter used in mesh refinment function. Reduce(small values) or increase(large values) slope on edge.", 
            value<double>()->default_value(to_string(mdl.mesh.exponant)) )
        ("mesh-sigma", "Parameter used in mesh refinment function. Shifts maximal slope region relatively to r0 value.", 
            value<double>()->default_value(to_string(mdl.mesh.sigma)) )
        ("A,mesh-max-area", "Constraints maximal area of triangle element", 
            value<double>()->default_value(to_string(mdl.mesh.max_area)) )
        ("mesh-min-area", "Constraints minimal area of triangle element.", 
            value<double>()->default_value(to_string(mdl.mesh.min_area)) )

        ("mesh-max-edge", "Constraints maximal edge lenght of triangle element.", 
            value<double>()->default_value(to_string(mdl.mesh.max_edge)) )
        ("mesh-min-edge", "Constraints minimal edge lenght of triangle element. It can has higher priority then adaptive function and other mesh constraints.", 
            value<double>()->default_value(to_string(mdl.mesh.min_edge)) )
        ("mesh-ratio", "Constraints ratio of triangle element. Relation of longest edge to shortes edge of triangle. Has similar effect and limitation as mesh-min-angle option. Value 2 correspond to minimal nagle 30. So be carefull with setting values less than 2.", 
            value<double>()->default_value(to_string(mdl.mesh.ratio)) )

        ("refinment-radius", "r0 - refinment radius from this formula. Corresponds to position of maximal slope.", 
            value<double>()->default_value(to_string(mdl.mesh.refinment_radius)) )
        ("q,mesh-min-angle", "Constraints minimal angle of triangle element.", 
            value<double>()->default_value(to_string(mdl.mesh.min_angle)) )
        ("e,eps", "Width of branch. Every branch is divided and joined into one big edge which covers all geometry. Width of divided branch corresponds to eps. Such solution is constrain of Deal.II library.", 
            value<double>()->default_value(to_string(mdl.mesh.eps)) )
        ("static-refinment-steps", "Number of mesh refinment steps used by Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(mdl.mesh.static_refinment_steps)) );

        options.add_options("Solver Parameters")
        ("quadrature-degree", "Quadrature polynomials degree used in numerical integration of Deal.II solver.", 
            value<unsigned>()->default_value(to_string(mdl.solver_params.quadrature_degree)))
        ("iteration-steps", "Number of iterations used by Deal.II solver.", 
            value<unsigned>()->default_value(to_string(mdl.solver_params.num_of_iterrations)))
        ("tol", "Tollerance used by Deal.II solver.", 
            value<double>()->default_value(to_string(mdl.solver_params.tollerance)))
        ("refinment-fraction", "Fraction(percent from total, 0.01 corresponds to 1%) of refined mesh elements using Deal.II adaptive mesh capabilities.", 
            value<double>()->default_value(to_string(mdl.solver_params.refinment_fraction)))
        ("adaptive-refinment-steps", "Number of refinment steps used by adaptive Deal.II mesh functionality.", 
            value<unsigned>()->default_value(to_string(mdl.solver_params.adaptive_refinment_steps)));

        
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
                "Border geometry parameters",
                "Model parameters",
                "Series parameters integral",
                "Mesh refinment parameters. Funciton of area constaint and its parameters: min_area - (max_area - min_area)*(1 - exp( - 1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp})/(1 + exp( -1/(2*{mesh-sigma}^2)*(r/ro)^{mesh-exp}).",
                "Solver Parameters"}) 
                << endl;
        }

        if (result.count("version"))
            print_version();
        
        return result;
    }

    void SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm, Model& mdl)
    {
        //program options

        if (vm.count("simulation-type"))
            mdl.prog_opt.simulation_type = vm["simulation-type"].as<unsigned>();
        if (vm.count("number-of-steps")) 
            mdl.prog_opt.number_of_steps = vm["number-of-steps"].as<unsigned>();
        if (vm.count("maximal-river-height"))
            mdl.prog_opt.maximal_river_height = vm["maximal-river-height"].as<double>();
        if (vm.count("number-of-backward-steps"))
            mdl.prog_opt.number_of_backward_steps = vm["number-of-backward-steps"].as<unsigned>();
        if (vm.count("vtk")) mdl.prog_opt.save_vtk = true;
        if (vm.count("verbose")) mdl.prog_opt.verbose = vm["verbose"].as<bool>();
        if(vm.count("output")) mdl.prog_opt.output_file_name = vm["output"].as<string>();
        if(vm.count("input")) mdl.prog_opt.input_file_name = vm["input"].as<string>();
        if(vm.count("save-each-step")) mdl.prog_opt.save_each_step = true;

        //mesh options
        if (vm.count("refinment-radius")) mdl.mesh.refinment_radius = vm["refinment-radius"].as<double>();
        if (vm.count("mesh-exp")) mdl.mesh.exponant = vm["mesh-exp"].as<double>();
        if (vm.count("mesh-sigma")) mdl.mesh.sigma = vm["mesh-sigma"].as<double>();
        if (vm.count("static-refinment-steps")) mdl.mesh.static_refinment_steps = vm["static-refinment-steps"].as<unsigned>();
        if (vm.count("mesh-max-area")) mdl.mesh.max_area = vm["mesh-max-area"].as<double>();
        if (vm.count("mesh-min-area")) mdl.mesh.min_area = vm["mesh-min-area"].as<double>();
        if (vm.count("mesh-min-angle")) mdl.mesh.min_angle = vm["mesh-min-angle"].as<double>();
        if (vm.count("mesh-max-edge")) mdl.mesh.max_edge = vm["mesh-max-edge"].as<double>();
        if (vm.count("mesh-min-edge")) mdl.mesh.min_edge = vm["mesh-min-edge"].as<double>();
        if (vm.count("mesh-ratio")) mdl.mesh.ratio = vm["mesh-ratio"].as<double>();
        if (vm.count("eps")) mdl.mesh.eps = vm["eps"].as<double>();
        
        //integration options
        if (vm.count("integration-radius")) mdl.integr.integration_radius = vm["integration-radius"].as<double>();
        if (vm.count("weight-radius")) mdl.integr.weigth_func_radius = vm["weight-radius"].as<double>();
        if (vm.count("weight-exp")) mdl.integr.exponant = vm["weight-exp"].as<double>();

        //solver options
        if (vm.count("tol")) mdl.solver_params.tollerance = vm["tol"].as<double>();
        if (vm.count("iteration-steps")) mdl.solver_params.num_of_iterrations = vm["iteration-steps"].as<unsigned>();
        if (vm.count("adaptive-refinment-steps")) mdl.solver_params.adaptive_refinment_steps = vm["adaptive-refinment-steps"].as<unsigned>();
        if (vm.count("refinment-fraction")) mdl.solver_params.refinment_fraction = vm["refinment-fraction"].as<double>();
        if (vm.count("quadrature-degree")) mdl.solver_params.quadrature_degree = vm["quadrature-degree"].as<unsigned>();

        //model parameters
        //geometry
        if (vm.count("dx")) mdl.dx = vm["dx"].as<double>();
        if (vm.count("width")) mdl.width = vm["width"].as<double>();
        if (vm.count("height")) mdl.height = vm["height"].as<double>();

        if (vm.count("boundary-condition")) mdl.boundary_condition = vm["boundary-condition"].as<unsigned>();
        if (vm.count("field-value")) mdl.field_value = vm["field-value"].as<double>();
        if (vm.count("eta")) mdl.eta = vm["eta"].as<double>();
        if (vm.count("bifurcation-type")) mdl.bifurcation_type = vm["bifurcation-type"].as<unsigned>();
        if (vm.count("bifurcation-threshold")) mdl.bifurcation_threshold = vm["bifurcation-threshold"].as<double>();
        if (vm.count("bifurcation-threshold-2")) mdl.bifurcation_threshold_2 = vm["bifurcation-threshold-2"].as<double>();
        if (vm.count("bifurcation-angle")) mdl.bifurcation_angle = vm["bifurcation-angle"].as<double>();
        if (vm.count("bifurcation-min-distance")) mdl.bifurcation_min_dist = vm["bifurcation-min-distance"].as<double>();
        if (vm.count("growth-type")) mdl.growth_type = vm["growth-type"].as<unsigned>();
        if (vm.count("growth-threshold")) mdl.growth_threshold = vm["growth-threshold"].as<double>();
        if (vm.count("growth-min-distance")) mdl.growth_min_distance = vm["growth-min-distance"].as<double>();
        if (vm.count("ds")) mdl.ds = vm["ds"].as<double>();
    }

    void Save(const Model& model, const string file_name)
    {
        if(file_name.length() == 0)
            throw invalid_argument("Save: File name is not set.");

        ofstream out(file_name + ".json");
        if(!out) throw invalid_argument("Save: Can't create file for write");

        out.precision(16);

        //Branches
        json branches;
        for(auto id: model.tree.branches_index)
        {
            auto branch_id = id.first;
            auto branch = model.tree.GetBranch(branch_id);
            vector<pair<double, double>> coords(branch->Size());
            for(unsigned i = 0; i < branch->Size(); ++i)
                coords[i] = {branch->GetPoint(i).x, branch->GetPoint(i).y};


            branches.push_back({
                {"sourcePoint", {branch->SourcePoint().x , branch->SourcePoint().y}},
                {"sourceAngle", branch->SourceAngle()},
                {"Desciption", "Order of elements should be from source point to tip. Source point should be the same as first point of array. Source angle - represents branch growth dirrection when it consist only from one(source) point. For example perpendiculary to border line. Id should be unique(and >= 1) to each branch and is referenced in Trees->Relations structure and Border->SourcesId"},
                {"coords", coords},
                {"id", branch_id}});
        }
        
        //Border
        json jborder;
        {
            vector<pair<double, double>> coords;
            vector<vector<int>> lines;
            coords.reserve(model.border.GetVertices().size());
            coords.reserve(model.border.GetLines().size());

            for(auto& p: model.border.GetVertices())
                coords.push_back({p.x, p.y});

            for(auto& l: model.border.GetLines())
                lines.push_back({(int)l.p1, (int)l.p2, l.id});

            jborder = {
                {"SourceIds", model.border.GetSourceMap()}, 
                {"SomeDetails", "Points and lines should be in counterclockwise order. SourcesIDs is array of pairs - where first number - is related branch id(source branche), and second is index of related point in coords array(after initialization it will be source point of related branch). Lines consist of three numbers: first and second - point index in coords array, third - configures boundary condition(See --boundary-condition option in program: ./riversim -h)."},
                {"coords", coords},
                {"lines", lines}};
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
                {"width", model.width},
                {"height", model.height},
                {"riverBoundaryId", model.river_boundary_id},
                {"boundaryIds", model.boundary_ids}, 
                {"numberOfBackwardSteps", model.prog_opt.number_of_backward_steps},

                {"boundaryCondition", model.boundary_condition},
                {"fieldValue", model.field_value},
                {"eta", model.eta},
                {"bifurcationType", model.bifurcation_type},
                {"bifurcationThreshold", model.bifurcation_threshold},
                {"bifurcationThreshold2", model.bifurcation_threshold_2},
                {"bifurcationMinDistance", model.bifurcation_min_dist},
                {"bifurcationAngle", model.bifurcation_angle},
                {"growthType", model.growth_type},
                {"growthThreshold", model.growth_threshold},
                {"growthMinDistance", model.growth_min_distance},
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
                    {"radius", model.integr.integration_radius},
                    {"exponant", model.integr.exponant},
                    {"weightRadius", model.integr.weigth_func_radius}}},

                {"Mesh", {
                    {"eps", model.mesh.eps},
                    {"exponant", model.mesh.exponant},
                    {"refinmentRadius", model.mesh.refinment_radius},
                    {"minArea", model.mesh.min_area},
                    {"maxArea", model.mesh.max_area},
                    {"minAngle", model.mesh.min_angle},
                    {"maxEdge", model.mesh.max_edge},
                    {"minEdge", model.mesh.min_edge},
                    {"ratio", model.mesh.ratio},
                    {"sigma", model.mesh.sigma},
                    {"staticRefinmentSteps", model.mesh.static_refinment_steps},
                    {"numberOfQuadrangles", model.mesh.number_of_quadrangles},
                    {"numberOfRefinedQuadrangles", model.mesh.number_of_refined_quadrangles}}},
        
                {"Solver", {
                    {"tol", model.solver_params.tollerance},
                    {"iterationSteps", model.solver_params.num_of_iterrations},
                    {"quadratureDegree", model.solver_params.quadrature_degree},
                    {"refinmentFraction", model.solver_params.refinment_fraction},
                    {"adaptiveRefinmentSteps", model.solver_params.adaptive_refinment_steps}}}}
            },
            
            {"Border", jborder},

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


    void Open(Model& model, string file_name, bool& q_update_border)
    {
        ifstream in(file_name);
        if(!in) throw invalid_argument("Open. Can't create file for read.");

        json j;
        in >> j;

        if(j.count("Model"))
        {
            json jmdl = j["Model"];
            
            if (jmdl.count("width")) jmdl.at("width").get_to(model.width);
            if (jmdl.count("height")) jmdl.at("height").get_to(model.height);
            if (jmdl.count("dx")) jmdl.at("dx").get_to(model.dx);
            if (jmdl.count("riverBoundaryId")) jmdl.at("riverBoundaryId").get_to(model.river_boundary_id);
            if (jmdl.count("boundaryIds")) jmdl.at("boundaryIds").get_to(model.boundary_ids);
            if (jmdl.count("numberOfBackwardSteps")) jmdl.at("numberOfBackwardSteps").get_to(model.prog_opt.number_of_backward_steps);
            
            if (jmdl.count("boundaryCondition")) jmdl.at("boundaryCondition").get_to(model.boundary_condition);
            if (jmdl.count("fieldValue")) jmdl.at("fieldValue").get_to(model.field_value);
            if (jmdl.count("eta")) jmdl.at("eta").get_to(model.eta);
            if (jmdl.count("bifurcationType")) jmdl.at("bifurcationType").get_to(model.bifurcation_type);
            if (jmdl.count("bifurcationThreshold")) jmdl.at("bifurcationThreshold").get_to(model.bifurcation_threshold);
            if (jmdl.count("bifurcationThreshold2")) jmdl.at("bifurcationThreshold2").get_to(model.bifurcation_threshold_2);
            if (jmdl.count("bifurcationMinDistance")) jmdl.at("bifurcationMinDistance").get_to(model.bifurcation_min_dist);
            if (jmdl.count("bifurcationAngle")) jmdl.at("bifurcationAngle").get_to(model.bifurcation_angle);
            if (jmdl.count("bifurcationThreshold")) jmdl.at("bifurcationThreshold").get_to(model.bifurcation_threshold);
            if (jmdl.count("growthType")) jmdl.at("growthType").get_to(model.growth_type);
            if (jmdl.count("growthThreshold")) jmdl.at("growthThreshold").get_to(model.growth_threshold);
            if (jmdl.count("growthMinDistance")) jmdl.at("growthMinDistance").get_to(model.growth_min_distance);
            if (jmdl.count("ds")) jmdl.at("ds").get_to(model.ds);
            
            if(jmdl.count("ProgramOptions"))
            {
                auto jprogopt = jmdl["ProgramOptions"];

                if (jprogopt.count("SimulationType")) jprogopt.at("SimulationType").get_to(model.prog_opt.simulation_type);
                if (jprogopt.count("NumberOfSteps")) jprogopt.at("NumberOfSteps").get_to(model.prog_opt.number_of_steps);
                if (jprogopt.count("NumberOfBackwardSteps")) jprogopt.at("NumberOfBackwardSteps").get_to(model.prog_opt.number_of_backward_steps);
                if (jprogopt.count("MaximalRiverHeight")) jprogopt.at("MaximalRiverHeight").get_to(model.prog_opt.maximal_river_height);
                if (jprogopt.count("Verbose")) jprogopt.at("Verbose").get_to(model.prog_opt.verbose);
                if (jprogopt.count("SaveVTK")) jprogopt.at("SaveVTK").get_to(model.prog_opt.save_vtk);
                
                if (jprogopt.count("InputFileName")) jprogopt.at("InputFileName").get_to(model.prog_opt.input_file_name);
                if (jprogopt.count("OutputFileName")) jprogopt.at("OutputFileName").get_to(model.prog_opt.output_file_name);
                if (jprogopt.count("SaveEachStep")) jprogopt.at("SaveEachStep").get_to(model.prog_opt.save_each_step);
            }

            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl["Mesh"];

                if (jmesh.count("eps"))      jmesh.at("eps").get_to(model.mesh.eps);
                if (jmesh.count("exponant")) jmesh.at("exponant").get_to(model.mesh.exponant);
                if (jmesh.count("maxArea"))  jmesh.at("maxArea").get_to(model.mesh.max_area);
                if (jmesh.count("minArea"))  jmesh.at("minArea").get_to(model.mesh.min_area);
                if (jmesh.count("minAngle")) jmesh.at("minAngle").get_to(model.mesh.min_angle);
                if (jmesh.count("refinmentRadius"))      jmesh.at("refinmentRadius").get_to(model.mesh.refinment_radius);
                if (jmesh.count("staticRefinmentSteps")) jmesh.at("staticRefinmentSteps").get_to(model.mesh.static_refinment_steps);
                if (jmesh.count("sigma"))    jmesh.at("sigma").get_to(model.mesh.sigma);
                if (jmesh.count("maxEdge"))  jmesh.at("maxEdge").get_to(model.mesh.max_edge);
                if (jmesh.count("minEdge"))  jmesh.at("minEdge").get_to(model.mesh.min_edge);
                if (jmesh.count("ratio"))    jmesh.at("ratio").get_to(model.mesh.ratio);   
            }

            if(jmdl.count("Integration"))
            {
                auto jinteg = jmdl["Integration"];

                if (jinteg.count("radius")) jinteg.at("radius").get_to(model.integr.integration_radius);
                if (jinteg.count("exponant")) jinteg.at("exponant").get_to(model.integr.exponant);
                if (jinteg.count("weightRadius")) jinteg.at("weightRadius").get_to(model.integr.weigth_func_radius);
            }

            if(jmdl.count("Solver"))
            {
                auto jsolver = jmdl["Solver"];
                
                if (jsolver.count("quadratureDegree")) jsolver.at("quadratureDegree").get_to(model.solver_params.quadrature_degree);
                if (jsolver.count("refinmentFraction")) jsolver.at("refinmentFraction").get_to(model.solver_params.refinment_fraction);
                if (jsolver.count("adaptiveRefinmentSteps")) jsolver.at("adaptiveRefinmentSteps").get_to(model.solver_params.adaptive_refinment_steps);
                if (jsolver.count("tol")) jsolver.at("tol").get_to(model.solver_params.tollerance);
                if (jsolver.count("iterationSteps")) jsolver.at("iterationSteps").get_to(model.solver_params.num_of_iterrations);
            }
        }

        if(j.count("Border"))
        {
            q_update_border = true;
            auto jborder = j["Border"];
            vector<pair<double, double>> coords;
            vector<Point> points;
            vector<vector<int>> lines_raw;
            vector<Line> lines;
            map<int, long unsigned> sources;

            jborder.at("SourceIds").get_to(sources);

            jborder.at("coords").get_to(coords);
            points.reserve(coords.size());
            for(auto& c : coords)
                points.push_back(Point{c.first, c.second});

            jborder.at("lines").get_to(lines_raw);
            lines.reserve(lines_raw.size());
            for(auto& l : lines_raw)
                lines.push_back(Line{(long unsigned)l.at(0), (long unsigned)l.at(1), l.at(2)});

            model.border = Border{points, lines, sources};

            //Get size of bounding box of border and update width and height values of module
            auto xmax = points.at(0).x,
                xmin = points.at(0).x,
                ymax = points.at(0).y,
                ymin = points.at(0).y;
            for(auto&p: points)
            {
                if(p.x > xmax)
                    xmax = p.x;
                if(p.x < xmin)
                    xmin = p.x;
                if(p.y > ymax)
                    ymax = p.y;
                if(p.y < ymin)
                    ymin = p.y;
            }
            model.width = xmax - xmin;
            model.height = ymax - ymin;
        }

        if(j.count("Trees"))
        {
            if(!j.count("Border"))
                throw invalid_argument("Input json file contains Trees and do not contain Border. Make sure that you created corresponding Border object(Trees and Border should contain same source/branches ids - its values and number)");
            
            model.tree.Clear();

            auto jtrees = j["Trees"];
            jtrees.at("SourceIds").get_to(model.tree.source_branches_id);
            jtrees.at("Relations").get_to(model.tree.branches_relation);

            
            for(auto& [key, value] : jtrees["Branches"].items()) 
            {   
                vector<double> s_point;
                vector<pair<double, double>> coords;
                double source_angle;
                int id;

                value.at("sourcePoint").get_to(s_point);
                value.at("sourceAngle").get_to(source_angle);
                value.at("coords").get_to(coords);
                value.at("id").get_to(id);
                
                BranchNew branch(River::Point{s_point.at(0), s_point.at(1)}, source_angle);
                branch.points.resize(coords.size());

                
                for(unsigned i = 0; i < coords.size(); ++i)
                {
                    branch.points[i] = River::Point{coords.at(i).first, coords.at(i).second};
                }
                try
                {
                    model.tree.AddBranch(branch, id);
                }
                catch (invalid_argument& e)
                {   
                    cout << e.what() << endl;
                    cout << "tree io..ivalid inser" << endl;
                }
            }
        }
        else if(j.count("Border"))
            //If no tree provided but border is, than we reinitialize tree.. to current border.
            model.tree.Initialize(model.border.GetSourcesPoint(), model.border.GetSourcesNormalAngle(), model.border.GetSourcesId());

        if(j.count("GeometryDifference"))
        { 
            json jgd = j["GeometryDifference"];

            jgd.at("AlongBranches").get_to(model.geometry_difference.branches_series_params_and_geom_diff);
            jgd.at("BifuractionPoints").get_to(model.geometry_difference.branches_bifuraction_info);
        }
    }
}//namespace River