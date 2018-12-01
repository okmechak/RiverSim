#include "riversim.hpp"

namespace River{
/*
    River Geometry Object

*/

Geometry::Geometry(
        double dl, double dx, 
        vector<double> coords): dl(dl), dx(dx)
{
    BottomBoxCorner = {coords[0], coords[1]};
    TopBoxCorner    = {coords[2], coords[3]};
    
    //Nodes
    nodes = {
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 9*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 8*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 7*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 6*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 5*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 4*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 3*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + 2*dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1] + dl, //node 1
        BottomBoxCorner[0] + (dx + eps), BottomBoxCorner[1],      //node 2
        TopBoxCorner[0],                 BottomBoxCorner[1],      //node 3
        TopBoxCorner[0],                 TopBoxCorner[1],         //node 4
        BottomBoxCorner[0],              TopBoxCorner[1],         //node 5
        BottomBoxCorner[0],              BottomBoxCorner[1],      //node 6
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1],      //node 7
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 2*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 3*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 4*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 5*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 6*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 7*dl, //node 8
        BottomBoxCorner[0] + dx,         BottomBoxCorner[1] + 8*dl, //node 8
    };

    nodeMarkers = {
        Markers::River,
        Markers::Bottom,
        Markers::Bottom,
        Markers::Right,
        Markers::Top,
        Markers::Left,
        Markers::Bottom,
        Markers::River,
    };

    //Segments
    circularSegments = {
        1, 2,   2, 3,   3, 4,   4, 5, 
        5, 6,   6, 7,   7, 8,   8, 9,   9,10,  10,11, 11,12, 12,13, 13,14, 14,15, 15,16, 16,17, 17,18,  18,19, 19,20, 20,21, 21,22, 22,23, 23,1
        };
    
    segmentMarkers = {
        Markers::River,
        Markers::Bottom,
        Markers::Right,
        Markers::Top,
        Markers::Left,
        Markers::Bottom,
        Markers::River,
        Markers::River,
    };
}

void Geometry::addNode(double x, double y, unsigned int marker)
{
    nodes.push_back(x);
    nodes.push_back(y);
    auto curNodeIndex = nodes.size() / 2;

    //insert left segment at the beginning of circular array
    circularSegments.insert(circularSegments.begin(), 
        circularSegments[0]);
    circularSegments.insert(circularSegments.begin(), 
        curNodeIndex);

    //insert rigth segment at the end of circular array
    circularSegments.push_back(circularSegments[0]);
    circularSegments.push_back(curNodeIndex);

}

void Geometry::addDNode(double dx, double dy, unsigned int marker)
{
    auto lastNodePosition = circularSegments[0] - 1;
    auto x = nodes[2 * lastNodePosition];
    auto y = nodes[2 * lastNodePosition + 1];

    nodes.push_back(x + dx);
    nodes.push_back(y + dy);
    auto curNodeIndex = nodes.size() / 2;
    nodes.push_back(x + (dx + eps));
    nodes.push_back(y + dy);
    auto curEpsNodeIndex = nodes.size() / 2;
    nodeMarkers.push_back(marker);

    //insert left segment at the beginning of circular array
    auto firstPoint = circularSegments[0];
    auto lastPoint = circularSegments.back();

    //TODO: add assertion.. first and last points should be the same

    circularSegments.insert(circularSegments.begin(), 
        firstPoint);
    circularSegments.insert(circularSegments.begin(), 
        curNodeIndex);
    

    //insert rigth segment at the end of circular array
    circularSegments.push_back(lastPoint);
    circularSegments.push_back(curEpsNodeIndex);
    
    //Markers
    segmentMarkers.insert(segmentMarkers.begin(), marker);
    segmentMarkers.push_back(marker);
}


Geometry::~Geometry(){}


/*
    vecTriangulateIO structure class

*/

vecTriangulateIO::vecTriangulateIO()
{
}

