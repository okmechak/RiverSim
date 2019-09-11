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

/*! \file border.hpp
    \brief Generate initial predefinide border geometry and its source positions.
    
    \details First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
    for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
    with any source position on this border.
    For additional information see \ref River::Border.
*/

#pragma once

///\cond
#include <vector>
#include <string>
#include <map>
///\endcond
#include "common.hpp"

using namespace std;

/*! \namespace River
    \brief River namespace holds everything that is related to River simulation.
*/
namespace River
{   
    /*! \class Border
        \brief
        Handles initial boundary geometry without any geometry of river itself 
        and contains info about sources and boundary ids. 

        \details
        River::Border has two purposes - 
        handle boundary of region which is futher used by River::Solver for setting boundry conditions
        and another one - it provides information about coordinates of sources and its dirrection of
        growth(perpendicular to boundary). 
        Boundary can be initialized by Border::MakeRectangular() to rectangular region. 
        Or it can read *.msh file by calling Border::ReadFromFile() function.

        Border is as wrapper of tethex::Mesh object.

        \imageSize{BorderClass.jpg, height:40%;width:40%;, }

        \todo It has very limited specification. Points can be only counterclockwise,
        River source can be only at the end on vertices vector.
        \todo Resolve problem with private members.
        \todo test for different number and position of source points.
        \warning May crash at import of external boundary data structure.
    */
    class Border
    {
        public:
            /*! \brief Border constructor.
                \param[in] vertices Vector of boundary points.
                \param[in] lines Vector of boundary lines between points.
                \param[in] sources Map of id an point position in points vector. Use to set rivers sources points.
                \param[in] holes Map of id an point position in points vector. Use to set holes regions.
            */
            Border(const vector<Point>& vertices = {}, const vector<Line>& lines = {},
                const map<int, long unsigned>& sources = {}, const vector<long unsigned>& holes = {})
            {
                Border::vertices = vertices;
                Border::lines = lines;
                Border::sources = sources;
                Border::holes = holes;
            }

            ///Asignment operator
            Border& operator=(const Border& b) = default;
            
            /*! \brief
                Initializes Border::borderMesh object of type tethex::Mesh by setting lines and vertices to rectangular shape.
                \imageSize{RectangularBorderClass.jpg, height:40%;width:40%;, }
                \param[in] regionSize configures size of region. Format is next {width, height}.
                \param[in] boundariesId assigns ids to each border in next order: right, top, left, bottom.
                \param[in] sourcesXCoord vector of coordiantes in form {x1, x2, .., xn} of sources on bottom line. 
                                     Coordinates should be ordered from smaller to bigger value(__clockwise__)     
                \param[in] sourcesId vector of sources Ids. Each id should be __unique__
            */
            Border& MakeRectangular(
                const vector<double>& regionSize, const vector<int>& boundariesId,
                const vector<double>& sourcesXCoord = {}, const vector<int>& sourcesId = {});

            ///Get vertices vector of border
            inline const vector<Point>& GetVertices() const
            {
                return vertices;
            }
            
            ///Get lines vector of border
            inline const vector<Line>& GetLines() const
            {
                return lines;
            }

            ///Return \ref River::Border::sources value.
            inline const map<int, long unsigned>& GetSourceMap() const
            {
                return sources;
            }

            /*! \name Getters for sources parametets
                @{
            */

            ///Returns vector of sources ids.
            vector<int> GetSourcesId() const;

            /// Returns vector of sources points(both source point is merged into one).
            vector<Point> GetSourcesPoint() const;
            
            ///Return Source Point
            Point GetSourcePoint(int source_id) const;
            
            ///Return source point vertice position
            inline long unsigned GetSourceVerticeIndex(int source_id) const
            {
                return sources.at(source_id);
            }
            
            ///Return __normal line angle__ to boundary at point where is located source with __source_id__
            double GetSourceNormalAngle(int source_id) const;

            ///Returns vector of all nolrmall angles.
            inline vector<double> GetSourcesNormalAngle() const
            {
                vector<double> norm_angles;
                for(auto id: GetSourcesId())
                    norm_angles.push_back(GetSourceNormalAngle(id));

                return norm_angles;
            }
            
            /*! @} */

            ///Generates Map where keys are vertices Id and values - id of corresponding source.
            inline map<long unsigned, int> SourceByVerticeIdMap() const
            {
                map<long unsigned, int> r;
                for (const auto& kv : sources)
                    r[kv.second] = kv.first;
                return r;
            } 

            friend class border_test_class;

        //private:

            /*!
                @name Private
                @{
             */
            ///Vertices of border in counterclockwise order.
            vector<Point> vertices;
            ///Lines of borders between vertices.
            vector<Line> lines;
            ///Map of source Id and corresponding vertice number.
            map<int, long unsigned> sources;
            ///Vector of holes vertices.
            vector<long unsigned> holes;
            
            /*!
                Return source adjacent points.

                \param[in] source_id current point index of position in tethex::Mesh::vertices vector.
                \result adjacent point index of position in same \ref tethex.hpp vector.

                Used to evalute normal vector. 
                \see GetSourceNormalAngle()
             */
            pair<Point, Point>  GetAdjacentPoints(int source_id) const;
            
            /*! @} */
    };
}