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


void print_nodes_and_elements()
{
    /*
        Nodes
    */
    vector<int> nodeTags;
    vector<double> coords, parametricCoords;
    const int dim = 2, tag = -1;
    bool bIncludeBoundary = true;

    
    msh::getNodes(nodeTags, coords, parametricCoords, dim, tag, bIncludeBoundary);

    cout << "With boundaries" << endl;
    for(unsigned int i = 0; i < nodeTags.size(); ++i)
        cout << i << "  tag: " << nodeTags[i] << "  coords: " 
        << coords[3*i] << "  " << coords[3*i + 1] << "  " << coords[3*i + 2] << endl;

    /*
        Elements
    */
    vector<int> elementTypes;
    vector<vector<int>> elementTags, elemNodeTags;

    msh::getElements(elementTypes, elementTags, elemNodeTags);

    cout << endl << endl << "Element Types" << endl;
    for(unsigned int i = 0; i < elementTags.size(); ++i){
        cout << "Element Types - " << elementTypes[i] << endl;
        for(unsigned int j = 0; j < elementTags[i].size(); ++j)
            cout << "  " << elementTags[i][j];
        cout << endl;
    }

    cout << endl << endl << "Element Tags and Node Tags" << endl;
    for(unsigned int i = 0; i < elemNodeTags.size(); ++i){
        cout << "Element Types - " << elementTypes[i] << endl;
        for(unsigned int j = 0; j < elemNodeTags[i].size(); ++j)
            cout << "  " << elemNodeTags[i][j];
        cout << endl;
    }

    //what about get boundaries?
    gmsh::vectorpair inDimTag, outDimTag;
    mdl::getEntities(inDimTag);
    mdl::getBoundary(inDimTag, outDimTag, true, true, true);
    cout << "Boundary entities" << endl;
    for(auto el: outDimTag)
        cout << "dim - " <<  el.first << " dim - " << el.second << endl;
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
        Main River Class initializtion
    */
    deallog.depth_console (2);
    
    RiverSim River(vm);    
    River.run();    

    return 0;
}