vecTriangulateIO::~vecTriangulateIO()
{
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

void Gmsh::generate(Geometry & geom)
{
    auto numOfNodes = geom.nodes.size() / 2;
    for(auto i = 0; i < numOfNodes; ++i)
        geo::addPoint(
            geom.nodes[2 * i], geom.nodes[2 * i + 1], 0, 0.01);//FIXME: very precise parameter

    
    auto numOfSegments = geom.circularSegments.size() / 2;
    for(auto i = 0; i < numOfSegments; ++i)
        geo::addLine(
            geom.circularSegments[2 * i], geom.circularSegments[2 * i + 1]);

    auto curveTag = geo::addCurveLoop(evaluateTags(numOfSegments, 1));
    auto surfaceTag = geo::addPlaneSurface({curveTag});
    cout << surfaceTag << endl;
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

        geom.points = coord;
        geom.pointTags = nodeTags;
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







/*

    RiverSim Class

*/

Simulation::Simulation(po::variables_map &vm) : fe(2), dof_handler(triangulation)
{
    option_map = vm;
}

Simulation::~Simulation()
{
    system_matrix.clear();
}

void Simulation::TryInsertCellBoundary(
    CellData<dim> &cellData,
    struct SubCellData &subcelldata, 
    std::unordered_map<std::pair<int, int>, int> &bound_ids, 
    int v1, int v2)
{
    CellData<1> boundary;
    pair<int, int> p;
    if(bound_ids.count(p = make_pair(v1, v2)) &&
        bound_ids[p] != 0){
        boundary.boundary_id = bound_ids[p];
        boundary.vertices[0] = v1;
        boundary.vertices[1] = v2;
        subcelldata.boundary_lines.push_back(boundary);
        cellData.manifold_id = boundary.boundary_id;
    }
    else if (bound_ids.count(p = make_pair(v2, v1)) &&
        bound_ids[p] != 0)
    {
        boundary.boundary_id = bound_ids[p];
        boundary.vertices[0] = v2;
        boundary.vertices[1] = v1;
        subcelldata.boundary_lines.push_back(boundary);
        cellData.manifold_id = boundary.boundary_id;
    }
}

void Simulation::OpenMesh(string fileName)
{
    GridIn<dim> gridin;
    gridin.attach_triangulation(triangulation);
    std::ifstream f(fileName);
    gridin.read_msh(f);
}

void Simulation::SetMesh(struct vecTriangulateIO & mesh)
{
    
    //VERTICES
    auto n_points = mesh.points.size() / 3; //FIXME: replace hardcoded 3 by more general thing
    vector<Point<dim>> vertices(n_points);
    for(unsigned int i = 0; i < n_points; ++i)
    {
        vertices[i] = Point<dim>(mesh.points[3 * i], mesh.points[3 * i + 1]);
    }
    
    //generat boundary id structure
    std::unordered_map<std::pair<int,int>, int> bound_id;
    for(unsigned int i = 0; i < mesh.segments.size()/2; ++i)
        bound_id.insert(make_pair(
            make_pair(mesh.segments[2 * i], mesh.segments[2*i + 1]), mesh.segmentMarkers[i]));

        


    //SETTING QUADRANGLES
    auto n_cells = mesh.triangles.size() / 4; //FIXME: remove hardcode
    vector<CellData<dim>> cells(n_cells, CellData<dim>());
    auto subCell = SubCellData();
    for (unsigned int i = 0; i < n_cells; ++i)
    {
        auto v1 = cells[i].vertices[0] = mesh.triangles[4 * i + 0] - 1;
        auto v2 = cells[i].vertices[1] = mesh.triangles[4 * i + 1] - 1;
        auto v4 = cells[i].vertices[2] = mesh.triangles[4 * i + 3] - 1;
        auto v3 = cells[i].vertices[3] = mesh.triangles[4 * i + 2] - 1;
        TryInsertCellBoundary(cells[i], subCell, bound_id, v1, v2);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v2, v3);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v3, v4);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v4, v1);
        
        //if(mesh.numOfAttrPerTriangle > 0)
        //    cells[i].material_id = mesh.triangleAttributes[i];
        //else 
        //    cells[i].material_id = 0;
    }
    cout << "checking consistency" << endl;
    if(subCell.check_consistency(dim))
        cout << "i don't know what it means" << endl;
    triangulation.create_triangulation(vertices,
                                       cells,
                                       subCell);
}


double Simulation::RightHandSide::value (const Point<dim> &/*p*/,
                                  const unsigned int /*component*/) const
{
  double return_value = 1.0;
  
  return return_value;
}

double Simulation::BoundaryValues::value (const Point<dim> &/*p*/,
                                   const unsigned int /*component*/) const
{
  return 0.;
}


