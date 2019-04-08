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

    Model mdl;
    mdl.eps = vm["eps"].as<double>();
    mdl.ds = vm["ds"].as<double>();
    mdl.dx = vm["dx"].as<double>();
    mdl.biffurcation_threshold = vm["biffurcation-threshold"].as<double>();
    mdl.width = vm["width"].as<double>();
    mdl.height = vm["height"].as<double>();

    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{mdl.width, mdl.height};
    auto sources_x_coord = vector<double>{mdl.dx};
    auto sources_id = vector<int>{1};

    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    border_mesh.write("lala.msh");
    Tree tr(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

    //MAIN LOOP
    for(int i = 0; 
        (vm["number-of-steps"].as<int>()==-1)? true: i < vm["number-of-steps"].as<int>(); 
        ++i)
        
        {
        auto mesh = BoundaryGenerator(mdl, tr, border, river_boundary_id);
        cout << "-------" << endl;
        cout << "  "<<i<< endl;
        cout << "-------" << endl;
        //FIXME:
        //preparing mesh constraint function
        AreaConstraint ac;
        ac.tip_points = tr.TipPoints();

        Triangle tria;
        tria.AreaConstrain = tria.ConstrainAngle = true;
        tria.MaxTriaArea = vm["mesh-max-area"].as<double>();
        tria.MinAngle = vm["mesh-min-angle"].as<double>();
        tria.Verbose = vm["verbose"].as<bool>();
        tria.Quite =  true;
        tria.CustomConstraint = true;
        tria.generate(mesh, &ac);
        mesh.convert();
        mesh.write(vm["output-mesh"].as<string>());

        //Simulation
        //Deal.II library
        if(vm["simulate"].as<bool>())
        {
            River::Solver sim;
            sim.numOfRefinments = vm["ref-num"].as<int>();
            sim.SetBoundaryRegionValue({river_boundary_id}, 0.);
            sim.SetBoundaryRegionValue({boundary_ids.at(1)}, 1.);
            sim.OpenMesh(vm["output-mesh"].as<string>());
            sim.run(i);

            for(auto id: tr.TipBranchesId())
            {
                auto tip_point = tr.GetBranch(id).TipPoint();
                auto tip_angle = tr.GetBranch(id).TipAngle();
                auto series_params = sim.integrate(tip_point, tip_angle);

                cout << "serieas_params: " 
                    << series_params.at(0) << " "
                    << series_params.at(1) << " "
                    << series_params.at(2) << endl;
                cout << "tip_angle: " << tip_angle << endl;
                cout << "tip_point: " << tip_point << endl;
                cout << "new point: " << mdl.next_point(series_params) << endl;

                if(mdl.q_biffurcate(series_params))
                {
                    auto br_left = BranchNew(tip_point, tip_angle + mdl.biff_angle);
                    br_left.AddPoint(Polar{mdl.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - mdl.biff_angle);
                    br_right.AddPoint(Polar{mdl.ds, 0});
                    tr.AddSubBranches(id, br_left, br_right);
                }
                else
                    tr.GetBranch(id).AddPoint(mdl.next_point(series_params));
            }
            //Biffurcation
        }
    }


    auto mesh = BoundaryGenerator(mdl, tr, border, river_boundary_id);

    Triangle tria;
    tria.AreaConstrain = tria.ConstrainAngle = true;
    tria.MaxTriaArea = vm["mesh-max-area"].as<double>();
    tria.MinAngle = vm["mesh-min-angle"].as<double>();
    tria.Verbose = vm["verbose"].as<bool>();
    tria.CustomConstraint = true;
    AreaConstraint ac;
    ac.tip_points = tr.TipPoints();
    tria.generate(mesh, &ac);
    mesh.convert();
    mesh.write(vm["output-mesh"].as<string>());

    tr.Save("tree_geometry.msh");
        
    return 0;
}