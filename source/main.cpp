#include <iostream>
#include "riversim.hpp"

using namespace std;

void geogeneration()
{
    geo::addPoint(0, 0, 0, 0.1, 1);
    geo::addPoint(0.5, 0, 0, 0.1, 2);
    geo::addPoint(1, 0, 0, 0.1, 3);
    geo::addPoint(1, 1, 0, 0.1, 4);
    geo::addPoint(0, 1, 0, 0.1, 5);
    geo::addPoint(0.5, 0.2, 0, 0.1, 6);

    geo::addLine(1, 2, 1);
    geo::addLine(2, 3, 2);
    geo::addLine(3, 4, 3);
    geo::addLine(4, 5, 4);
    geo::addLine(5, 1, 5);
    geo::addLine(2, 6, 6);
    geo::addLine(6, 2, 7);

    geo::addCurveLoop({1, 2, 3, 4, 5, 6, 7}, 1);
    geo::addPlaneSurface({1}, 6);
    geo::synchronize();
}

void basicgeneration()
{

    //TODO: implement right workwflow 
    cout << "List of models: " << mdl::list << endl;

    const int meshDimension = 2; 
    
    /*
        defining of geometry
    */
    const int geomTag = 1;
    cout << "discr entity" << endl;
    
    mdl::addDiscreteEntity(meshDimension, geomTag);
    
    //node points
    //auto nodesTag = {1, 2, 3, 4, 5};
    cout << "set nodes" << endl;
    msh::setNodes(meshDimension, geomTag, 
        {1, 2, 3, 4, 5, 6, 7, 8, 9}, 
        {0.,  0.,  0.,   //node 1 
         1.,  0.,  0.,   //node 2
         1.,  1.,  0.,   //node 3
         0.,  1.,  0.,   //node 4

         0.5, 0.,  0.,   //node 5
         1.,  0.2, 0.,   //node 6
         0.5, 1.,  0.,   //node 7
         0.,  0.2, 0.,   //node 8
         0.5, 0.2, 0.}); // node 9           

    cout << "set elements" << endl;
    msh::setElements( meshDimension, geomTag, 
        {15}, 
        {{1, 5, 9}},
        {{1, 5, 9}});
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
    cout << "=================================" << endl;
    cout << "=================================" << endl;
    cout << "=================================" << endl << endl;
    process_input_options(argc, argv);

    gmsh::initialize();
    gmsh::option::setNumber("Mesh.RecombineAll", 1);
    gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::option::setNumber("General.Terminal", 1);
    
    mdl::add("square");
    geogeneration();
    //basicgeneration();

    cout << "mesh generation" << endl;
    mdl::mesh::generate(2);

    gmsh::graphics::draw();
    print_nodes_and_elements();
         
    gmsh::fltk::run();
    gmsh::write("test.mesh");
    gmsh::finalize();
    
    return 0;
}