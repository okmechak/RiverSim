#include "mesh.hpp"

namespace River{


/*
    Mesh class
    it encorporates different mesh generation tools
    and provides one interface

    

*/
void Triangle::print_geometry(struct triangulateio &io) const
{
    int i, j, shift = 1;
    //if(StartNumberingFromZero)
    //    shift = 0;

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


struct triangulateio Triangle::tethex_to_io(tethex::Mesh &mesh) const
{
    struct triangulateio io;
    set_tria_to_default(&io);

    //Points
    if(!mesh.vertices.empty())
    {
        io.pointlist = new double[2 * mesh.vertices.size()];
        io.pointmarkerlist = new int[mesh.vertices.size()];
        io.numberofpoints = mesh.vertices.size();
        
        int i = 0;
        for(auto &p: mesh.vertices)
        {
            io.pointlist[2 * i ] = p.get_coord(0);
            io.pointlist[2 * i + 1] = p.get_coord(1);
            io.pointmarkerlist[i] = p.regionTag;
            ++i;
        }
    }

    //Segments
    if(!mesh.lines.empty())
    {
        io.segmentlist = new int[2 * mesh.lines.size()];
        io.segmentmarkerlist = new int[mesh.lines.size()];
        io.numberofsegments = mesh.lines.size();
        int i = 0;
        for(auto l: mesh.lines)
        {
            io.segmentlist[2 * i ] = l->get_vertex(0) + 1;//HOTFIX FIXME
            io.segmentlist[2 * i + 1] = l->get_vertex(1) + 1;//HOTFIX FIXME
            io.segmentmarkerlist[i] = l->get_material_id();
            ++i;
        }
    }

    if(!mesh.triangles.empty())
    {
        io.trianglelist = new int[3 * mesh.lines.size()];
        io.numberoftriangleattributes = 1;
        io.triangleattributelist = new double[mesh.lines.size()];
        io.numberofsegments = mesh.lines.size();
        int i = 0;
        for(auto t: mesh.triangles)
        {
            io.trianglelist[3 * i ] = t->get_vertex(0);
            io.trianglelist[3 * i + 1] = t->get_vertex(1);
            io.trianglelist[3 * i + 2] = t->get_vertex(2);
            io.triangleattributelist[i] = t->get_material_id();
            ++i;
        }
    }

    return io;
}

    void  Triangle::io_to_tethex(
        struct triangulateio &io, tethex::Mesh &initMesh)
{
    vector<tethex::Point> pointsVal;
    vector<tethex::MeshElement*> segmentsVal;
    vector<tethex::MeshElement*> trianglesVal;

    pointsVal.reserve(io.numberofpoints);
    for(int i = 0; i < io.numberofpoints; ++i)
    {
        auto x = io.pointlist[2 * i],
            y = io.pointlist[2 * i + 1];
        auto regionTag = io.pointmarkerlist[i];

        pointsVal.push_back(tethex::Point(x, y, 0/*z-component*/, regionTag, 1./*default mesh size used in gmsh*/));
    }

    segmentsVal.reserve(io.numberofsegments);
    for(int i = 0; i < io.numberofsegments; ++i)
    {
        auto v1 = io.segmentlist[2*i] - 1,//NOTE vertices index should start from zero
            v2 = io.segmentlist[2*i + 1] - 1,
            regionTag = io.segmentmarkerlist[i];
        if(regionTag == 0)//NOTE boundary_id of inner cells in Deal.II should be -1
            regionTag = -1;
        segmentsVal.push_back(new tethex::Line(v1, v2, regionTag));
    }

    trianglesVal.reserve(io.numberoftriangles);
    for(int i = 0; i < io.numberoftriangles; ++i)
    {
        auto v1 = io.trianglelist[3*i] - 1,//NOTE vertice index should start from zero
            v2 = io.trianglelist[3*i + 1] - 1,
            v3 = io.trianglelist[3*i + 2] - 1;   
        
        //taking only the first one attribute
        double regionTag = 0;
        if(io.numberoftriangleattributes != 0)
            regionTag = io.triangleattributelist[io.numberoftriangleattributes * i];

        trianglesVal.push_back(new tethex::Triangle(v1, v2, v3, regionTag));
    }

    initMesh.set_vertexes(pointsVal);
    initMesh.set_lines(segmentsVal);
    initMesh.set_triangles(trianglesVal);
}

/*
  
    Triangle Class
  
*/
Triangle::Triangle()
{
    set_all_values_to_default();
    
    if (Verbose){
        cout << "Default Options: " << endl;
        print_options(true);
    }
}

Triangle::~Triangle()
{   
    //we don't call this function
    //free_allocated_memory();
    //cos it is already present at the of generate() function
}


void Triangle::set_all_values_to_default()
{
    update_options();
    set_tria_to_default(&in);
    set_tria_to_default(&out);
    set_tria_to_default(&vorout);
}


void Triangle::free_allocated_memory()
{

    triangulateiofree(&in);
    triangulateiofree(&out);
    triangulateiofree(&vorout);
}


string Triangle::update_options()
{
    options = "";
    #define MAX_ANGLE 36
    if (ConstrainAngle)     options += "q";
    if (ConstrainAngle && MinAngle > 0 && MinAngle < MAX_ANGLE)
                            options += to_string(MinAngle);
    else if (ConstrainAngle && MinAngle > MAX_ANGLE)
        throw invalid_argument("Triangle quality angle should be equal or less then 36");
    //if (StartNumberingFromZero)
    //    options += "z";
    if (Refine)             options += "r";
    if (AreaConstrain)      options += "a";
    if (AreaConstrain && MaxTriaArea > 0)
                            options +=  to_string(MaxTriaArea);
    if (CustomConstraint)   options += "u";
    if (DelaunayTriangles)  options += "D";
    if (EncloseConvexHull)  options += "c";
    if (CheckFinalMesh)     options += "C";
    if (AssignRegionalAttributes) options += "A";
    if (VoronoiDiagram)     options += "v";
    if (Quite)              options += "Q";
    else if (Verbose)       options += "V";
    if (Algorithm == FORTUNE)
                            options += "F";
    else if (Algorithm == ITERATOR)
                            options += "i";
    if (ReadPSLG)           options += "p";
    if (SuppressBoundaryMarkers)
                            options += "B";
    if (SuppressPolyFile)   options += "P";
    if (SuppressNodeFile)   options += "N";
    if (SuppressEleFile)    options += "E";
    if (OutputEdges)        options += "e";
    if (ComputeNeighbours)  options += "n";
    if (SuppressMehsFileNumbering)
                            options += "I";
    if (SuppressExactArithmetics)
                            options += "X";
    if (SuppressHoles)      options += "O";
    //if (SecondOrderMesh)
    //    options += "o2";
    if (SteinerPointsOnBoundary) options += "Y";
    if (SteinerPointsOnSegments) options += "YY";
    if (MaxNumOfSteinerPoints > 0)
                            options += "S" + to_string(MaxNumOfSteinerPoints);

    return options;
}


void Triangle::print_options(bool qDetailedDescription)
{
    update_options();

    cout << "Triangle options command : " << options << endl;
    
    if(!qDetailedDescription) return;

    cout << "Detailed description: " << endl << endl; 

    if (ReadPSLG)        cout << "(p) read PSLG" << endl;
    //if (StartNumberingFromZero)
    //    cout << "(z) numbering starts from zero" << endl;
    if (Refine)          cout << "(r) refine" << endl;
    if (ConstrainAngle)  cout << "(q) quality min 20 degree" << endl;
    if (MinAngle > 0)    cout << "                    value:" << MinAngle << endl;
    if(AreaConstrain)    cout << "(a) area constrain: " << endl;
    if (MaxTriaArea > 0) cout << "                  area constrain: " << MaxTriaArea << endl;
    if (CustomConstraint) cout << "(u) imposes a user-defined constraint on triangle size" << endl;
    if (DelaunayTriangles)cout << "(D) all traingles will be Delaunay" << endl;
    if (EncloseConvexHull)cout << "(c) enclose convex hull" << endl;
    if (CheckFinalMesh)  cout << "(C) check final mesh" << endl;
    if (AssignRegionalAttributes)
                         cout << "(A) assign additional attribute to each triangle which specifies segment which it belongs too" << endl;
    if (OutputEdges)     cout << "(e) output list of edges" << endl;
    if (VoronoiDiagram)  cout << "(v) outputs voronoi diagram" << endl;
    if (ComputeNeighbours)cout << "(n) outputs neighboors" << endl;
    if (SuppressBoundaryMarkers)cout << "(B) suppress boundary markers" << endl;
    if (SuppressPolyFile)cout << "(P) suppress output poly file(either don't work)" << endl;
    if (SuppressNodeFile)cout << "(N) suppress output nodes file(either don't work)" << endl;
    if (SuppressEleFile) cout << "(E) suppress output elements file(either don't work)" << endl;
    if (SuppressHoles)   cout << "(O) suppress holes" << endl;
    //if (SecondOrderMesh)
    //    cout << "(o2) second order mesh" << endl;
    if (SteinerPointsOnBoundary || SteinerPointsOnSegments)
                        cout << "(Y) prohibits stainer points on boundary" << endl;
    if (MaxNumOfSteinerPoints > 0)
                        cout << "(S) specify max number off added Steiner points" << endl;
    if (Algorithm == ITERATOR)
                        cout << "(i) use incremental algorithm" << endl;
    if (Algorithm == FORTUNE)
                        cout << "(F) use Fortune algorithm" << endl;
    if (Quite)          cout << "(Q) quite" << endl;
    if (Verbose)        cout << "(V) verbose" << endl;
}



void Triangle::set_geometry(struct triangulateio &geom)//TODO check passing by references
{
    in = geom;
}


struct triangulateio* Triangle::get_geometry()
{
    return &out;
}




struct triangulateio* Triangle::get_voronoi()
{
    return &vorout;
}



void Triangle::generate(tethex::Mesh &initMesh)
{
    set_all_values_to_default();

    in = tethex_to_io(initMesh);

    if (Verbose)
    {
        cout << "Input Geometry: " << endl;
        print_geometry(in);
    }

    //Main call to Triangle
    triangulate(options.c_str(), &in, &out, &vorout);
    
    if (Verbose)
    {
        cout << "Output Geometry: " << endl;
        print_geometry(out);
    }

    if (Verbose && VoronoiDiagram)
    {
        cout << "Voronoi Diagram: " << endl;
        print_geometry(vorout);
    }

    io_to_tethex(out, initMesh);
    free_allocated_memory();
}






/*
    GMSH

*/
Gmsh::Gmsh()
{
  gmsh::initialize();
  init();
  if(recombine)
    gmsh::option::setNumber("Mesh.RecombineAll", (int)recombine);
  //gmsh::option::setNumber("Mesh.Optimize", 1);
  //gmsh::option::setNumber("Mesh.OptimizeThreshold", 0.31);
  //gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
  if(mesh24format)
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.0);
  if(Verbose)
    gmsh::option::setNumber("General.Terminal", 1);

  gmsh::model::add(modelName);
}

Gmsh::~Gmsh()
{
    gmsh::finalize();
}

void Gmsh::open(string fileName)
{
    gmsh::open(fileName);
}

void Gmsh::write(string fileName)
{
    gmsh::write(fileName);
}

void Gmsh::set_nodes(vector<double> nodes, int dim, int tag)
{
    auto tags = evaluate_tags(nodes.size()/3, 1);
    
    gmsh::model::mesh::setNodes(
        dim, 
        tag, 
        tags,
        nodes);
}

void Gmsh::set_elements(vector<int> elements, int elType, int dim, int tag)
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