void Simulation::setup_system()
{
    dof_handler.distribute_dofs(fe);

    solution.reinit(dof_handler.n_dofs());
    system_rhs.reinit(dof_handler.n_dofs());

    constraints.clear();
    DoFTools::make_hanging_node_constraints(
        dof_handler, constraints);

    VectorTools::interpolate_boundary_values(
        dof_handler, 0, ZeroFunction<dim>(), constraints);
    
    constraints.close();
    DynamicSparsityPattern dsp(dof_handler.n_dofs());
    DoFTools::make_sparsity_pattern(
        dof_handler, dsp, constraints, false);
    sparsity_pattern.copy_from(dsp);
    system_matrix.reinit(sparsity_pattern);
}


double coefficient (const Point<2> &p)
{
  //if (p.square() < 0.5*0.5)
  //  return 20;
  //else
    return 1;
}

void Simulation::assemble_system()
{
const QGauss<dim>  quadrature_formula(3);
  FEValues<dim> fe_values (fe, quadrature_formula,
                           update_values    |  update_gradients |
                           update_quadrature_points  |  update_JxW_values);
  const unsigned int   dofs_per_cell = fe.dofs_per_cell;
  const unsigned int   n_q_points    = quadrature_formula.size();
  FullMatrix<double>   cell_matrix (dofs_per_cell, dofs_per_cell);
  Vector<double>       cell_rhs (dofs_per_cell);
  std::vector<types::global_dof_index> local_dof_indices (dofs_per_cell);
  typename DoFHandler<dim>::active_cell_iterator
  cell = dof_handler.begin_active(),
  endc = dof_handler.end();
  for (; cell!=endc; ++cell)
    {
      cell_matrix = 0;
      cell_rhs = 0;
      fe_values.reinit (cell);
      for (unsigned int q_index=0; q_index<n_q_points; ++q_index)
        {
          const double current_coefficient = coefficient
                                             (fe_values.quadrature_point (q_index));
          for (unsigned int i=0; i<dofs_per_cell; ++i)
            {
              for (unsigned int j=0; j<dofs_per_cell; ++j)
                cell_matrix(i,j) += (current_coefficient *
                                     fe_values.shape_grad(i,q_index) *
                                     fe_values.shape_grad(j,q_index) *
                                     fe_values.JxW(q_index));
              cell_rhs(i) += (fe_values.shape_value(i,q_index) *
                              1.0 *
                              fe_values.JxW(q_index));
            }
        }
      cell->get_dof_indices (local_dof_indices);
      constraints.distribute_local_to_global (cell_matrix,
                                              cell_rhs,
                                              local_dof_indices,
                                              system_matrix,
                                              system_rhs);
    }
}

void Simulation::solve()
{
    SolverControl      solver_control (4000, 1e-12);
    SolverCG<>         solver (solver_control);
    PreconditionSSOR<> preconditioner;
    preconditioner.initialize(system_matrix, 1.2);
    solver.solve (system_matrix, solution, system_rhs,
                    preconditioner);
    constraints.distribute (solution);
}

void Simulation::refine_grid ()
{
    Vector<float> estimated_error_per_cell (triangulation.n_active_cells());
    KellyErrorEstimator<dim>::estimate (dof_handler,
                                      QGauss<dim-1>(3),
                                      typename FunctionMap<dim>::type(),
                                      solution,
                                      estimated_error_per_cell);
    GridRefinement::refine_and_coarsen_fixed_number (triangulation,
                                                   estimated_error_per_cell,
                                                   0.3, 0.03);
    triangulation.execute_coarsening_and_refinement ();
}

void Simulation::output_results (const unsigned int cycle) const
{
    DataOut<dim> data_out;
    data_out.attach_dof_handler (dof_handler);
    data_out.add_data_vector (solution, "solution");
    data_out.build_patches ();
    std::ofstream output ("solution-" + std::to_string(cycle) + ".vtk");
    data_out.write_vtk (output);
}

void Simulation::run()
{
for (unsigned int cycle=0; cycle < 3; ++cycle)
    {
      std::cout << "Cycle " << cycle << ':' << std::endl;
      if (cycle > 0)
        refine_grid ();
        
      std::cout << "   Number of active cells:       "
                << triangulation.n_active_cells()
                << std::endl;
      setup_system ();
      std::cout << "   Number of degrees of freedom: "
                << dof_handler.n_dofs()
                << std::endl;
      assemble_system ();
      solve ();
      output_results (cycle);
    }
}


} //end of River namespace