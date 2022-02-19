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

/*! \file boundary.hpp
    \brief Generate initial predefinide boundary geometry and its source positions.
    
    \details First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
    for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
    with any source position on this boundary.
    For additional information see \ref River::Boundary.
*/

#pragma once

///\cond
#include <vector>
#include <string>
#include <map>
///\endcond

#include "GeometryPrimitives.hpp"

using namespace std;

/*! \namespace River
    \brief River namespace holds everything that is related to River simulation.
*/
namespace River
{   
    ///Enumeration of different boundary conditions
    enum t_boundary 
    {
        DIRICHLET = 0, 
        NEUMAN
    }; 

    /*! \struct BoundaryCondition
        \brief Describes boudary condition type.
    */
    struct BoundaryCondition
    {
        t_boundary type = DIRICHLET;
        double value = 0;

        bool operator==(const BoundaryCondition& bc) const;
        friend ostream& operator <<(ostream& write, const BoundaryCondition & boundary_condition);
    };

    ///Vertice positionn index data type.
    typedef size_t t_vert_pos;
    ///Boundary id data type.
    typedef size_t t_boundary_id;
    ///Source id data type.
    typedef size_t t_source_id;
    
    ///Map structure of boundary condition types.
    typedef map<t_boundary_id, BoundaryCondition> t_BoundaryConditions;
    
    /*! \class BoundaryConditions
        \brief Map structure of boundary condition types.
        
        \details Holds for each id its appropraite boundary condition.
    */
    class BoundaryConditions: public t_BoundaryConditions
    {
        public:
            ///Returns map structure with boundary conditions of specific type(Neuman or Dirichlet).
            t_BoundaryConditions Get(t_boundary type) const;
            friend ostream& operator <<(ostream& write, const BoundaryConditions & bcs);
    };
    
    ///Source point coordinate data type. Pair which holds boundary id and vertice position on this id.
    typedef pair<t_boundary_id, t_vert_pos> t_source_coord;
    ///Vector of source point ids data type.
    typedef vector<t_source_id> t_sources_ids;
    ///Map structure which holds source id and its coordinates.
    typedef map<t_source_id, t_source_coord> t_Sources;
    ///Map structure which holds source id and its source point coordinates.
    class Sources: public t_Sources
    {
        public:
            ///Returns all sources ids.
            t_sources_ids GetSourcesIds() const;
    };

    /*! \brief 
        Line - holds indexes of __p1__, __p2__ vertices and id.
        Its structure is depended on Mesh data structure.
    */
    class Line
    {
        public:
            ///Creates line with zero points indexes and boundary condition.
            Line() = default;

            Line(const t_vert_pos p1v, const t_vert_pos p2v, const t_boundary_id boundary_id):
                p1(p1v), 
                p2(p2v), 
                boundary_id(boundary_id)
            {};

            Line(const Line& line) = default;

            ///First point index.
            t_vert_pos p1 = 0; 
            ///Second point index.
            t_vert_pos p2 = 0;

            ///Boundary condition id.
            t_boundary_id boundary_id = 0;

            friend ostream& operator <<(ostream& write, const Line & line);
            bool operator==(const Line& line) const;
    };

    ///Vector of points data type.
    typedef vector<Point> t_PointList;
    
    ///Vector of lines data type.
    typedef vector<Line> t_LineList;

    /*! \brief Structure which defines simple boundary data structure.
        
        \details Simple boundary means only one boundary.
        \imageSize{Boundary.jpg, height:40%;width:40%;, }
    */
    class Boundary
    {
        public:
            ///Vertices of boundary vector.
            t_PointList vertices;
            ///Connvections between boundaries.
            t_LineList lines;
            ///Is inner boundary. It has consequences on holes evaluation.
            bool inner_boundary = false;
            ///Array of holes. Which will be eliminated by mesh generator.
            t_PointList holes;
            ///Name with description of boundary.
            string name = "";
            ///Appends another simple boundary at the end of current boundary.
            void Append(const Boundary& simple_boundary);
            ///Replace on element of boundary with whole simple boundary structure.
            void ReplaceElement(t_vert_pos vertice_pos, const Boundary& simple_boundary);

            bool operator==(const Boundary& simple_boundary) const;
            friend ostream& operator <<(ostream& write, const Boundary & boundary);
    };

    ///
    typedef map<t_boundary_id, Boundary> t_Boundaries;
    typedef pair<Point, double> t_branch_source;
    /*! \brief Structure which defines Region of region.
        
        \details It is a combination of simple boundaries which of each has  it own unique id.
        \imageSize{Region.jpg, height:40%;width:40%;, }
    */
    class Region: public t_Boundaries
    {
        public:
            ///Data structure - interface between boundary sources and source points of \ref Rivers
            typedef map<t_source_id, t_branch_source> trees_interface_t;
            
            ///Constructor
            Region(t_Boundaries simple_boundaries = {});

            ///Initialize rectangular Region.
            Sources MakeRectangular(double width = 1, double height = 1, double source_x_position = 0.25);
            
            ///Initialize rectangular with hole Region.
            Sources MakeRectangularWithHole(double width = 1, double height = 1, double source_x_position = 0.25);

            ///Some basic checks of data structure.
            void Check();   

            ///Return outer simple boundary(There should be only one such).
            Boundary& GetOuterBoundary();

            ///Returns vector of all holes.
            t_PointList GetHolesList() const;

            ///Returns map of source points ids and coresponding source point and angle of tree \ref Branch.
            trees_interface_t GetSourcesIdsPointsAndAngles(const Sources& sources) const;

            friend ostream& operator <<(ostream& write, const Region & boundary);

        //private:fix me todo

            ///Returns normal angle based on three points.
            static double NormalAngle(const Point& left, const Point& center, const Point& right);

            ///Returns andjancent vertice indexes.
            static pair<t_vert_pos, t_vert_pos> GetAdjacentVerticesPositions(
                const t_vert_pos vertices_size, 
                const t_vert_pos vertice_pos);
            
            ///Returns normal angle at point with vertice_pos index.
            static double GetVerticeNormalAngle(    
                const t_PointList& vertices, 
                const t_vert_pos vertice_pos);
    };
}