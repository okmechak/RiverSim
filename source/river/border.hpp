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
    
     ///Handles initial boundary geometry without any geometry of river and contains info about sources and boundary ids. 
     /** It is wrapper of tethex::Mesh object. 
     * \image html stub.png image description.
     * TODO: Add Images
     */
    class Border
    {
        public:

            
            /// Border constructor.
            /** \param b_mesh - can be empty or initialized with special geometry.*/
            Border(tet::Mesh& b_mesh): borderMesh(b_mesh){};

            /// Initializes Border#borderMesh object of type tethex#Mesh by setting lines and vertices to rectangular shape.
            /** \param regionSize configures size of region. Format is next {width, height}.
             * \param boundaryId assigns ids to each border in next order: right, top, left, bottom.
             * \param sourcesXCoord vector of coordiantes in form {x1, x2, .., xn} of sources on bottom line. 
             *                      Coordinates should be ordered from smaller to bigger value(__clockwise__)     
             * \param sourceId vector of sources Ids. Each id should be __unique__*/
            Border& MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord = {}, vector<int> sourcesId = {});

            /// Read region information from @file_name msh file
            /**
             * TODO: describe exact format of such file.
             */
            Border& ReadFromFile(string file_name)
            {
                borderMesh.read(file_name);
                return *this;
            }

            /// Add border line to each source points.
            /**
             * As each source consist of two points with gap in between, 
             * this function will fill this gap by adding line.
             * \param boundary_id of additional lines. 
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
            
            /// Return source adjacent point(source has __only one__ such points)
            /**
             * \param point_id current point index of position in tethex::Mesh::vertices vector.
             * \result adjacent point index of position in same \ref tethex vector.
             * 
             * Ss used to evalute normal vector. 
             * \see GetSourceNormalAngle()
             */
            int GetAdjacentPointId(int point_id) const;
            /// Return source poinst indexes(each source consist of two points at distance \ref eps)
            /**
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