#include "riversim.hpp"

using namespace River;

int main(int argc, char *argv[])
{
    /*
        Program options
    */

    auto vm = processProgramOptions(argc, argv);

    if (vm.count("help"))
        return 0;

    if (!vm.count("supprchess-signature"))
        printAsciiSignature();

    /*
        Geometry Object
    */
    auto geom = SimpleGeo::CustomRiverTree();
    
    auto initMesh = geom.GetInitialMesh();
    auto tr = Triangle(initMesh);

    tr.Verbose = vm["Verbose"].as<bool>();
    tr.Quite = vm["Quiet"].as<bool>();
    tr.AreaConstrain = tr.ConstrainAngle = true;
    tr.MaxTriaArea = vm["MeshMaxArea"].as<double>();
    tr.MinAngle = vm["MeshMinAngle"].as<double>();
    tr.Generate();
    tr.convert();
    tr.write("quadrangle.msh");
    
    River::Gmsh Gmsh;
    Gmsh.Open("quadrangle.msh");

    //Solve
    River::Solver sim;
    sim.numOfRefinments = vm["RefNum"].as<int>();
    sim.SetBoundaryRegionValue({Geometry::Markers::Bottom, Geometry::Markers::River}, 0.);
    sim.SetBoundaryRegionValue({Geometry::Markers::Top}, 1.);
    sim.OpenMesh("quadrangle.msh");
    sim.run();

    Gmsh.StartUserInterface();
    return 0;
}