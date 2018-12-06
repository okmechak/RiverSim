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

    auto dl = 0.01; //FIXME: 0.001 is to small
    auto riverGeom = Geometry();
    riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);
    
    auto rootBranchID = 1;
    riverGeom.initiateRootBranch(rootBranchID);
    auto rootBranch = riverGeom.GetBranch(rootBranchID);
    
    for (int i = 0; i < 30; ++i){
        riverGeom.addPolar(Polar{dl, 0, rootBranchID}, true/*relative angle*/);
    }

    riverGeom.AddBiffurcation(rootBranchID, 0.03);
    riverGeom.addPolar(Polar{dl, 0, 2}, true);
    riverGeom.addPolar(Polar{dl, 0, 2}, true);
    riverGeom.addPolar(Polar{dl, 0, 4}, true);
    riverGeom.addPolar(Polar{dl, 0, 4}, true);
    

    riverGeom.generateCircularBoundary();

    River::Gmsh Gmsh;
    Gmsh.generate(riverGeom.points);
    Gmsh.Write();

    //Solve
    River::Simulation sim;
    sim.OpenMesh();
    sim.run();

    Gmsh.StartUserInterface();
    return 0;
}