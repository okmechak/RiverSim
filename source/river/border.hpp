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

/** @file border.hpp
 *  Generate initial predefinide border geometry and its source positions.
 *  
 *  First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
 *  for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
 *  with any source position on tis border
 */

#pragma once

#include <vector>
#include <string>
#include <map>
#include "common.hpp"

using namespace std;

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
             */
            Border() = default;

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
             * @name Getters for sources parametets
             * @{
             */
            ///Returns vector of sources ids.
            vector<int> GetSourcesId() const;

            /// Returns vector of sources points(both source point is merged into one).
            vector<Point> GetSourcesPoint() const;
            
            ///Return Source Point
            Point GetSourcePoint(int source_id) const;
            
            ///Return source point vertice position
            long unsigned GetSourceVerticeIndex(int source_id) const
            {
                return sources.at(source_id);
            }
            
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

            map<long unsigned, int> SourceByVerticeIdMap() const
            {
                map<long unsigned, int> r;
                for (const auto& kv : sources)
                    r[kv.second] = kv.first;
                return r;
            } 

        //private: for testing purposes

            /**
             * @name Private
             * @{
             */
            vector<Point> vertices;///<
            vector<Line> lines;
            map<int, long unsigned> sources;
            vector<long unsigned> holes;
            
            /**
             * Return source adjacent point(source has __only one__ such points).
             *
             * \param point_id current point index of position in tethex::Mesh::vertices vector.
             * \result adjacent point index of position in same \ref tethex vector.
             * 
             * Used to evalute normal vector. 
             * \see GetSourceNormalAngle()
             */
            pair<Point, Point>  GetAdjacentPoints(int source_id) const;
            
            /**
             * @}
             */
    };
}