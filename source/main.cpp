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

    string output_file_name = vm["output"].as<string>();

    //Model object setup
    Model mdl;
    if(!vm.count("input"))
    {
        mdl = SetupModelParamsFromProgramOptions(vm);
    }

    //Timing Object setup
    Timing time;

    //Border object setup.. Rectangular boundaries
    Border border;
    if(!vm.count("input"))
        border.MakeRectangular(
            {mdl.width, mdl.height}, 
            mdl.boundary_ids,
            {mdl.dx},
            {1});
    
    //Tree object setup
    Tree tree;
    if(!vm.count("input"))
        tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

    if(border.GetSourcesId() != tree.SourceBranchesID())
        throw invalid_argument("Border ids and tree ids are not the same, or are not in same order!");

    if(vm.count("input"))
        Open(mdl, border, tree, vm["input"].as<string>());

    //Triangle mesh object setup
    Triangle tria;
    tria.AreaConstrain = true;
    tria.CustomConstraint = true;
    tria.MaxTriaArea = mdl.mesh.max_area;
    tria.MinAngle = mdl.mesh.min_angle;
    tria.Verbose = vm["verbose"].as<bool>();
    tria.Quite =  vm["quiet"].as<bool>();
    tria.ref = &mdl.mesh;

    //Simulation object setup
    River::Solver sim;
    sim.field_value = mdl.field_value;

    //MAIN LOOP
    for(int i = 0; 
        (vm["number-of-steps"].as<int>() == -1)? true: i < vm["number-of-steps"].as<int>(); 
        ++i)    
    {
        cout << "-------" << endl;
        cout << "  "<<i<< endl;
        cout << "-------" << endl;

        ForwardRiverEvolution(mdl, tria, sim, tree, border, output_file_name);

        time.Record();//Timing
        Save(mdl, time, border, tree, output_file_name);
    }

    return 0;
}