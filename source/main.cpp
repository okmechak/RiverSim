#include "riversim.hpp"


int main(int argc, char *argv[])
{
    /*
        Program options
    */

    auto vm = River::processProgramOptions(argc, argv);
    
    if (vm.count("help")) 
        return 0;
    
    if (!vm.count("suppress-signature"))
        River::printAsciiSignature();
    


    /*
        Geometry Object
    */
    auto dl = 0.01; //FIXME: 0.001 is to small
    auto riverGeom = River::Geometry();
    riverGeom.SetEps(2e-2);
    riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.4);
    for(int i = 0; i < 10; ++i)
        riverGeom.addPolar(River::Polar{0.01, M_PI/2., 1});
    riverGeom.initiateRootBranch();

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