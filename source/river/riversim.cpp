#include "riversim.hpp"

/*
  
    Options Class
  
*/

void Mesh::Options::Print(bool qDetailedDescription)
{
    cout << "Triangle options command : " << getTriangleOptions() << endl;
    
    if(!qDetailedDescription) return;

    cout << "Detailed description: " << endl << endl; 

    if (ReadPSLG)
        cout << "(p) read PSLG" << endl;
    if (StartNumberingFromZero)
        cout << "(z) numbering starts from zero" << endl;
    if (Refine)
        cout << "(r) refine" << endl;
    if (AngleQuality > 0)
        cout << "(q) quality min 20 degree" << endl;
    if (MaxTriaArea > 0)
        cout << "(a) maximum triangle area constrain" << endl;
    if (DelaunayTriangles)
        cout << "(D) all traingles will be Delaunay" << endl;
    if (EncloseConvexHull)
        cout << "(c) enclose convex hull" << endl;
    if (CheckFinalMesh)
        cout << "(C) check final mesh" << endl;
    if (AssignRegionalAttributes)
        cout << "(A) assign additional attribute to each triangle which specifies segment which it belongs too" << endl;
    if (OutputEdges)
        cout << "(e) output list of edges" << endl;
    if (VoronoiDiagram)
        cout << "(v) outputs voronoi diagram" << endl;
    if (ComputeNeighbours)
        cout << "(n) outputs neighboors" << endl;
    if (SuppressBoundaryMarkers)
        cout << "(B) suppress boundary markers" << endl;
    if (SuppressPolyFile)
        cout << "(P) suppress output poly file(either don't work)" << endl;
    if (SuppressNodeFile)
        cout << "(N) suppress output nodes file(either don't work)" << endl;
    if (SuppressEleFile)
        cout << "(E) suppress output elements file(either don't work)" << endl;
    if (SuppressHoles)
        cout << "(O) suppress holes" << endl;
    if (SecondOrderMesh)
        cout << "(o2) second order mesh" << endl;
    if (SteinerPointsOnBoundary || SteinerPointsOnSegments)
        cout << "(Y) prohibits stainer points on boundary" << endl;
    if (MaxNumOfSteinerPoints > 0)
        cout << "(S) specify max number off added Steiner points" << endl;
    if (Algorithm == ITERATOR)
        cout << "(i) use incremental algorithm" << endl;
    if (Algorithm == FORUNE)
        cout << "(F) use Fortune algorithm" << endl;
    if (Quite)
        cout << "(Q) quite" << endl;
    if (Verbose)
        cout << "(V) verbose" << endl;
}


string Mesh::Options::getTriangleOptions()
{
    triaOpt = "";

    if (AngleQuality > 0 && AngleQuality < 35)
        triaOpt += "q" + to_string(AngleQuality);
    else if (AngleQuality >= 35)
        throw invalid_argument("Triangle quality angle should be less then 35");
    if (Refine)
        triaOpt += "r";
    if (MaxTriaArea > 0)
        triaOpt = +"a" + to_string(MaxTriaArea);
    if (DelaunayTriangles)
        triaOpt += "D";
    if (EncloseConvexHull)
        triaOpt += "c";
    if (CheckFinalMesh)
        triaOpt += "C";
    if (AssignRegionalAttributes)
        triaOpt += "A";
    if (VoronoiDiagram)
        triaOpt += "v";
    if (Quite)
        triaOpt += "Q";
    else if (Verbose)
        triaOpt += "V";
    if (Algorithm == FORUNE)
        triaOpt += "F";
    else if (Algorithm == ITERATOR)
        triaOpt += "i";
    if (ReadPSLG)
        triaOpt += "p";
    if (SuppressBoundaryMarkers)
        triaOpt += "B";
    if (SuppressPolyFile)
        triaOpt += "P";
    if (SuppressNodeFile)
        triaOpt += "N";
    if (SuppressEleFile)
        triaOpt += "E";
    if (OutputEdges)
        triaOpt += "e";
    if (ComputeNeighbours)
        triaOpt += "n";
    if (SuppressMehsFileNumbering)
        triaOpt += "I";
    if (SuppressExactArithmetics)
        triaOpt += "X";
    if (SuppressHoles)
        triaOpt += "O";
    if (SecondOrderMesh)
        triaOpt += "o2";
    if (SteinerPointsOnBoundary)
        triaOpt += "Y";
    if (SteinerPointsOnSegments)
        triaOpt += "YY";
    if (MaxNumOfSteinerPoints > 0)
        triaOpt += "S" + to_string(MaxNumOfSteinerPoints);

    return triaOpt;
}


/*

    Mesh Class

*/

Mesh::Mesh::Mesh()
{

}

Mesh::Mesh::~Mesh()
{
    trifree(NULL);
}

