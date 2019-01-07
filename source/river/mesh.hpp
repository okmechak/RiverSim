#pragma once

#include <iostream>
#include <numeric>//iota
#include <iterator>//ostream_iterator

//mesh generation tools
#include <gmsh.h>
#include <tetgen.h>
#include "triangle.h"
//mesh convertor tool
#include "tethex.hpp"

#include "geometry.hpp"

using namespace std;

namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;


namespace River
{

  
 //class Tethex
 //{
 //  public:
 //     bool Verbose = false;
 //     Tethex();
 //     ~Tethex();
 //     void Convert(struct vecTriangulateIO &);
 //};



  class Triangle: public tethex::Mesh
  {
    private:
      int dim = 2;
      string options;

      struct triangulateio in, out, vorout;
      //z - numbering starts from zero
      //bool StartNumberingFromZero = false; - commented out
      //p - reads PSLG..
      //    1) will generate Constrained Delaunay.
      //    2) if are used -s(theoretical interest),
      //        -q(angles greter than 20, quality q23), -a maximum triangle area(a0.1) or
      //        -u.. will generate
      //            conforming constrained Delaunay
      //    3) if -D - conforming Delaunay triangulation(every triangle is Delaunay)
      //    4) if -r(refine) then segments of prev coarser mesh will be preserved
      bool ReadPSLG = true;
      //B - suppress boundary markers in output file
      bool SuppressBoundaryMarkers = false;
      //P - suppress output poly file
      bool SuppressPolyFile = false;
      //N - suppress node file
      bool SuppressNodeFile = false;
      //E - suppress ele file
      bool SuppressEleFile = false;
      //e - outputs list of edges
      bool OutputEdges = false;
      //n - outputs neighbours
      bool ComputeNeighbours = true;
      //I - suppress file mesh iteration number
      bool SuppressMehsFileNumbering = false;
      //O - suppress holes
      bool SuppressHoles = false;
      //X - suppress exact arithmetics
      bool SuppressExactArithmetics = false;
      //Y - prohibits stainer points on mesh boundary
      bool SteinerPointsOnBoundary = false;
      //YY -                  ------ on any segment
      bool SteinerPointsOnSegments = false;
      //S - specify max num of added steiner points
      int MaxNumOfSteinerPoints = -1;
      //v - outputs voronoi diagram
      bool VoronoiDiagram = false;
      //o2 - generates second order mesh
      //bool SecondOrderMesh = false; - not implemented
      

      void SetAllValuesToDefault();
      void FreeAllocatedMemory();

      string updateOptions();

      void PrintOptions(bool qDetailedDescription = true);
      
      void SetGeometry(struct triangulateio &geom);
      void SetGeometry(struct vecTriangulateIO &geom);
      struct triangulateio* GetGeometry();
      struct triangulateio* GetVoronoi();

      struct triangulateio tethexToIO(
        vector<tethex::Point> verticesVal,  
        vector<tethex::MeshElement *> linesVal,
        vector<tethex::MeshElement *> trianglesVal);

      tuple<vector<tethex::Point>,
      vector<tethex::MeshElement*>,
      vector<tethex::MeshElement*>>  
      IOToTethex(
        struct triangulateio &io);

      void PrintGeometry(struct vecTriangulateIO &geom);
      
      struct vecTriangulateIO toVectorStr(struct triangulateio &geom, bool b3D = true);
      struct triangulateio toTriaStr(struct vecTriangulateIO &geom);
      
    public:
      void PrintGeometry(struct triangulateio &io);
      enum algorithm
      {
          CONQUER,
          FORTUNE,
          ITERATOR
      };
      //not used options -u(user defined constraint), -I, -J(garbage triangles), h - help

      /*
           Triangle specific option list
      */

      //r - refine previously generated mesh, with preserving of segments
      bool Refine = false;
      //q - quality(minimu 20 degree).. also angle can be specified by q30
      bool ConstrainAngle = false;
      double MinAngle = 20;
      //a - maximum triangle area constrain. a0.1
      double MaxTriaArea = -1.;
      bool AreaConstrain = false;
      //D - all triangles will be delaunay. Not just constrained delaunay.
      bool DelaunayTriangles = true;
      //c - enclose convex hull with segments
      bool EncloseConvexHull = false;
      //C - check final mesh if it was conf with X
      bool CheckFinalMesh = false;
      //A - asign additional regional attribute to each triangle, and specifies it to
      //    each closed segment region it belongs. (has effect with -p and without -r)
      bool AssignRegionalAttributes = true;
      //i - use incremental algorithm instead of divide and conqure
      //F - fortune algorithm instead of delauna
      algorithm Algorithm = CONQUER;
      //Q - quite
      bool Quite = false;
      //V - verbose
      bool Verbose = false;

      Triangle();
      Triangle(
        tethex::Mesh &initMesh
      );
      ~Triangle();
      struct vecTriangulateIO Generate(struct vecTriangulateIO &geom);
      void Generate();

  };

  class TetGen
  {
    public:
    private:
  };


  class Gmsh
  {
    public:
      //some flags
      bool recombine = true;
      bool mesh24format = true;
      bool Verbose = true;
      //GMSH
      
      Gmsh();
      ~Gmsh();
      void Open(string fileName);
      void Write();
      void Clear();
      //MESH
      void refine();
      void setNodes(vector<double> nodes, int dim = 2, int tag = 1);//<- implement first
      void setElements(vector<int> elements, int elType = 2, int dim = 2, int tag = 1);
      void generate(vector<GeomPoint> points);
      void getElements();//<- implement first
      void TestMesh(struct vecTriangulateIO &geom);
      void StartUserInterface();

    private:
      string modelName = "basic";
      string fileName = "river.msh";
      const int dim = 1;

      vector<int> evaluateTags(int size, int tag0)
      {
        auto tags = vector<int>(size);
        iota(begin(tags), end(tags), tag0);
        return tags;
      }
  };

} //namespace mesh
