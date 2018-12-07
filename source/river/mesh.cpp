#include "mesh.hpp"

namespace River{


/*
    Mesh class
    it encorporates different mesh generation tools
    and provides one interface

    

*/

struct vecTriangulateIO MeshGenerator::toVectorStr(struct triangulateio &geom, bool b3D)
{
  struct vecTriangulateIO geoOut;
  int i, dim = 2;
  double z0 = 0.;
  if(b3D) dim = 3;
  //Triangle generates two dimensional data, but almost all packages need three dimmnesion
  //so we have such possibility
  //reserve memory for vector containers
  
  geoOut.points.reserve(dim * geom.numberofpoints);
  geoOut.pointAttributes.reserve(geom.numberofpoints * geom.numberofpointattributes);
  geoOut.pointMarkers.reserve(geom.numberofpoints);
  geoOut.numOfAttrPerPoint = geom.numberofpointattributes;

  geoOut.segments.reserve(2 * geom.numberofsegments);
  geoOut.segmentMarkers.reserve(geom.numberofsegments);

  geoOut.triangles.reserve(3 * geom.numberoftriangles);
  geoOut.triangleAreas.reserve(geom.numberoftriangles);
  geoOut.triangleAttributes.reserve(geom.numberoftriangles * geom.numberoftriangleattributes);
  geoOut.numOfAttrPerTriangle = geom.numberoftriangleattributes;

  geoOut.holes.reserve(dim * geom.numberofholes);

  geoOut.regions.reserve(dim * geom.numberofregions);
  geoOut.numOfRegions = geom.numberofregions;

  geoOut.edges.reserve(dim * geom.numberofedges);
  geoOut.edgeMarkers.reserve(geom.numberofedges);

  geoOut.neighbors.reserve(geom.numberoftriangles * 3);
  
  for(i = 0; i < 2 * geom.numberofpoints; ++i){
    geoOut.points.push_back(geom.pointlist[i]);
    if(b3D && i % 2 != 0)
        geoOut.points.push_back(z0);
    if(i < geom.numberofpoints)
        geoOut.pointMarkers.push_back(geom.pointmarkerlist[i]);
  }

  
  for(i = 0; i < geom.numberofpoints * geom.numberofpointattributes; ++i)
    geoOut.pointAttributes.push_back(geom.pointattributelist[i]);

  
  for(i = 0; i < 2 * geom.numberofsegments; ++i){
    geoOut.segments.push_back(geom.segmentlist[i]);
    if (i < geom.numberofsegments)
        geoOut.segmentMarkers.push_back(geom.segmentmarkerlist[i]);
  }

  
  for(i = 0; i < 3*geom.numberoftriangles; ++i){
    geoOut.triangles.push_back(geom.trianglelist[i]);
    if(geom.trianglearealist != NULL && i < geom.numberoftriangles)
        geoOut.triangleAreas.push_back(geom.trianglearealist[i]);
  }

  
  for(i = 0; i < geom.numberoftriangles * geom.numberoftriangleattributes; ++i)
    geoOut.triangleAttributes.push_back(geom.triangleattributelist[i]);

  
  for(i = 0; i < dim * geom.numberofholes; ++i){
    geoOut.holes.push_back(geom.holelist[i]);
    if(b3D && i % 2 != 0)
        geoOut.holes.push_back(z0);
  }

  
  for(i = 0; i < dim * geom.numberofregions; ++i){
    geoOut.regions.push_back(geom.regionlist[i]);
    if(b3D && i % 2 != 0)
        geoOut.regions.push_back(z0);
  }

  
  for(i = 0; i < 2 * geom.numberofedges; ++i){
    geoOut.edges.push_back(geom.edgelist[i]);
    if (i < geom.numberofedges)
        geoOut.edgeMarkers.push_back(geom.edgemarkerlist[i]);
  }
  return geoOut;
}


struct triangulateio MeshGenerator::toTriaStr(struct vecTriangulateIO &geom)
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
    //if (SecondOrderMesh)
    //  triaStr.numberoftriangles = geom.triangles.size()/6;
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

void MeshGenerator::PrintGeometry(struct triangulateio &io)
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
    //if (StartNumberingFromZero)
    //    options += "z";
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
    if (Algorithm == FORTUNE)
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
    //if (SecondOrderMesh)
    //    options += "o2";
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
    //if (StartNumberingFromZero)
    //    cout << "(z) numbering starts from zero" << endl;
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
    if (Quite)
        cout << "(Q) quite" << endl;
    if (Verbose)
        cout << "(V) verbose" << endl;
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


struct vecTriangulateIO Triangle::Generate(struct vecTriangulateIO &geom)
{
    SetAllValuesToDefault();
    
    in = toTriaStr(geom);

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
    
    struct vecTriangulateIO OutputMesh = toVectorStr(out);

    FreeAllocatedMemory();

    return OutputMesh;
}






/*
    Tethex

*/
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

void Gmsh::Open()
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

void Gmsh::generate(vector<Point> points)
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

} //end of River namespace