void Mesh::Mesh::SetMeshOptions(Options &meshOptions)
{
    meshOpt = meshOptions;
}

void Mesh::Mesh::Print(
    struct triangulateio &io)
{
    int i, j;

    if (io.numberofpoints > 0 && io.pointlist != NULL)
    {
        cout << "Total of points     : " << io.numberofpoints << endl;
        cout << "      of attributes : " << io.numberofpointattributes << endl;
        if (io.pointmarkerlist == NULL)
            cout << "Markers aren't set " << endl;

        for (i = 0; i < io.numberofpoints; i++)
        {
            cout << "Point " << i << ": ";

            for (j = 0; j < 2; j++)
                cout << io.pointlist[i * 2 + j] << " ";

            if (io.numberofpointattributes > 0 && io.pointattributelist != NULL)
            {
                cout << "   attributes  ";
                for (j = 0; j < io.numberofpointattributes; j++)
                    cout << io.pointattributelist[i * io.numberofpointattributes + j] << " ";
            }
            else
                cout << " point attributes aren't set" << endl;

            if (io.pointmarkerlist != NULL)
                cout << "   marker " << io.pointmarkerlist[i];

            cout << endl;
        }
    }
    else
        cout << "Points aren't set";

    cout << endl;

    cout << "Total of triangles:  " << io.numberoftriangles << endl;
    cout << "      of attributes: " << io.numberoftriangleattributes << endl;
    cout << "      of corners:    " << io.numberofcorners << endl;
    if (io.numberoftriangles > 0 && io.trianglelist != NULL)
    {
        cout << "Number of triangles: " << io.numberoftriangles << endl;
        for (i = 0; i < io.numberoftriangles; i++)
        {

            cout << "Triangle " << i << " points: ";
            for (j = 0; j < io.numberofcorners; j++)
                cout << io.trianglelist[i * io.numberofcorners + j] << " ";
            if (io.numberoftriangleattributes > 0 && io.triangleattributelist != NULL)
            {
                cout << "   attributes";
                for (j = 0; j < io.numberoftriangleattributes; j++)
                    cout << "  " << io.triangleattributelist[i * io.numberoftriangleattributes + j];
            }
            else
                cout << " no attributes ";

            if (io.trianglearealist != NULL)
                cout << " area constrain " << io.trianglearealist[i];
            else
                cout << " no area constrain";

            cout << "  "
                 << " neighbors";
            for (j = 0; j < 3; j++)
                cout << " " << io.neighborlist[i * 3 + j];

            cout << endl;
        }
    }
    else
        cout << "Triangles aren't set";

    cout << endl;

    cout << "Total of holes   : " << io.numberofholes << endl;
    cout << "Total of regions : " << io.numberofregions << endl;

    cout << "Total of segments:  " << io.numberofsegments << endl;
    if (io.numberofsegments > 0 && io.segmentlist != NULL)
        for (i = 0; i < io.numberofsegments; i++)
        {
            cout << "Segment " << i << " points: ";
            for (j = 0; j < 2; j++)
                cout << "  " << io.segmentlist[i * 2 + j];
            if (io.segmentmarkerlist != NULL)
                cout << "   marker " << io.segmentmarkerlist[i];
            else
                cout << " no marker" << endl;
            cout << endl;
        }
    else
        cout << "Segments aren't set";

    cout << endl;

    cout << "Total of edges:  " << io.numberofedges << endl;
    if (io.numberofedges > 0 && io.edgelist != NULL)
        for (i = 0; i < io.numberofedges; i++)
        {
            cout << "Edge " << i << " points:";
            for (j = 0; j < 2; j++)
                cout << "  " << io.edgelist[i * 2 + j];
            if (io.normlist != NULL)
                for (j = 0; j < 2; j++)
                    cout << "  " << io.normlist[i * 2 + j];
            if (io.edgemarkerlist != NULL)
                cout << "   marker " << io.edgemarkerlist[i];
            else
                cout << " no marker";
            cout << endl;
        }
    else
        cout << "Edges aren't set";
    cout << endl;
}


