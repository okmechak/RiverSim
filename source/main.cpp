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
    struct Mesh::vecTriangulateIO geom;
    geom.points = vector<double>
        {
            0.0, 0.0,  //0
            1.0, 0.0,  //2
            1.0, 10.0, //3
            0.0, 10.0  //4
        };
    
    geom.numOfAttrPerPoint = 1;
    geom.pointAttributes = vector<double>
        {0.0, 1.0, 11.0, 10.0};

    geom.pointMarkers = vector<int>
        {0, 2, 0, 0};

    geom.segments = vector<int> 
        {
            //0, 1, //1
            //1, 2, //2
            //2, 3, //3
            //3, 1
        };

    geom.segmentMarkers = vector<int>
        {
            //    0, 1, 2, 3
        };

    geom.numOfRegions = 1;
    vector<double> regionList =
        {0.5, 5.0, 7.0, 0.1};
    

    //Triangle
    Mesh::Triangle tria;
    tria.Verbose = true;
    tria.EncloseConvexHull = true;
    tria.VoronoiDiagram = true;
    tria.Generate(geom);

    


    
    
    


    /*
        Main River Class initializtion
    */
    if(!vm.count("ss"))
    {
        deallog.depth_console (2);
        RiverSim River(vm);    
        River.run();    
    }

    return 0;
}