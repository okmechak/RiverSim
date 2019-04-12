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
    if(!vm.count("input"))
    {
        //geometry
        mdl.width = vm["width"].as<double>();
        mdl.height = vm["height"].as<double>();
        mdl.dx = vm["dx"].as<double>();
        //model parameters
        mdl.boundary_condition = vm["boundary-condition"].as<int>();
        mdl.field_value = vm["field-value"].as<double>();
        mdl.eta = vm["eta"].as<double>();
        mdl.biffurcation_type = vm["biffurcation-type"].as<int>();
        mdl.biffurcation_threshold = vm["biffurcation-threshold"].as<double>();
        mdl.biffurcation_angle = vm["biffurcation-angle"].as<double>();
        mdl.growth_type = vm["growth-type"].as<int>();
        mdl.growth_threshold = vm["growth-threshold"].as<double>();
        mdl.ds = vm["ds"].as<double>();
        //mesh options
        mdl.mesh.eps = vm["eps"].as<double>();
        mdl.mesh.exponant = vm["mesh-exp"].as<double>();
        mdl.mesh.max_area = vm["mesh-max-area"].as<double>();
        mdl.mesh.min_area = vm["mesh-min-area"].as<double>();
        mdl.mesh.min_angle = vm["mesh-min-angle"].as<double>();
        mdl.mesh.refinment_radius = vm["refinment-radius"].as<double>();
        //integration options
        mdl.integr.integration_radius = vm["integration-radius"].as<double>();
        mdl.integr.exponant = vm["weight-exp"].as<double>();
    }

    //Timing Object setup
    Timing time;

    //Border object setup.. Rectangular boundaries
    Border border;
    if(!vm.count("input"))
    {
        border.MakeRectangular(
            {mdl.width, mdl.height}, 
            mdl.boundary_ids,
            {mdl.dx},
            {1});
    }
    
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

        ForwardRiverEvolution(mdl, tria, sim, tree, border, vm["output-mesh"].as<string>());

        time.Record();//Timing
        Save(mdl, time, border, tree, vm["output-sim"].as<string>());
    }

    return 0;
}