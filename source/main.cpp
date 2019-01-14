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
    River::Geometry geom;
    switch(vm["GeomType"].as<int>())
    {   
        case 0: 
            geom = SimpleGeo::Box();
            break;
        case 1:
            geom = SimpleGeo::CustomRiverTree();
            break;
        case 2:
            geom = SimpleGeo::SingleTip();
            break;
    }
    auto mesh = geom.GetInitialMesh();
    
    /*
                Mesh Generation
        */
    auto tr = Triangle();
    tr.Verbose = vm["Verbose"].as<bool>();
    tr.Quite = vm["Quiet"].as<bool>();
    tr.AreaConstrain = tr.ConstrainAngle = true;
    tr.MaxTriaArea = vm["MeshMaxArea"].as<double>();
    tr.MinAngle = vm["MeshMinAngle"].as<double>();
    //generate mesh
    tr.Generate(mesh);


    /*

         */
    mesh.convert();
    mesh.write("quadrangle.msh");
    mesh.info();

    River::Gmsh Gmsh;
    Gmsh.Open("quadrangle.msh");

    //Solve
    River::Solver sim;
    sim.numOfRefinments = vm["RefNum"].as<int>();
    sim.SetBoundaryRegionValue({Geometry::Markers::Bottom, Geometry::Markers::River}, 0.);
    sim.SetBoundaryRegionValue({Geometry::Markers::Top}, 1.);
    //sim.SetMesh(mesh);
    sim.OpenMesh("quadrangle.msh");
    sim.run();

    Gmsh.StartUserInterface();
    return 0;
}