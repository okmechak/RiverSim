#include "mesh.hpp"

namespace River{


/*
    Mesh class
    it encorporates different mesh generation tools
    and provides one interface

    

*/
void Triangle::PrintGeometry(struct triangulateio &io)
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


struct triangulateio Triangle::tethexToIO(tethex::Mesh &mesh)
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
            io.segmentlist[2 * i ] = l->get_vertex(0);
            io.segmentlist[2 * i + 1] = l->get_vertex(1);
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

    void  Triangle::IOToTethex(
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

        pointsVal.push_back(tethex::Point(x, y, 0/*z-component*/, regionTag));
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


void Triangle::PrintOptions(bool qDetailedDescription)
{
    updateOptions();

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



void Triangle::SetGeometry(struct triangulateio &geom)//TODO check passing by references
{
    in = geom;
}


struct triangulateio* Triangle::GetGeometry()
{
    return &out;
}




struct triangulateio* Triangle::GetVoronoi()
{
    return &vorout;
}



void Triangle::Generate(tethex::Mesh &initMesh)
{
    SetAllValuesToDefault();

    in = tethexToIO(initMesh);

    if (Verbose)
    {
        cout << "Input Geometry: " << endl;
        PrintGeometry(in);
    }

    //Main call to Triangle
    triangulate(options.c_str(), &in, &out, &vorout);
    
    if (Verbose)
    {
        cout << "Output Geometry: " << endl;
        PrintGeometry(out);
    }

    if (Verbose && VoronoiDiagram)
    {
        cout << "Voronoi Diagram: " << endl;
        PrintGeometry(vorout);
    }

    IOToTethex(out, initMesh);
    FreeAllocatedMemory();
}






/*
    Tethex

*/
//Tethex::Tethex()
//{}
//Tethex::~Tethex()
//{}
//
//void Tethex::Convert(struct vecTriangulateIO &geom)
//{
//    tethex::Mesh TethexMesh;
//    TethexMesh.read_triangl(
//        geom.node.coords, 
//        geom.edges, 
//        geom.edgeMarkers,  
//        geom.element.triangle.nodeTags);
//
//    if(Verbose)
//        TethexMesh.info(cout);
//    TethexMesh.convert();
//    if(Verbose)
//        TethexMesh.info(cout);
//
//    //TODO: write normal arguments    
//    TethexMesh.write_triangle(
//        geom.node.coords,
//        geom.element.line.nodeTags,
//        geom.element.line.markers,
//        geom.element.triangle.nodeTags);
//
//}







/*
    GMSH

*/
Gmsh::Gmsh()
{
  gmsh::initialize();
  gmsh::option::setNumber("Mesh.RecombineAll", (int)recombine);
  //gmsh::option::setNumber("Mesh.Optimize", 1);
  //gmsh::option::setNumber("Mesh.OptimizeThreshold", 0.31);
  //gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
  gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
  gmsh::option::setNumber("General.Terminal", 1);
  gmsh::model::add(modelName);
  //gmsh::model::addDiscreteEntity(2, 1);
}

Gmsh::~Gmsh()
{
    gmsh::finalize();
}

void Gmsh::Open(string fileName)
{
    gmsh::open(fileName);
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

void Gmsh::generate(vector<GeomPoint> points)
{
    for(auto &p: points)
        geo::addPoint(
            p.x, p.y, 0, 0.01);//FIXME: very precise parameter

    for(unsigned int i = 1; i < points.size(); ++i)
        geo::addLine(i, i + 1);
    
    //closing circle
    geo::addLine(points.size(), 1);

    auto curveTag = geo::addCurveLoop(evaluateTags(points.size(), 1));
    auto surfaceTag = geo::addPlaneSurface({curveTag});

    geo::synchronize();
    //mdl::mesh::generate(2);
    //mdl::mesh::setRecombine(2, 1);
    mdl::mesh::generate(2);
    
}


void Gmsh::StartUserInterface()
{
    gmsh::fltk::run();
}


void Gmsh::TestMesh(struct River::vecTriangulateIO &geom)
{
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
    gmsh::model::geo::addCurveLoop({1, 2, 3, 4, 5, 6, 7}, 1);
    gmsh::model::geo::addPlaneSurface({1}, 6);
    gmsh::model::geo::synchronize();
    gmsh::model::mesh::generate(2);

    {
        std::vector<int> nodeTags;
        std::vector<double> coord, parametricCoord;
        const int dim = -1, tag = -1;
        const bool includeBoundary = true;
        gmsh::model::mesh::getNodes(nodeTags, coord, parametricCoord, dim, tag, includeBoundary);

        geom.node.coords = coord;
        geom.node.tags = nodeTags;
    }
    
    {
        std::vector<int> elementTypes;
        std::vector<std::vector<int> > elementTags, nodeTags;
        const int dim = -1;
        const int tag = -1;
        gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, dim, tag);
        cout << "Element Types" << endl;
        copy(elementTypes.begin(), elementTypes.end(), ostream_iterator<int>(cout, " "));
    }
}

} //end of River namespace