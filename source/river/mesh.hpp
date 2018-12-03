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

#include "common.hpp"

using namespace std;

namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;


namespace River
{
  
 class Tethex
 {
   private:
   public:
      bool Verbose = false;
      Tethex();
      ~Tethex();
      void Convert(struct vecTriangulateIO &);
 };



  class Triangle
  {
    private:
      int dim = 2;
      string options;

      struct triangulateio in, out, vorout;
      //z - numbering starts from zero
      bool StartNumberingFromZero = false;
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
      bool OutputEdges = true;
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


      

      void SetAllValuesToDefault();
      void FreeAllocatedMemory();

      string updateOptions();

      void PrintOptions(bool qDetailedDescription = true);
      void PrintGeometry(struct triangulateio &io);
      
      void SetGeometry(struct triangulateio);
      void SetGeometry(struct vecTriangulateIO &geom);
      struct triangulateio* GetGeometry();
      struct vecTriangulateIO toVectorStructure(struct triangulateio*, bool b3D = true);
      struct triangulateio toTriaStructure(struct vecTriangulateIO&);
      struct triangulateio* GetVoronoi();

    public:
      enum algorithm
      {
          CONQUER,
          FORUNE,
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
      double MaxAngle = 20;
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
      //v - outputs voronoi diagram
      bool VoronoiDiagram = false;
      //o2 - generates second order mesh
      bool SecondOrderMesh = false;
      //i - use incremental algorithm instead of divide and conqure
      //F - fortune algorithm instead of delauna
      algorithm Algorithm = CONQUER;
      //Q - quite
      bool Quite = false;
      //V - verbose
      bool Verbose = false;

      Triangle();
      ~Triangle();
      struct vecTriangulateIO Generate(struct vecTriangulateIO &geom);

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
      void Open();
      void Write();
      void Clear();
      //MESH
      void generate();
      void partition();
      void refine();
      void setOrder();
      void getNodes();
      void setNodes(vector<double> nodes, int dim = 2, int tag = 1);//<- implement first
      void setElements(vector<int> elements, int elType = 2, int dim = 2, int tag = 1);
      void generate(vector<Point> points);
      void getElements();//<- implement first
      void getJacobians();
      void TestMesh(struct vecTriangulateIO &geom);
      //... and lot of other
      //FIELD
      //GEO
      //GEOMESH
      //OCC
      //VIEW <-propably I will need this
      //PLUGIN
      //GRAPHICS
      //FLTK <- important one
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