void Mesh::Mesh::TriangleGenerator()
{
    triangulateio in, out, vorout;
    set_tria_to_default(&in);
    set_tria_to_default(&out);
    set_tria_to_default(&vorout);
    //input data
    /*

    */
    char triswitches[] = {'p', 'z', 'V', 'A', 'q', '2', '0', 'D', 'C', 'e', 'c', 'v', 'n', '\0'};

    vector<double> pointList =
        {
            0.0, 0.0,  //0
            1.0, 0.0,  //2
            1.0, 10.0, //3
            0.0, 10.0  //4
        };

    vector<double> pointAttributeList =
        {0.0, 1.0, 11.0, 10.0};

    vector<int> pointMarkerList =
        {0, 2, 0, 0};

    vector<int> segmentList =
        {
            //0, 1, //1
            //1, 2, //2
            //2, 3, //3
            //3, 1
        };

    vector<int> segmentMarkerList =
        {
            //    0, 1, 2, 3
        };

    vector<double> regionList =
        {0.5, 5.0, 7.0, 0.1};

    //points
    in.numberofpoints = 4;
    in.numberofpointattributes = 1;
    in.pointlist = &pointList[0];
    in.pointattributelist = &pointAttributeList[0];
    in.pointmarkerlist = &pointMarkerList[0];

    //region
    in.numberofregions = 1;
    in.regionlist = &regionList[0];

    //edge - read only

    cout << endl
         << endl
         << "Input point set:" << endl
         << endl;
    Print(in);

    //mesh generation main call
    cout << endl
         << "Triangulation Started" << endl
         << endl;
    triangulate(triswitches, &in, &out, &vorout);

    cout << endl
         << endl
         << "Output triangulation:" << endl
         << endl;
    Print(out);

    cout << endl
         << endl
         << " Voronoi diagram:" << endl
         << endl;
    Print(vorout);
}





/*

    RiverSim Class

*/

RiverSim::RiverSim(po::variables_map &vm) : fe(1), dof_handler(triangulation)
{
    option_map = vm;

    /*
        GMSH intialization
    */

    gmsh::initialize();
    gmsh::option::setNumber("Mesh.RecombineAll", 1);
    gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::option::setNumber("General.Terminal", (int)option_map["gmsh-log"].as<bool>());
    mdl::add("square");
}

RiverSim::~RiverSim()
{
    gmsh::finalize();
}

void RiverSim::geo_mesh_generator()
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
    mdl::mesh::generate(2);
}

void RiverSim::gmsh_mesh_generator()
{
    //TODO: implement right workwflow

    /*
        defining of geometry
    */
    cout << "discr entity" << endl;

    mdl::addDiscreteEntity(1, 1);

    cout << "set nodes" << endl;
    msh::setNodes(1, 1,
                  {1, 2, 3, 4, 5, 6, 7},
                  {
                      0., 0., 0.,   //node 1
                      0.5, 0., 0.,  //node 2
                      1., 0., 0.,   //node 3
                      1., 1., 0.,   //node 4
                      0., 1., 0.,   //node 5
                      0.5, 0.2, 0., //node 6
                      0.5, 0.6, 0.  //node 7
                  });

    cout << "set elements" << endl;
    msh::setElements(1, 1,
                     {1},                                           //line element
                     {{1, 2, 3, 4, 5, 6, 7}},                       //line tags
                     {{1, 2, 2, 6, 6, 2, 2, 3, 3, 4, 4, 5, 5, 1}}); //lines

    geo::addPoint(0.5, 0.6, 0., 0.01, 99);
    geo::addCurveLoop({1}, 100);
    geo::addPlaneSurface({100}, 101);
    geo::synchronize();

    mdl::mesh::generate(2);
}

void RiverSim::make_custom_grid()
{
    static const Point<2> vertices_1[] =
        {Point<2>(0., 0.),
         Point<2>(1, 0.),
         Point<2>(0, 1.),
         Point<2>(1., 1.)};

    const unsigned int
        n_vertices = sizeof(vertices_1) / sizeof(vertices_1[0]);

    const vector<Point<dim>> vertices(&vertices_1[0],
                                      &vertices_1[n_vertices]);

    static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell] = {{0, 1, 2, 3}};

    const unsigned int
        n_cells = sizeof(cell_vertices) / sizeof(cell_vertices[0]);

    vector<CellData<dim>> cells(n_cells, CellData<dim>());

    for (unsigned int i = 0; i < n_cells; ++i)
    {
        for (unsigned int j = 0;
             j < GeometryInfo<dim>::vertices_per_cell;
             ++j)
            cells[i].vertices[j] = cell_vertices[i][j];
        cells[i].material_id = 0;
    }

    triangulation.create_triangulation(vertices,
                                       cells,
                                       SubCellData());

    triangulation.refine_global(5);
}

void RiverSim::setup_system()
{
    dof_handler.distribute_dofs(fe);
    cout << "Number of degrees of freedom: "
         << dof_handler.n_dofs()
         << endl;

    DynamicSparsityPattern dsp(dof_handler.n_dofs());

    DoFTools::make_sparsity_pattern(dof_handler, dsp);
    sparsity_pattern.copy_from(dsp);
    system_matrix.reinit(sparsity_pattern);
    solution.reinit(dof_handler.n_dofs());
    system_rhs.reinit(dof_handler.n_dofs());
}

