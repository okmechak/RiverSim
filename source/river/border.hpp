/*! \file border.hpp
    \brief Generate some initial predefinide border geometry and its source positions
    
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

namespace River
{
    class Border
    {
        public:
            ///Intialize object by only taking reference to @tethex::Mesh object
            Border(tet::Mesh& bMesh): borderMesh(bMesh){};
            
            //different shapes

            ///Initializes @borderMesh object to rectangular shape
            /** Size or region is configured by @regionSize as {width, height}.
                             For each line is assigned boundaryId in next order: right, top, left, bottom.
                             Coordiantes of source on bottom line are set by vector @sourcesXCoord
                             Coordinates should be ordered from smaller to bigger value(__clockwise__)     
                             and sources Ids is set by @sourceId and should be __unique__*/
            Border& MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord = {}, vector<int> sourcesId = {});

            ///Read region information from @file_name msh file
            Border& ReadFromFile(string file_name)
            {
                borderMesh.read(file_name);
                return *this;
            }

            ///Add border line to each source
            Border& CloseSources(int boundary_id);


            //addition
            vector<int> GetSourcesId() const;
            vector<Point> GetSourcesPoint() const;
            vector<int> GetHolesId() const ;
            double GetSourceNormalAngle(int source_id) const;
            vector<double> GetSourcesNormalAngle() const
            {
                vector<double> norm_angles;
                for(auto id: GetSourcesId())
                    norm_angles.push_back(GetSourceNormalAngle(id));

                return norm_angles;
            }

        //private: for testing purposes
            double eps = 1e-10;
            tet::Mesh& borderMesh;
            
            int GetAdjacentPointId(int point_id) const;
            pair<int, int> GetSourceVerticesIndexById(int source_id) const;

            ///All hole points should be indicated with @hole_point_index material_id
            const int hole_point_index = -1;
            ///All source points should be indicated with material_id >= @first_source_index
            const int first_source_index = 1;
            bool IsHole(int point_id) const
            {
                return point_id == hole_point_index;
            }
            bool IsSource(int point_id) const
            {
                return point_id >= first_source_index;
            }
    };
}