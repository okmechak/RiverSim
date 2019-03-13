/*! \file border.hpp
    \brief Generate some initial predefinide border geometry and its source positions
    
    First step at beginning of solving this problem is defining a region. It should be quite flexible, cos
    for testing purposes we want to use simple rectangular region, but for real models we need almost any shape
    with any source position on tis border
*/

#pragma once

#include <tethex.hpp>
#include <vector>
#include <algorithm>
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
            ~Border(){};
            
            //different shapes

            ///Initializes @borderMesh object to rectangular shape
            /** Size or region is configured by @regionSize as {width, height}.
                             For each line is assigned boundaryId in next order: right, top, left, bottom.
                             Coordiantes of source on bottom line are set by vector @sourcesXCoord
                             Coordinates should be ordered from smaller to bigger value     
                             and sources Ids is set by @sourceId*/
            Border& MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord = {}, vector<int> sourcesId = {});

        private:
            tet::Mesh& borderMesh;
    };
}