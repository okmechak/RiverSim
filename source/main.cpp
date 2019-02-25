#include "riversim.hpp"

using namespace River;

int main(int argc, char *argv[])
{
    /*
            Program options
        */

    auto vm = process_program_options(argc, argv);

    if (vm.count("help") || vm.count("version"))
        return 0;

    if (!vm.count("supprchess-signature"))
        print_ascii_signature();

    /*
                    Geometry Object
         */
    River::Geometry geom;
    switch(vm["geom-type"].as<int>())
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
    tethex::Mesh meshio;
    geom.InitiateMesh(meshio);

    
    /*
                Mesh Generation
        */
    
    River::Triangle tr;

    //options
    tr.Verbose = vm["Verbose"].as<bool>();
    tr.Quite = vm["Quiet"].as<bool>();
    tr.AreaConstrain = tr.ConstrainAngle = true;
    tr.MaxTriaArea = vm["MeshMaxArea"].as<double>();
    tr.MinAngle = vm["MeshMinAngle"].as<double>();
    //generate mesh
    tr.generate(meshio);
    //convert triangles to quadrangles
    meshio.convert();
    meshio.write(vm["output-mesh"].as<string>());

    if(vm["Verbose"].as<bool>())
        meshio.info();


    /*
            Solver
         */
    if(vm["simulate"].as<bool>())
    {
        River::Solver sim;
        sim.numOfRefinments = vm["ref-num"].as<int>();
        sim.SetBoundaryRegionValue({Geometry::Markers::Bottom, Geometry::Markers::River}, 0.);
        sim.SetBoundaryRegionValue({Geometry::Markers::Top}, 1.);
        //sim.SetMesh(meshio);
        sim.OpenMesh(vm["output-mesh"].as<string>());
        sim.run();
    }

    return 0;
}