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
    

    auto [points, lines, triangles] = geom.GetInitialMesh();

    auto tr = Triangle(points, lines, triangles);
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
    Gmsh.Write();
    Gmsh.StartUserInterface();

    //Solve
    River::Simulation sim;
    sim.OpenMesh("quadrangle.msh");
    sim.run();

    return 0;
}