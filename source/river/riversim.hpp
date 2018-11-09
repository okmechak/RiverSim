#pragma once

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string.h>

#include <tetgen.h>
#include <gmsh.h>
#include "triangle.h"
#include "tethex.hpp"

using namespace dealii;
using namespace std;
using namespace gmsh;

namespace po = boost::program_options;
namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;

namespace Mesh
{
  class Point
  {
    public:
      int index;
      struct
      {
          double x;
          double y;
      } coord;
      //physical attributes
      vector<double> phys_attributes;
      int marker;
      bool is_on_edge;
      vector<double> data;
  };

  class Options
  {
    private:
      string triaOpt;
      //NOTE: not implemented yet
      string LibMesh;
      string GmshOpt;
      string TetGenOpt;

      //z - numbering starts from zero
      bool StartNumberingFromZero = true;
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
      bool SuppressPolyFile = true;
      //N - suppress node file
      bool SuppressNodeFile = true;
      //E - suppress ele file
      bool SuppressEleFile = true;
      //e - outputs list of edges
      bool OutputEdges = true;
      //n - outputs neighbours
      bool ComputeNeighbours = true;
      //I - suppress file mesh iteration number
      bool SuppressMehsFileNumbering = true;
      //O - suppress holes
      bool SuppressHoles = false;
      //X - suppress exact arithmetics
      bool SuppressExactArithmetics = false;
      //o2 - generates second order mesh
      bool SecondOrderMesh = false;
      //Y - prohibits stainer points on mesh boundary
      bool SteinerPointsOnBoundary = false;
      //YY -                  ------ on any segment
      bool SteinerPointsOnSegments = false;
      //S - specify max num of added steiner points
      int MaxNumOfSteinerPoints = -1;

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
      double AngleQuality = -1.;
      //a - maximum triangle area constrain. a0.1
      double MaxTriaArea = -1.;
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
      //i - use incremental algorithm instead of divide and conqure
      //F - fortune algorithm instead of delauna
      algorithm Algorithm = CONQUER;
      //Q - quite
      bool Quite = false;
      //V - verbose
      bool Verbose = false;

      void Print(bool qDetailedDescription = true);

      string getTriangleOptions();

      void getTetgenOptions();
      void getGmshOptions();
      void getLibMeshOptions();
  };

  class Mesh
  {
    public:
      int dim = 2;
      //pointlist: array of point coordinates, each point ocupies two reals
      //pointattributelist: array of point attributes
      //    each point occupies 'numberofpointattributes' REALs
      //pointmarkerlist: array of point markers; one int per point
      //trianglelist: array of triangle corner list
      //triangleattributelist
      //trianglearealist - area constrains
      //neighborlist
      //segmentlist - array of segment endpoints, two per segment
      //segmentmarkerlist - array of segment markers, one int per segment
      //holelist - array of holes
      //regionlist - array regional attributes and area constrains
      //edgelist - output only
      //edgemarkerlist - output
      ////permanent Triangle options

      vector<double> pointList;
      int numOfPointAttributes = 0;
      vector<double> pointAttributes;
      vector<int> pointMarkesList;

      vector<int> segmentList;
      vector<int> segmentMarkerList;

      vector<int> triangleList;
      int numOfTriangleAttributes = 0;
      vector<double> triangleAttributeList;
      vector<double> triangleAreaList;
      vector<int> neighborList;

      //array of coordiantes
      vector<double> holeList;

      //array of array of coordinates
      vector<vector<double>> regionList;

      //out only
      vector<double> edgeList;
      vector<double> edgeMarkerList;

      Mesh();
      ~Mesh();
      void SetGeometryDescription();
      void SetMeshOptions(Options &meshOpt);
      void TriangleGenerator();
      void Print(
          struct triangulateio &io);

    private:
      Options meshOpt;
  };
} // namespace Mesh



class RiverSim
{
  public:
    RiverSim(po::variables_map &vm);
    ~RiverSim();
    void run();

  private:
    void geo_mesh_generator();
    void gmsh_mesh_generator();
    void mesh_covertor();
    void make_grid();
    void make_custom_grid();
    void setup_system();
    void assemble_system();
    void solve();
    void output_results() const;

    const static int dim = 2;

    Triangulation<dim> triangulation;
    FE_Q<dim> fe;
    DoFHandler<dim> dof_handler;
    SparsityPattern sparsity_pattern;
    SparseMatrix<double> system_matrix;
    Vector<double> solution;
    Vector<double> system_rhs;

    //options fro command line
    po::variables_map option_map;
};



void gmsh_possibilites(int argc, char *argv[]);