void RiverSim::assemble_system()
{
    QGauss<2> quadrature_formula(2);
    FEValues<2> fe_values(fe, quadrature_formula,
                          update_values | update_gradients | update_JxW_values);
    const unsigned int dofs_per_cell = fe.dofs_per_cell;
    const unsigned int n_q_points = quadrature_formula.size();
    FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
    Vector<double> cell_rhs(dofs_per_cell);
    std::vector<types::global_dof_index> local_dof_indices(dofs_per_cell);
    DoFHandler<2>::active_cell_iterator
        cell = dof_handler.begin_active(),
        endc = dof_handler.end();

    for (; cell != endc; ++cell)
    {
        fe_values.reinit(cell);
        cell_matrix = 0;
        cell_rhs = 0;
        for (unsigned int q_index = 0; q_index < n_q_points; ++q_index)
        {
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                for (unsigned int j = 0; j < dofs_per_cell; ++j)
                    cell_matrix(i, j) += (fe_values.shape_grad(i, q_index) *
                                          fe_values.shape_grad(j, q_index) *
                                          fe_values.JxW(q_index));
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                cell_rhs(i) += (fe_values.shape_value(i, q_index) *
                                1 *
                                fe_values.JxW(q_index));
        }

        cell->get_dof_indices(local_dof_indices);

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
            for (unsigned int j = 0; j < dofs_per_cell; ++j)
                system_matrix.add(local_dof_indices[i],
                                  local_dof_indices[j],
                                  cell_matrix(i, j));

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
            system_rhs(local_dof_indices[i]) += cell_rhs(i);
    }

    std::map<types::global_dof_index, double> boundary_values;

    VectorTools::interpolate_boundary_values(dof_handler,
                                             0,
                                             ZeroFunction<2>(),
                                             boundary_values);

    MatrixTools::apply_boundary_values(boundary_values,
                                       system_matrix,
                                       solution,
                                       system_rhs);
}

void RiverSim::solve()
{
    SolverControl solver_control(1000, 1e-12);
    SolverCG<> solver(solver_control);
    solver.solve(system_matrix, solution, system_rhs,
                 PreconditionIdentity());
}

void RiverSim::output_results() const
{
    DataOut<2> data_out;
    data_out.attach_dof_handler(dof_handler);
    data_out.add_data_vector(solution, "solution");
    data_out.build_patches();
    std::ofstream output("solution.vtk");
    data_out.write_vtk(output);
}

void RiverSim::run()
{
    make_custom_grid();
    setup_system();
    assemble_system();
    solve();
    output_results();

    if (option_map.count("b1"))
        gmsh_mesh_generator();
    else
        geo_mesh_generator();

    if (option_map["draw-mesh"].as<bool>())
        gmsh::fltk::run();

    if (option_map.count("output"))
        gmsh::write(option_map["output"].as<string>());
}
























































void gmsh_possibilites(int argc, char *argv[])
{
    const int meshDimension = 2; 

    cout << "init" << endl;
    gmsh::initialize(argc, argv);

    //only this format is supported by deal.ii
    //gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);

    //create new test model
    cout << "add" << endl;
    mdl::add("test");
    vector<string> model_list;
    mdl::list(model_list);
    cout << "List of models" << endl;
    for(auto m : model_list)
        cout << m << endl;
    cout << "Model dimenssions: " << mdl::getDimension() << endl;

    
    /*
        defining of geometry
    */
    int geomTag = 1;
    cout << "discr entity" << endl;
    mdl::addDiscreteEntity(meshDimension, geomTag);
    
    //node points
    cout << "set nodes" << endl;
    msh::setNodes(
        meshDimension, 
        geomTag, 
        {1, 2, 3, 4, 5}, 
        {0., 0., 0.,   //node 1 
         0.5, 0., 0.,  //node 2
         1., 0., 0.,   //node 3
         1., 1., 0.,   //node 4
         0., 1., 0.}); //node 5

    //elemets definition(e.g lines)
    //vector<int> elementTypes{1};//1-line, 2 -triangle, see gmsh.pdf for more details
    //vector<vector<int>> elementTags = {{1, 1, 1, 1}};
    //vector<vector<int>> nodeTags = {{1,2,2,3,3,4,4,1}};
    cout << "set elements" << endl;
    msh::setElements(
        meshDimension, 
        geomTag, 
        {1}, 
        {{1, 2, 3, 4, 5}},
        {{1, 2, 2, 3, 3, 4, 4, 5, 5, 1}});

    //Mesh generation
    //msh::generate(meshDimension);
    //gmsh::graphics::draw();
    gmsh::fltk::initialize();
    gmsh::fltk::wait();
    gmsh::fltk::run();
    
    //gmsh::write("playing.msh");
    //finalizing working with gmsh library
    cout << "fina" << endl;
    gmsh::finalize();
}

