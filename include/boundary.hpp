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
#include <map>
///\endcond

#include "point.hpp"

namespace River
{   
    ///Enumeration of different boundary conditions
    enum t_boundary 
    {
        DIRICHLET = 0, 
        NEUMAN,
        ROBIN
    }; 

    /*! \struct BoundaryCondition
        \brief Describes boudary condition type.
    */
    struct BoundaryCondition
    {
        public:

            BoundaryCondition() = default;
            BoundaryCondition(t_boundary type, double value):
                type{type},
                value{value}
            {};

            t_boundary type = DIRICHLET;
            double value = 0.;

            bool operator==(const BoundaryCondition& bc) const;
            friend ostream& operator <<(ostream& write, const BoundaryCondition & boundary_condition);
    };

    ///Vertice positionn index data type, boundary id data type and source id data type.
    typedef size_t t_vert_pos; 
    typedef size_t t_boundary_id; 
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
            t_BoundaryConditions operator()(t_boundary type) const;
            friend ostream& operator <<(ostream& write, const BoundaryConditions & bcs);
    };
    
    ///Source point coordinate data type. Pair which holds boundary id and vertice position on this id.
    ///boundary_id is particular boundary where source is located.
    ///and t_vert_pos is vetice indec position in point list.
    typedef pair<t_boundary_id, t_vert_pos> t_source_coord;
    ostream& operator <<(ostream& write, const t_source_coord & source_coord);
    
    ///Vector of source point ids data type.
    typedef vector<t_source_id> t_sources_ids;

    ///Map structure which holds source id and its coordinates.
    typedef map<t_source_id, t_source_coord> t_Sources;

    ///Map structure which holds source id and its source point coordinates.
    class Sources: public t_Sources
    {
        public:
            ///Returns all sources ids.
            t_sources_ids getSourcesIds() const;
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

    ///Vector of lines data type.
    typedef vector<Line> t_LineList;

    class Triangle
    {

    };
    
    

    /*! \brief Structure which defines simple boundary data structure.
        
        \details Simple boundary means only one boundary.
        \imageSize{SimpleBoundary.jpg, height:40%;width:40%;, }
    */
    class Boundary
    {
        public:

            //fields

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

            //modifiers

            ///Appends another simple boundary at the end of current boundary.
            void append(const Boundary& simple_boundary);
            ///Appends vertice with cartesian at the end of current boundary and connect it with line with specified boundary id.
            void append(const Point& p, const t_boundary_id boundary_id);
            ///Appends vertice with polar coords at the end of current boundary and connect it with line with specified boundary id.
            void append(const Polar& p, const t_boundary_id boundary_id);
            /*! \brief Reduces lenght of branch by \p lenght.
                \note
                If \p lenght is greater than full lenght of branch, then __source_point__ only remains.
                \throw Exception.
            */

            ///Replace on element of boundary with whole simple boundary structure.
            void replaceElement(const t_vert_pos vertice_pos, const Boundary& simple_boundary);
            /*! \brief Reduces lenght of branch by \p lenght.
                \note
                If \p lenght is greater than full lenght of branch, then __source_point__ only remains.
                \throw Exception.
            */
            Boundary& shrink(double lenght);

            /*! \brief Remove tip point from branch(simply pops element from vector).
                \imageSize{BranchShrink.jpg, height:30%;width:30%;, }
                \throw Exception if trying to remove last point.
            */
            Boundary& removeTipPoint();

            ///Clear all data of boundary to make it empty.
            void clear();
            
            //properties and values extractors/evaluators

            /*! \brief Return TipPoint of branch(last point in branch).
                \throw Exception if branch is empty.
            */
            Point tipPoint() const;

            /*! \brief Returns vector of tip - difference between two adjacent points.
                \throw Exception if branch consist only from one point.
                \warning name a little confusing cos there is still source_angle variable.
            */
            Point tipVector() const;

            /*! \brief Returns vector of \p i th segment of branch.
                \throw Exception if branch size is 1 or is \p i > branch size.
            */
            Point vector(unsigned i) const;
            
            /*! \brief Returns angle of branch tip.
                \throw Exception if branch is empty.
            */
            double tipAngle() const;

            ///Returns source point of branch(the first one).
            Point sourcePoint() const;

            ///Returns lenght of whole branch.
            double lenght() const;
            
            //operators 

            bool operator==(const Boundary& simple_boundary) const;

            friend ostream& operator <<(ostream& write, const Boundary & boundary);
    };

    /*! \brief Holds all functionality that you need to work with single branch.
        \details
        Take a look on picture:
        \imageSize{BranchClass.jpg, height:40%;width:40%;, }
        \todo resolve problem with private members
    */
    class Branch: public Boundary
    {
        public:
            Branch() = default;

            /*! \brief Branch construcor.
                \details Initiates branch with initial point \p source_point and initial \p angle.
                \param[in] source_point_val - Branch source point.
                \param[in] angle - Intial growth(or flow) dirrection of brahch source point.
            */
            Branch(const Point& source_point_val, const double angle);

            ///Returns source angle of branch - initial __source_angle__.
            double SourceAngle() const;
            /*! @} */

            ///Prints branch and all its parameters.
            friend ostream& operator<<(ostream& write, const Branch & b);

            ///Comparison of branches.
            bool operator==(const Branch& br) const;
            
        //private:

            ///Initial angle of source(or direction of source).
            double source_angle;

            ///Used in Shrink function call.
            ///If after shrink lenght of between adjacent to tip point
            ///is less then eps then we delete it.
            double eps = EPS;
    };

    
    typedef map<t_boundary_id, Boundary> t_Boundaries;
    typedef pair<Point, double> t_branch_source;

    /*! \brief Structure which defines Boundaries of region.
        
        \details It is a combination of simple boundaries which of each has  it own unique id.
        \imageSize{Boundaries.jpg, height:40%;width:40%;, }
    */
    class Boundaries: public t_Boundaries
    {
        public:
            ///Data structure - interface between boundary sources and source points of \ref Tree
            typedef map<t_source_id, t_branch_source> trees_interface_t;
            
            ///Constructor
            Boundaries(t_Boundaries simple_boundaries = {});

            ///Initialize rectangular Boundaries.
            Sources MakeRectangular(double width = 1, double height = 1, double source_x_position = 0.25);
            
            ///Initialize rectangular with hole Boundaries.
            Sources MakeRectangularWithHole(double width = 1, double height = 1, double source_x_position = 0.25);

            ///Some basic checks of data structure.
            void Check();   

            ///Return outer simple boundary(There should be only one such).
            Boundary& GetOuterBoundary();

            ///Returns vector of all holes.
            t_PointList GetHolesList() const;

            ///Returns map of source points ids and coresponding source point and angle of tree \ref Branch.
            trees_interface_t GetSourcesIdsPointsAndAngles(const Sources& sources) const;

            friend ostream& operator <<(ostream& write, const Boundaries & boundary);

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