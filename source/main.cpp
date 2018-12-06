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
    auto geom = CustomRiverTree(0.02, 0.00001);
    

    geom.generateCircularBoundary();

    River::Gmsh Gmsh;
    Gmsh.generate(geom.points);
    Gmsh.Write();

    //Solve
    River::Simulation sim;
    sim.OpenMesh();
    sim.run();
    Gmsh.StartUserInterface();

    return 0;
}