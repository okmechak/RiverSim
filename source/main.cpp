#include <iostream>
#include "riversim.hpp"

using namespace std;

void print_ascii_signature()
{
    cout << endl;
    cout << "     _)                    _)           " << endl;
    cout << "  __| |\\ \\   / _ \\  __| __| | __ `__ \\  " << endl;
    cout << " |    | \\ \\ /  __/ |  \\__ \\ | |   |   | " << endl;
    cout << "_|   _|  \\_/ \\___|_|  ____/_|_|  _|  _| " << endl;         
    cout << endl;            
}

int main(int argc, char *argv[])
{
    /*
        Boost program options
    */

    //Declaring group of options that will be allowed on both
    //command line and config file
    po::options_description config("Configuration");

    //declare supported options
    config.add_options()
        ("help,h", "produce help message")
        ("version,v", "print version string")
        ("test,t", po::value<string>(), "test option")
        ("sw1,s", po::value<int>()->default_value(0), "custom switch between different flows")
        ("sw2", po::value<int>()->default_value(1), "custom switch between different flows")
        ("sw2", po::value<int>()->default_value(3), "custom switch between different flows")
        ("b1,b", "custom switch between different flows" )
        ("b2", "custom switch between different flows" )
        ("b3", "custom switch between different flows" )
        ("input-mesh,f",po::value<vector<string>>(), "input mesh file")
        ("suppress-signature", "suppress signature printing")
        ("draw-mesh,d", po::value<bool>()->default_value(true), "draw mesh using gmsh fltk submodule")
        ("output,o", po::value<string>()->default_value("out_mesh.mesh"), "save output mesh")
        ("gmsh-log", po::value<bool>()->default_value(false), "print Gmsh log to terminal")
        ("sm", "suppress mesh")
        ("ss", "suppress solver")
    ;
    
    po::positional_options_description p;
    p.add("input-mesh", -1);
   
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, config), vm);
    po::notify(vm);    

    if (!vm.count("suppress-signature"))
        print_ascii_signature();
    
    if (vm.count("help")) {
        cout << config << endl;
        return 0;
    }

    /*
        Mesh class test
    */
    

    //Geomerty
    struct River::vecTriangulateIO geom;
    geom.points = vector<double>
        {
            0.0, 0.0,  //1
            1.0, 0.0,  //3
            1.0, 1.0,  //4
            0.0, 1.0,  //5
            0.333, 0.333, 
            0.666, 0.6
        };
    
    geom.numOfAttrPerPoint = 1;
    geom.pointAttributes = vector<double>
        {0.0, 1.0, 11.0, 10.0, 0., 0.};

    geom.pointMarkers = vector<int>
        {1, 2, 3, 4, 5, 6};

    geom.segments = vector<int> {1, 2, 2, 3, 3, 4, 4, 1, 5, 6};//, 2, 6, 2, 7, 6, 7};

    geom.segmentMarkers = vector<int>{1, 2, 3, 4, 5};//, 6, 7, 8};

    geom.numOfRegions = 0;
    vector<double> regionList = {};
    

    //Triangle
    River::Triangle tria;
    //tria.EncloseConvexHull = true;
    //tria.AssignRegionalAttributes = false;
    //tria.DelaunayTriangles = true;
    tria.ConstrainAngle = true;
    tria.MaxAngle = 25;
    tria.AreaConstrain = true;
    tria.MaxTriaArea = 0.0003;

    geom = tria.Generate(geom);

    cout << "Tethex" << endl;
    tethex::Mesh TethexMesh;
    TethexMesh.read_triangl(geom.points, geom.triangles);
    TethexMesh.convert();
    auto[points, quads] = TethexMesh.write_triangle();
    geom.points = points;
    geom.triangles = quads;// fix this name
    cout << "GMSH " <<endl;
    //Visualization using GMSH object
    River::Gmsh Gmsh;
    Gmsh.setNodes(geom.points);
    Gmsh.setElements(geom.triangles, 3);

    /*
        Main River Class initializtion
    */
    cout << "RiverSIM" << endl;
    deallog.depth_console (2);
    River::Simulation RiverSim(vm);    
    RiverSim.SetMesh(geom);
    RiverSim.run();    

    Gmsh.StartUserInterface();

    return 0;
}