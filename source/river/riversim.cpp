#include "riversim.hpp"

namespace River{
/*
  
    Triangle Class
  
*/
Triangle::Triangle()
{
    SetAllValuesToDefault();
    
    if (Verbose){
        cout << "Default Options: " << endl;
        PrintOptions(true);
    }
}


Triangle::~Triangle()
{
    FreeAllocatedMemory();
}


void Triangle::SetAllValuesToDefault()
{
    updateOptions();
    set_tria_to_default(&in);
    set_tria_to_default(&out);
    set_tria_to_default(&vorout);
}


void Triangle::FreeAllocatedMemory()
{
    triangulateiofree(&out);
    triangulateiofree(&vorout);
}


string Triangle::updateOptions()
{
    options = "";

    if (ConstrainAngle)
        options += "q";
    if (ConstrainAngle && MaxAngle > 0 && MaxAngle < 35)
        options += to_string(MaxAngle);
    else if (ConstrainAngle && MaxAngle >= 35)
        throw invalid_argument("Triangle quality angle should be less then 35");
    if (StartNumberingFromZero)
        options += "z";
    if (Refine)
        options += "r";
    if (AreaConstrain)
        options += "a";
    if (AreaConstrain && MaxTriaArea > 0)
        options +=  to_string(MaxTriaArea);
    if (DelaunayTriangles)
        options += "D";
    if (EncloseConvexHull)
        options += "c";
    if (CheckFinalMesh)
        options += "C";
    if (AssignRegionalAttributes)
        options += "A";
    if (VoronoiDiagram)
        options += "v";
    if (Quite)
        options += "Q";
    else if (Verbose)
        options += "V";
    if (Algorithm == FORUNE)
        options += "F";
    else if (Algorithm == ITERATOR)
        options += "i";
    if (ReadPSLG)
        options += "p";
    if (SuppressBoundaryMarkers)
        options += "B";
    if (SuppressPolyFile)
        options += "P";
    if (SuppressNodeFile)
        options += "N";
    if (SuppressEleFile)
        options += "E";
    if (OutputEdges)
        options += "e";
    if (ComputeNeighbours)
        options += "n";
    if (SuppressMehsFileNumbering)
        options += "I";
    if (SuppressExactArithmetics)
        options += "X";
    if (SuppressHoles)
        options += "O";
    if (SecondOrderMesh)
        options += "o2";
    if (SteinerPointsOnBoundary)
        options += "Y";
    if (SteinerPointsOnSegments)
        options += "YY";
    if (MaxNumOfSteinerPoints > 0)
        options += "S" + to_string(MaxNumOfSteinerPoints);

    return options;
}


void Triangle::PrintOptions(bool qDetailedDescription)
{
    updateOptions();

    cout << "Triangle options command : " << options << endl;
    
    if(!qDetailedDescription) return;

    cout << "Detailed description: " << endl << endl; 

    if (ReadPSLG)
        cout << "(p) read PSLG" << endl;
    if (StartNumberingFromZero)
        cout << "(z) numbering starts from zero" << endl;
    if (Refine)
        cout << "(r) refine" << endl;
    if (ConstrainAngle)
        cout << "(q) quality min 20 degree" << endl;
    if (MaxAngle > 0)
        cout << "                    value:" << MaxAngle << endl;
    if(AreaConstrain)
        cout << "(a) area constrain: " << endl;
    if (MaxTriaArea > 0)
        cout << "                  area constrain: " << MaxTriaArea << endl;
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


void Triangle::PrintGeometry(
    struct triangulateio &io)
{
    int i, j, shift = 1;
    if(StartNumberingFromZero)
        shift = 0;

    if (io.numberofpoints > 0 && io.pointlist != NULL)
    {
        cout << "Total of points     : " << io.numberofpoints << endl;
        cout << "         attributes : " << io.numberofpointattributes << endl;
        if (io.pointmarkerlist == NULL)
            cout << "Markers aren't set " << endl;

        for (i = 0; i < io.numberofpoints; i++)
        {
            cout << "Point " << i + shift << ": ";

            for (j = 0; j < 2; j++)
                cout << io.pointlist[i * 2 + j] << " ";

            if (io.numberofpointattributes > 0 && io.pointattributelist != NULL)
            {
                cout << "   attributes  ";
                for (j = 0; j < io.numberofpointattributes; j++)
                    cout << io.pointattributelist[i * io.numberofpointattributes + j] << " ";
            }
            else
                cout << " point attributes aren't set ";

            if (io.pointmarkerlist != NULL)
                cout << "   marker " << io.pointmarkerlist[i];

            cout << endl;
        }
    }
    else
        cout << "Points aren't set";

    cout << endl;

    cout << "Total of triangles:  " << io.numberoftriangles << endl;
    cout << "         attributes: " << io.numberoftriangleattributes << endl;
    cout << "         corners:    " << io.numberofcorners << endl;
    if (io.numberoftriangles > 0 && io.trianglelist != NULL)
    {
        for (i = 0; i < io.numberoftriangles; i++)
        {
            cout << "Triangle " << i + shift << " points: ";
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
            if(io.neighborlist != NULL)
                for (j = 0; j < 3; j++)
                    cout << " " << io.neighborlist[i * 3 + j];
            else
                cout << " no neighbors ";

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
            cout << "Segment " << i + shift << " points: ";
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
            cout << "Edge " << i + shift << " points:";
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

void Triangle::SetGeometry(struct triangulateio geom)
{
    in = geom;
}


struct triangulateio* Triangle::GetGeometry()
{
    return &out;
}

struct triangulateio Triangle::toTriaStructure(struct vecTriangulateIO& geom)
{
  struct triangulateio triaStr;
  set_tria_to_default(&triaStr);

  if(!geom.points.empty())
  {
    triaStr.pointlist = &geom.points[0];                            /* In / out */
    triaStr.pointattributelist = &geom.pointAttributes[0];          /* In / out */
    triaStr.pointmarkerlist = &geom.pointMarkers[0];                /* In / out */
    triaStr.numberofpoints = geom.points.size()/2;                /* In / out */
    triaStr.numberofpointattributes = geom.numOfAttrPerPoint;       /* In / out */
  }

  if(!geom.triangles.empty())
  {
    triaStr.trianglelist = &geom.triangles[0];                      /* In / out */
    triaStr.triangleattributelist = &geom.triangleAttributes[0];    /* In / out */
    triaStr.trianglearealist = &geom.triangleAreas[0];              /* In only */
    triaStr.neighborlist = NULL;                               /* Out only */
    triaStr.numberoftriangles = geom.triangles.size()/3;            /* In / out */
    cout << "num of triangles " << triaStr.numberoftriangles << endl;
    if (SecondOrderMesh)
      triaStr.numberoftriangles = geom.triangles.size()/6;
    triaStr.numberofcorners = 3;                               /* In / out */
    triaStr.numberoftriangleattributes = geom.numOfAttrPerTriangle; /* In / out */
  }

  if(!geom.segments.empty())
  {
    triaStr.segmentlist = &geom.segments[0];                        /* In / out */
    triaStr.segmentmarkerlist = &geom.segmentMarkers[0];            /* In / out */
    triaStr.numberofsegments = geom.segments.size() / 2;            /* In / out */
  }

  if(!geom.holes.empty())
  {
    triaStr.holelist = &geom.holes[0];               /* In / pointer to array copied out */
    triaStr.numberofholes = geom.holes.size() / 2; /* In / copied out */
  }

  if(!geom.regions.empty())
  {
    triaStr.regionlist = &geom.regions[0];           /* In / pointer to array copied out */
    triaStr.numberofregions = geom.numOfRegions;     /* In / copied out */
  }

  return triaStr;
}

struct vecTriangulateIO Triangle::toVectorStructure(struct triangulateio* triaStr, bool b3D)
{
  struct vecTriangulateIO vecOut;
  int i;
  double z0 = 0.;
  if(b3D) dim = 3;
  //Triangle generates two dimensional data, but almost all packages need three dimmnesion
  //so we have such possibility
  //reserve memory for vector containers
  
  vecOut.points.reserve(dim * triaStr->numberofpoints);
  vecOut.pointAttributes.reserve(triaStr->numberofpoints * triaStr->numberofpointattributes);
  vecOut.pointMarkers.reserve(triaStr->numberofpoints);
  vecOut.numOfAttrPerPoint = triaStr->numberofpointattributes;

  vecOut.segments.reserve(2 * triaStr->numberofsegments);
  vecOut.segmentMarkers.reserve(triaStr->numberofsegments);

  vecOut.triangles.reserve(3 * triaStr->numberoftriangles);
  vecOut.triangleAreas.reserve(triaStr->numberoftriangles);
  vecOut.triangleAttributes.reserve(triaStr->numberoftriangles * triaStr->numberoftriangleattributes);
  vecOut.numOfAttrPerTriangle = triaStr->numberoftriangleattributes;

  vecOut.holes.reserve(dim * triaStr->numberofholes);

  vecOut.regions.reserve(dim * triaStr->numberofregions);
  vecOut.numOfRegions = triaStr->numberofregions;

  vecOut.edges.reserve(dim * triaStr->numberofedges);
  vecOut.edgeMarkers.reserve(triaStr->numberofedges);

  vecOut.neighbors.reserve(triaStr->numberoftriangles * 3);
  
  for(i = 0; i < 2 * triaStr->numberofpoints; ++i){
    vecOut.points.push_back(triaStr->pointlist[i]);
    if(b3D && i % 2 != 0)
        vecOut.points.push_back(z0);
    if(i < triaStr->numberofpoints)
        vecOut.pointMarkers.push_back(triaStr->pointmarkerlist[i]);
  }

  
  for(i = 0; i < triaStr->numberofpoints * triaStr->numberofpointattributes; ++i)
    vecOut.pointAttributes.push_back(triaStr->pointattributelist[i]);

  
  for(i = 0; i < 2 * triaStr->numberofsegments; ++i){
    vecOut.segments.push_back(triaStr->segmentlist[i]);
    if (i < triaStr->numberofsegments)
        vecOut.segmentMarkers.push_back(triaStr->segmentmarkerlist[i]);
  }

  
  for(i = 0; i < 3*triaStr->numberoftriangles; ++i){
    vecOut.triangles.push_back(triaStr->trianglelist[i]);
    if(triaStr->trianglearealist != NULL && i < triaStr->numberoftriangles)
        vecOut.triangleAreas.push_back(triaStr->trianglearealist[i]);
  }

  
  for(i = 0; i < triaStr->numberoftriangles * triaStr->numberoftriangleattributes; ++i)
    vecOut.triangleAttributes.push_back(triaStr->triangleattributelist[i]);

  
  for(i = 0; i < dim * triaStr->numberofholes; ++i){
    vecOut.holes.push_back(triaStr->holelist[i]);
    if(b3D && i % 2 != 0)
        vecOut.holes.push_back(z0);
  }

  
  for(i = 0; i < dim * triaStr->numberofregions; ++i){
    vecOut.regions.push_back(triaStr->regionlist[i]);
    if(b3D && i % 2 != 0)
        vecOut.regions.push_back(z0);
  }

  
  for(i = 0; i < 2 * triaStr->numberofedges; ++i){
    vecOut.edges.push_back(triaStr->edgelist[i]);
    if (i < triaStr->numberofedges)
        vecOut.edgeMarkers.push_back(triaStr->edgemarkerlist[i]);
  }
  return vecOut;
}


struct triangulateio* Triangle::GetVoronoi()
{
    return &vorout;
}


struct vecTriangulateIO Triangle::Generate(struct vecTriangulateIO &geom)
{
    SetAllValuesToDefault();
    
    in = toTriaStructure(geom);

    if (Verbose){
        cout << "Input Geometry: " << endl;
        PrintGeometry(in);
    }

    //Main call to Triangle
    triangulate(options.c_str(), &in, &out, &vorout);
    
    if (Verbose){
        cout << "Output Geometry: " << endl;
        PrintGeometry(out);
    }
    if (Verbose && VoronoiDiagram)
    {
        cout << "Voronoi Diagram: " << endl;
        PrintGeometry(vorout);
    }
    
    struct vecTriangulateIO OutputMesh = toVectorStructure(&out);

    FreeAllocatedMemory();

    return OutputMesh;
}

Tethex::Tethex()
{}
Tethex::~Tethex()
{}

void Tethex::Convert(struct vecTriangulateIO &geom)
{
    tethex::Mesh TethexMesh;
    TethexMesh.read_triangl(
        geom.points, 
        geom.edges, 
        geom.edgeMarkers,  
        geom.triangles);

    if(Verbose)
        TethexMesh.info(cout);
    TethexMesh.convert();
    if(Verbose)
        TethexMesh.info(cout);

    //TODO: write normal arguments    
    TethexMesh.write_triangle(
        geom.points,
        geom.segments,
        geom.segmentMarkers,
        geom.triangles);

}



Gmsh::Gmsh()
{
  gmsh::initialize();
  //gmsh::option::setNumber("Mesh.RecombineAll", 1);
  gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
  gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
  gmsh::option::setNumber("General.Terminal", 1);
  gmsh::model::add(modelName);
  gmsh::model::addDiscreteEntity(2, 1);
}

Gmsh::~Gmsh()
{
    gmsh::finalize();
}

void Gmsh::Open()
{
    gmsh::open(fileName);
}

void Gmsh::refine()//just code samles
{
    //First
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

    //second
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

void Gmsh::Write()
{
    gmsh::write(fileName);
}

void Gmsh::setNodes(vector<double> nodes, int dim, int tag)
{
    auto tags = evaluateTags(nodes.size()/3, 1);
    
    gmsh::model::mesh::setNodes(
        dim, 
        tag, 
        tags,
        nodes);
}

void Gmsh::setElements(vector<int> elements, int elType, int dim, int tag)
{
    vector<vector<int>> tags(1);
    //FIXME: this divider depends on element type
    //for triangles(el type = 2) - it will be 3
    //for quads (el type - 4) - it will be 4
    int div = 0;
    switch(elType){
        case 1: div = 2; break;
        case 2: div = 3; break;
        case 3: div = 4; break;
    }

    tags[0] = evaluateTags(elements.size()/div, 1);

    vector<int> elementTypes = {elType};

    vector<vector<int>> elementsContainer;
    elementsContainer.push_back(elements);

    gmsh::model::mesh::setElements(
        dim,
        tag,
        elementTypes,
        tags,
        elementsContainer
    );
}


void Gmsh::StartUserInterface()
{
    gmsh::fltk::run();
}









/*

    RiverSim Class

*/

Simulation::Simulation(po::variables_map &vm) : fe(1), dof_handler(triangulation)
{
    option_map = vm;
}

Simulation::~Simulation()
{

}

void Simulation::SetMesh(struct vecTriangulateIO & mesh)
{
    const int dim = 2; //FIXME: set dim somewhere else
    
    //VERTICES
    auto n_points = mesh.points.size() / 3; //FIXME: replace hardcoded 3 by more general thing
    vector<Point<dim>> vertices(n_points);
    for(unsigned int i = 0; i < n_points; ++i)
    {
        vertices[i] = Point<dim>(mesh.points[3 * i], mesh.points[3 * i + 1]);
    }
    
    //generat boundary id structure
    std::unordered_map<std::pair<int,int>, int> boundary_ids;

    //SETTING QUADRANGLES
    auto n_cells = mesh.triangles.size() / 4; //FIXME: remove hardcode
    vector<CellData<dim>> cells(n_cells, CellData<dim>());
    for (unsigned int i = 0; i < n_cells; ++i)
    {
        //for (unsigned int j = 0;
        //     j < GeometryInfo<dim>::vertices_per_cell;
        //     ++j)
        //
        //    cells[i].vertices[j] = mesh.triangles[4 * i + j];
        cells[i].vertices[0] = mesh.triangles[4 * i + 0] - 1;
        cells[i].vertices[1] = mesh.triangles[4 * i + 1] - 1;
        cells[i].vertices[2] = mesh.triangles[4 * i + 3] - 1;
        cells[i].vertices[3] = mesh.triangles[4 * i + 2] - 1;
        cells[i].material_id = 0;
    }
    
    triangulation.create_triangulation(vertices,
                                       cells,
                                       SubCellData());

}

void Simulation::make_custom_grid()
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

    static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell] 
        = {{0, 1, 2, 3}};

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

void Simulation::setup_system()
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

void Simulation::assemble_system()
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

void Simulation::solve()
{
    SolverControl solver_control(1000, 1e-12);
    SolverCG<> solver(solver_control);
    solver.solve(system_matrix, solution, system_rhs,
                 PreconditionIdentity());
}

void Simulation::output_results(string fileName) const
{
    DataOut<2> data_out;
    data_out.attach_dof_handler(dof_handler);
    data_out.add_data_vector(solution, "solution");
    data_out.build_patches();
    std::ofstream output(fileName);
    data_out.write_vtk(output);
}

void Simulation::run()
{
    setup_system();
    assemble_system();
    solve();
    output_results();
}


} //end of River namespace























































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

