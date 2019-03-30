/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/*! \file border.hpp
    Generate initial predefinide border geometry and its source positions.
    
    First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
    for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
    with any source position on tis border
*/

#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include "tethex.hpp"
#include "common.hpp"

using namespace std;
namespace tet = tethex;

/**
 * River namespace holds everything that is related to River simulation.
 */
namespace River
{   
    /** 
     * Handles initial boundary geometry without any geometry of river itself 
     * and contains info about sources and boundary ids. 
     * 
     * River::Border has two purposes - 
     * handle boundary of region which is futher used by River::Solver for setting boundry conditions
     * and another one - it provides information about coordinates of sources and its dirrection of
     * growth(perpendicular to boundary). 
     * Boundary can be initialized by Border::MakeRectangular() to rectangular region. 
     * Or it can read *.msh file by calling Border::ReadFromFile() function.
     * 
     * Border is as wrapper of tethex::Mesh object.
     * 
     *  \image html stub.png image description.
     *  TODO: Add Images
     */
    class Border
    {
        public:

            
            /** 
             * Border constructor.
             * \param b_mesh - can be empty or initialized with __special geometry__(see ReadFromFile() for more details).
             */
            Border(tet::Mesh& b_mesh): borderMesh(b_mesh){};

            /**
             * Initializes Border::borderMesh object of type tethex::Mesh by setting lines and vertices to rectangular shape.
             * \param regionSize configures size of region. Format is next {width, height}.
             * \param boundariesId assigns ids to each border in next order: right, top, left, bottom.
             * \param sourcesXCoord vector of coordiantes in form {x1, x2, .., xn} of sources on bottom line. 
             *                      Coordinates should be ordered from smaller to bigger value(__clockwise__)     
             * \param sourcesId vector of sources Ids. Each id should be __unique__
             */
            Border& MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord = {}, vector<int> sourcesId = {});

            /**
             * Read region information from __file_name__ msh file
             * 
             * File have several consrtaints. 
             * 1. First of all it should
             * be of msh version 2 format.  
             * 2. It should contain only points and lines.  
             * 3. Points with marker equal to Border::hole_point_index indicates holes. 
             * Remember that hole points should be circled by lines in other case all mesh will
             * seeing as hole.  
             * 4. Points with, marker equal to Border::source_point_index - are source points.
             * They should be set in very special way, for example:
             * --*--*  *--*--*--* *--*
             * where those points aroud gap are points of __same__ source. And should have same index >= 1.
             * 5. Line markers indicates boundary condition.
             */
            Border& ReadFromFile(string file_name)
            {
                borderMesh.read(file_name);
                return *this;
            }

            /**
             * Add border line to each source points.
             * 
             * As each source consist of two points with gap in between, 
             * this function will fill this gap by adding line.
             * \param boundary_id of additional lines. 
             * Used only for test purposes.
             */
            Border& CloseSources(int boundary_id);


            /**
             * @name Getters for sources parametets
             * @{
             */
            ///Returns vector of sources ids.
            vector<int> GetSourcesId() const;

            /// Returns vector of sources points(both source point is merged into one).
            vector<Point> GetSourcesPoint() const;
            
            /// Returns holes ids.
            vector<int> GetHolesId() const ;
            
            /// Return __normal line angle__ to boundary at point where is located source with __source_id__
            double GetSourceNormalAngle(int source_id) const;

            /// Returns vector of all nolrmall angles.
            vector<double> GetSourcesNormalAngle() const
            {
                vector<double> norm_angles;
                for(auto id: GetSourcesId())
                    norm_angles.push_back(GetSourceNormalAngle(id));

                return norm_angles;
            }
            /**
             * @}
             */

        //private: for testing purposes

            /**
             * @name Private
             * @{
             */
            double eps = 1e-10; ///< width between points at each source
            tet::Mesh& borderMesh; ///< tethex::Mesh object with geometry info inside
            
            /**
             * Return source adjacent point(source has __only one__ such points).
             *
             * \param point_id current point index of position in tethex::Mesh::vertices vector.
             * \result adjacent point index of position in same \ref tethex vector.
             * 
             * Used to evalute normal vector. 
             * \see GetSourceNormalAngle()
             */
            int GetAdjacentPointId(int point_id) const;
            
            /**
             * Return source poinst indexes(each source consist of two points at distance \ref eps).
             *
             * \param source_id source id
             * \result pair of points indexes position in vertices vector.
             */
            pair<int, int> GetSourceVerticesIndexById(int source_id) const;

            /// All hole points should be indicated with \ref hole_point_index material_id.
            const int hole_point_index = -1;
            /// All source points should be indicated with material_id >= \ref first_source_index.
            const int first_source_index = 1;
            
            /// Checks if point id value represents hole.
            bool IsHole(int point_id) const
            {
                return point_id == hole_point_index;
            }

            /// Checks if point id value represents source.
            bool IsSource(int point_id) const
            {
                return point_id >= first_source_index;
            }
            /**
             * @}
             */
    };
}