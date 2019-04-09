/*! \file main.cpp
    \brief Entry point of program and main loop handler.
    
    Main incorporates user commands handling and managing of inner simulation objects.
    User commands are simple terminal commands(run ./riversim --help to check them all).
    It setups initial conditions and parameters. Initializes all object that are used in
    simulation like: Geometry, Mesh, Triangle and Simualtion, which are all combined
    into the loop.
    Please review code for further details
*/

#include "riversim.hpp"

using namespace River;
using namespace std;
///Entry point of RiverSim program.
/** It glues all together user inputs(commands) and inner objects to run simulation */
int main(int argc, char *argv[])
{
    /**
        Program options
    */
    auto vm = process_program_options(argc, argv);

    if (vm.count("help") || vm.count("version"))
        return 0;

    if (!vm.count("supprchess-signature"))
        print_ascii_signature();

    //Model object setup
    Model mdl;
    mdl.eps = vm["eps"].as<double>();
    mdl.ds = vm["ds"].as<double>();
    mdl.dx = vm["dx"].as<double>();
    mdl.biffurcation_threshold = vm["biffurcation-threshold"].as<double>();
    mdl.width = vm["width"].as<double>();
    mdl.height = vm["height"].as<double>();
    mdl.field_value = vm["field-value"].as<double>();

    //Timing Object setup
    Timing time;

    //Border object setup.. Rectangular boundaries
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{mdl.width, mdl.height};
    auto sources_x_coord = vector<double>{mdl.dx};
    auto sources_id = vector<int>{1};
    
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.river_boundary_id = river_boundary_id;
    
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    
    //Tree object setup
    Tree tree;
    tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

    if(border.GetSourcesId() != tree.SourceBranchesID())
        throw invalid_argument("Border ids and tree ids are not the same, or are not in same order!");


    //Triangle mesh object setup
    Triangle tria;
    tria.AreaConstrain = tria.ConstrainAngle = true;
    tria.MaxTriaArea = vm["mesh-max-area"].as<double>();
    tria.MinAngle = vm["mesh-min-angle"].as<double>();
    tria.Verbose = vm["verbose"].as<bool>();
    tria.Quite =  true;
    tria.CustomConstraint = true;

    //Simulation object setup
    River::Solver sim;
    sim.numOfRefinments = vm["ref-num"].as<int>();
    sim.field_value = mdl.field_value;

    //MAIN LOOP
    for(int i = 0; 
        (vm["number-of-steps"].as<int>() == -1)? true: i < vm["number-of-steps"].as<int>(); 
        ++i)    
    {
        cout << "-------" << endl;
        cout << "  "<<i<< endl;
        cout << "-------" << endl;

        ForwardRiverEvolution(mdl, tria, sim, tree, border, vm["output-mesh"].as<string>(), boundary_ids);

        time.Record();//Timing
        Save(mdl, time, border, tree, vm["output-sim"].as<string>());
    }

    return 0;
}