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

void dealii_custom_triangulation(Triangulation<2> &coarse_grid)
{
    const unsigned int dim = 2;
    static const Point<dim> vertices_1[]
        = {  Point<dim> (-1.,   -1.),
             Point<dim> (-1./2, -1.),
             Point<dim> (0.,    -1.),
             Point<dim> (+1./2, -1.),
             Point<dim> (+1,    -1.),
             Point<dim> (-1.,   -1./2.),
             Point<dim> (-1./2, -1./2.),
             Point<dim> (0.,    -1./2.),
             Point<dim> (+1./2, -1./2.),
             Point<dim> (+1,    -1./2.),
             Point<dim> (-1.,   0.),
             Point<dim> (-1./2, 0.),
             Point<dim> (+1./2, 0.),
             Point<dim> (+1,    0.),
             Point<dim> (-1.,   1./2.),
             Point<dim> (-1./2, 1./2.),
             Point<dim> (0.,    1./2.),
             Point<dim> (+1./2, 1./2.),
             Point<dim> (+1,    1./2.),
             Point<dim> (-1.,   1.),
             Point<dim> (-1./2, 1.),
             Point<dim> (0.,    1.),
             Point<dim> (+1./2, 1.),
             Point<dim> (+1,    1.)
          };
    const unsigned int
    n_vertices = sizeof(vertices_1) / sizeof(vertices_1[0]);
    const std::vector<Point<dim> > vertices (&vertices_1[0],
                                             &vertices_1[n_vertices]);
    static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell]
      = {{0, 1, 5, 6},
        {1, 2, 6, 7},
        {2, 3, 7, 8},
        {3, 4, 8, 9},
        {5, 6, 10, 11},
        {8, 9, 12, 13},
        {10, 11, 14, 15},
        {12, 13, 17, 18},
        {14, 15, 19, 20},
        {15, 16, 20, 21},
        {16, 17, 21, 22},
        {17, 18, 22, 23}
    };
    const unsigned int
    n_cells = sizeof(cell_vertices) / sizeof(cell_vertices[0]);

    std::vector<CellData<dim> > cells (n_cells, CellData<dim>());

    for (unsigned int i=0; i<n_cells; ++i)
    {
        for (unsigned int j=0;
             j < GeometryInfo<dim>::vertices_per_cell;
             ++j)
            cells[i].vertices[j] = cell_vertices[i][j];
        cells[i].material_id = 0;
    }
    
    coarse_grid.create_triangulation (vertices,
                                        cells,
                                        SubCellData());
    //coarse_grid.refine_global (1);
}

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
    /*
        program options
    */

    // Declaring group of options that will be allowed on both
    //command line and config file
    po::options_description config("Configuration");
    //declare supported options
    config.add_options()
        ("help,h", "produce help message")
        ("version,v", "print version string")
        ("test,t", po::value<string>(), "test option")
        ("sw1,s", po::value<int>()->default_value(0), "used for custom switch between different flows")
        ("sw2", po::value<int>()->default_value(1), "used for custom switch between different flows")
        ("sw2", po::value<int>()->default_value(3), "used for custom switch between different flows")
        ("b1,b", "used for custom switch between different flows" )
        ("b2", "used for custom switch between different flows" )
        ("b3", "used for custom switch between different flows" )
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
    
    
    if (vm.count("test")) {
        cout << "test option was set to " 
             << vm["test"].as<string>() << endl;
    } else {
        cout << "test was not set." << endl;
    }

    dealii_test();

    gmsh::initialize();
    gmsh::option::setNumber("Mesh.RecombineAll", 1);
    gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::option::setNumber("General.Terminal", (int)vm["gmsh-log"].as<bool>());
    
    mdl::add("square");

    if (vm.count("b1"))
        basicgeneration();
    else
        geogeneration();
        

    cout << "mesh generation" << endl;
    mdl::mesh::generate(2);

    //print_nodes_and_elements();
    
    if (vm["draw-mesh"].as<bool>())
        gmsh::fltk::run();
    if(vm.count("output"))
        gmsh::write(vm["output"].as<string>());

    gmsh::finalize();
    
    return 0;
}