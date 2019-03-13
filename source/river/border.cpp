#include "border.hpp"

namespace River{

    Border& Border::MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord, vector<int> sourcesId)
    {
        auto width = regionSize[0],
            height = regionSize[1];

        //corner Points of rectangular region
        vector<tet::Point> regionPoints{
            tet::Point(width, 0),
            tet::Point(width, height),
            tet::Point(0, height),
            tet::Point(0, 0)
        };

        //Border Lines
        vector<tet::MeshElement * > borderLines
        {
            new tet::Line(0, 1, boundariesId[0]),//indexes starts from zero
            new tet::Line(1, 2, boundariesId[1]),
            new tet::Line(2, 3, boundariesId[2])
        };
        
        vector<tet::MeshElement *> sources;
        auto indexShift = regionPoints.size() - 1;
        int i = 0;
        for(; i < sourcesId.size(); ++i)
        {
            regionPoints.push_back(
                tet::Point(sourcesXCoord[i], 0/*y coord*/));
            sources.push_back(
                new tet::PhysPoint(i + indexShift, sourcesId[i]));
            
            if(i < sourcesId.size())
                borderLines.push_back(
                    new tet::Line(i + indexShift, i + 1 + indexShift,
                        boundariesId[3]));
        }

        borderLines.push_back(
                    new tet::Line(i + indexShift, 0, boundariesId[3]));

        //passing values to meshBorder object
        borderMesh.set_vertexes(regionPoints);
        borderMesh.set_lines(borderLines);

        return *this;
    }
}