/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

/*! \file mesh.hpp
    \brief Mesh generation functionality
*/
#pragma once

///mesh generation tools
#include "triangle.hpp"

///mesh convertor tool
#include "tethex.hpp"

///other dependencies
#include "physmodel.hpp"

using namespace std;


namespace River
{
  
    /*! \brief Traingle mesh geneartion tool wrapper.
        \details 
        For more details please see 
        [official web-site of Triangle(c) tool](https://www.cs.cmu.edu/~quake/triangle.html).
        \todo implement better interface between triangle and constrint area function.
        \see triangle.hpp
    */
    class Triangle
    {
        private:
            ///Dimmension.
            int dim = 2;
            ///Holds triangulation options as string.
            string options;

            ///Structures used as interface to mesh generation.
            struct triangulateio in, out, vorout;
            //z - numbering starts from zero
            //bool StartNumberingFromZero = false; - commented out
            /**
             * p - reads PSLG.
             *    1) will generate Constrained Delaunay.
             *    2) if are used -s(theoretical interest),
             *        -q(angles greter than 20, quality q23), -a maximum triangle area(a0.1) or
             *        -u.. will generate
             *            conforming constrained Delaunay
             *    3) if -D - conforming Delaunay triangulation(every triangle is Delaunay)
             *    4) if -r(refine) then segments of prev coarser mesh will be preserved
             */
            bool ReadPSLG = true;
            ///B - suppress boundary markers in output file
            bool SuppressBoundaryMarkers = false;
            ///P - suppress output poly file
            bool SuppressPolyFile = false;
            ///N - suppress node file
            bool SuppressNodeFile = false;
            ///E - suppress ele file
            bool SuppressEleFile = false;
            ///e - outputs list of edges
            bool OutputEdges = false;
            ///n - outputs neighbours
            bool ComputeNeighbours = true;
            ///I - suppress file mesh iteration number
            bool SuppressMehsFileNumbering = false;
            ///O - suppress holes
            bool SuppressHoles = false;
            ///X - suppress exact arithmetics
            bool SuppressExactArithmetics = false;
            ///Y - prohibits stainer points on mesh boundary
            bool SteinerPointsOnBoundary = false;
            ///YY -                  ------ on any segment
            bool SteinerPointsOnSegments = false;
            ///S - specify max num of added steiner points
            int MaxNumOfSteinerPoints = -1;
            ///v - outputs voronoi diagram
            bool VoronoiDiagram = false;
            ///o2 - generates second order mesh
            //bool SecondOrderMesh = false; - not implemented

            /**
             * Set the all interface variables(in, out, vorout) to default values.
             */
            void set_all_values_to_default();
            ///Freeups allocated memory for interface variables(in, out, vorout).
            void free_allocated_memory();

            ///Updates options bassing on interface flags.
            string update_options();


            ///Outupts options with or without detailed description.
            void print_options(bool qDetailedDescription = true);

            ///Get output geometry. e.g generated mesh.
            struct triangulateio* get_geometry();

            ///Get voronoi diagram, if such was generated.
            struct triangulateio* get_voronoi();

            ///Converts tethex::Mesh to \ref triangulateio used by Triangle.
            struct triangulateio tethex_to_io(const tethex::Mesh &initMesh) const;

            ///Converts \ref triangulateio to tethex::Mesh used by rest of program.
            void io_to_tethex(const struct triangulateio &io, tethex::Mesh &initMesh) const;
      
        public:
      
            ///Outputs to consloe geometry.
            void print_geometry(const struct triangulateio &io) const;
        
            ///Enumerating all algorithm stragies. 
            ///CONQUER is default and the fastest.
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
       
            ///r - refine previously generated mesh, with preserving of segments
            bool Refine = false;
            ///q - quality(minimu 20 degree).. also angle can be specified by q30
            bool ConstrainAngle = true;

            ///Minimal angle of mesh element.
            double MinAngle = 20;
            ///a - maximum triangle area constrain. a0.1
            double MaxTriaArea = -1.;

            ///If true, then mesh element will be constrined to some maxiaml value.
            bool AreaConstrain = false;
            ///User defined function constrint
            bool CustomConstraint = false;
            ///D - all triangles will be delaunay. Not just constrained delaunay.
            bool DelaunayTriangles = true;
            ///c - enclose convex hull with segments
            bool EncloseConvexHull = false;
            ///C - check final mesh if it was conf with X
            bool CheckFinalMesh = false;
            ///A - asign additional regional attribute to each triangle, and specifies it to
            ///    each closed segment region it belongs. (has effect with -p and without -r)
            bool AssignRegionalAttributes = true;
            ///i - use incremental algorithm instead of divide and conqure
            ///F - fortune algorithm instead of delauna
            algorithm Algorithm = CONQUER;
            ///Q - quite
            bool Quite = false;
            ///V - verbose
            bool Verbose = false;
        
            ///Mesh refinment object
            MeshParams* ref;
        
            Triangle();
            ~Triangle();
            
            /**
             * Generate mesh.
             * 
             * @param initMesh tethex::Mesh object which holds boundary constrains, holes etc. 
             */
            void generate(tethex::Mesh &initMesh);
  };
} //namespace mesh