    tags[0] = evaluate_tags(elements.size()/div, 1);

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

void Gmsh::generate(vector<GeomPoint> points)
{
    for(auto &p: points)
        geo::addPoint(
            p.x, p.y, 0, 0.01);//FIXME: very precise parameter

    for(unsigned int i = 1; i < points.size(); ++i)
        geo::addLine(i, i + 1);
    
    //closing circle
    geo::addLine(points.size(), 1);

    auto curveTag = geo::addCurveLoop(evaluate_tags(points.size(), 1));
    geo::addPlaneSurface({curveTag});

    geo::synchronize();
    //mdl::mesh::generate(2);
    //mdl::mesh::setRecombine(2, 1);
    mdl::mesh::generate(2);
    
}

void Gmsh::set_geometry(tethex::Mesh &meshio)
{
    int i = 1;
    for(auto &p: meshio.vertices)
    {
        geo::addPoint(
            p.get_coord(0), p.get_coord(1), p.get_coord(3), p.meshSize, i++);
    }

    i = 1;
    for(auto &l: meshio.lines)
    //TODO: create per each material_id separate group!
        geo::addLine(l->get_vertex(0)+1, l->get_vertex(1)+1, i++);


    auto lineTags = evaluate_tags(meshio.lines.size(), 1);
    auto curveTag = geo::addCurveLoop(lineTags);
    auto surfaceTag = geo::addPlaneSurface({curveTag});
    
    //TODO: test it! maybe i somehing forgot
    mdl::addPhysicalGroup(1, lineTags, 1);
    mdl::addPhysicalGroup(2, {surfaceTag}, 0);
    geo::synchronize();
}

void Gmsh::generate(tethex::Mesh &meshio)
{   
    //mdl::addDiscreteEntity(2, 1, lineTags);

    set_geometry(meshio);
    mdl::mesh::generate();
}

void Gmsh::clear()
{
    gmsh::clear();
}

void Gmsh::init()
{
    if(recombine)
        gmsh::option::setNumber("Mesh.RecombineAll", (int)recombine);
    //gmsh::option::setNumber("Mesh.Optimize", 1);
    //gmsh::option::setNumber("Mesh.OptimizeThreshold", 0.31);
    //gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    if(mesh24format)
        gmsh::option::setNumber("Mesh.MshFileVersion", 2.0);
    if(Verbose)
        gmsh::option::setNumber("General.Terminal", 1);

    gmsh::model::add(modelName);
}

void Gmsh::start_ui()
{
    gmsh::fltk::run();
}


void Gmsh::test_mesh()
{
    //gmsh::model::addDiscreteEntity(2, 0);
    gmsh::model::geo::addPoint(0, 0, 0, 0.2, 1);
    gmsh::model::geo::addPoint(0.5, 0, 0, 0.2, 2);
    gmsh::model::geo::addPoint(1, 0, 0, 0.2, 3);
    gmsh::model::geo::addPoint(1, 1, 0, 0.2, 4);
    gmsh::model::geo::addPoint(0, 1, 0, 0.2, 5);
    gmsh::model::geo::addPoint(0.5, 0.2, 0, 0.2, 6);
    
    gmsh::model::geo::addLine(1, 2, 1);
    gmsh::model::geo::addLine(2, 6, 2);
    gmsh::model::geo::addLine(6, 2, 3);
    gmsh::model::geo::addLine(2, 3, 4);
    gmsh::model::geo::addLine(3, 4, 5);
    gmsh::model::geo::addLine(4, 5, 6);
    gmsh::model::geo::addLine(5, 1, 7);
    auto curveTag = gmsh::model::geo::addCurveLoop({1, 2, 3, 4, 5, 6, 7});
    auto surfaceTag = gmsh::model::geo::addPlaneSurface({curveTag});
    gmsh::model::addPhysicalGroup(1, {1,2,3,4,5,6,7}, 1);
    //gmsh::model::addPhysicalGroup(1, {curveTag}, 1119);
    gmsh::model::addPhysicalGroup(2, {surfaceTag}, 1117);
    gmsh::model::geo::synchronize();
    gmsh::model::mesh::setRecombine(2, surfaceTag);
    gmsh::model::mesh::generate(2);
    gmsh::write("out_mesh.msh");

}

} //end of River namespace