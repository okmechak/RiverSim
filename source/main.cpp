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

struct River::vecTriangulateIO RotatingGeometry(double alpha = 0.)
{
    struct River::vecTriangulateIO geom;
    geom.points = vector<double>
        {
            -1.0, -1.0,  //1
            1.0, -1.0,  //2
            1.0, 1.0,  //3
            -1.0, 1.0,  //4
            0.5 * cos(alpha), 0.5 * sin(alpha),
            - 0.5 * cos(alpha), - 0.5 * sin(alpha)
        };
    
    geom.numOfAttrPerPoint = 0;
    geom.pointAttributes = vector<double>{};

    geom.pointMarkers = vector<int>
        {1, 1, 1, 1, 1, 1};

    geom.segments = vector<int> 
    {1, 2, 
    2, 3, 
    3, 4, 
    4, 1, 
    5, 6};
    
    /*markers:
        1 - dirichlet
        4 - dirichlet too but on river
        2 - zero neuman
        3 - non zero neuman
    */
    
    geom.segmentMarkers = vector<int>{1, 1, 1, 1, 1};

    geom.numOfRegions = 0;
    vector<double> regionList = {};
    
    return geom;
}


struct River::vecTriangulateIO CircularGeometry()
{
    struct River::vecTriangulateIO geom;
    geom.points = vector<double>
        {
            -1.0,-1.0,  //1
            0.0, -1.0,  //2
            1.0, -1.0,  //3
            1.0,  1.0,  //4
            -1.0, 1.0,  //5
            0.,   0. //6
        };
    
    geom.numOfAttrPerPoint = 0;
    geom.pointAttributes = vector<double>{};

    geom.pointMarkers = vector<int>
        {1, 1, 1, 1, 1, 1};

    geom.segments = vector<int> 
    {1, 2, 
    2, 6, 
    6, 2, 
    2, 3, 
    3, 4,
    4, 1};
    
    /*markers:
        1 - dirichlet
        4 - dirichlet too but on river
        2 - zero neuman
        3 - non zero neuman
    */
    
    geom.segmentMarkers = vector<int>{1, 1, 1, 1, 1, 1};

    geom.numOfRegions = 0;
    vector<double> regionList = {};
    
    return geom;
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
        ("Verbose,V", po::value<bool>()->default_value(false), "print detailed log to terminal")
        ("Quiet,Q", po::value<bool>()->default_value(false), "print detailed log to terminal")
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
    //initialization of objects
    //Triangle
    River::Triangle tria;
    tria.Verbose = vm["Verbose"].as<bool>();
    tria.Quite = vm["Quiet"].as<bool>();


    //Tethex
    River::Tethex tet;
    tet.Verbose = vm["Verbose"].as<bool>();
    //Geomerty
    auto geom = CircularGeometry();
    //Triangulate
    geom = tria.Generate(geom);
    //Covert to quadrangles
    tet.Convert(geom);

    River::Gmsh Gmsh;
    Gmsh.TestMesh(geom);
    Gmsh.Write();
    
    //Solve
    River::Simulation RiverSim(vm);    
    //RiverSim.SetMesh(geom);
    RiverSim.OpenMesh();
    RiverSim.run();
    
    std::cout << "GMSH " <<endl;
    //Visualization using GMSH object
    //Gmsh.setNodes(geom.points);
    //Gmsh.setElements(geom.triangles, 3);
    Gmsh.StartUserInterface();
    

    return 0;
}