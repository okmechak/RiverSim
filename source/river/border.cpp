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
#include "border.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

namespace River
{

    Border& Border::MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord, vector<int> sourcesId)
    {
        //clear all data
        vertices.clear();
        sources.clear();
        lines.clear();
        
        //checks
        if(sourcesXCoord.at(0) <= 0 || sourcesXCoord.back() >= regionSize.at(0))
            throw invalid_argument("X coords of sources should be in interval (0, width)");

        if(!is_sorted(sourcesXCoord.begin(), sourcesXCoord.end()))
            throw invalid_argument("Sources X coords should be sorted! From smaller to greater");

        auto width = regionSize.at(0),
            height = regionSize.at(1);

        //corner Points of rectangular region
        vertices = {
            Point{width, 0},
            Point{width, height},
            Point{0, height},
            Point{0, 0}
        };

        //Border Lines
        lines = {
            {0, 1, boundariesId.at(0)},
            {1, 2, boundariesId.at(1)},
            {2, 3, boundariesId.at(2)}
        };
        
        //Iterating over sources points
        for(long unsigned i = 0; i < sourcesId.size(); ++i)
        {
            vertices.push_back(Point{sourcesXCoord.at(i), 0});
            auto index = vertices.size() - 1;
            sources[sourcesId.at(i)] = index;
            lines.push_back({index - 1, index, boundariesId.at(3)});
        }
        lines.push_back({vertices.size() - 1, 0, boundariesId.at(3)});

        return *this;
    }


    vector<int> Border::GetSourcesId() const
    {
        vector<int> sources_id;
        for(auto[key, value]: sources)
            sources_id.push_back(key);
        
        return sources_id;
    }

    vector<Point> Border::GetSourcesPoint() const
    {
        vector<Point> sources_point;
        for(auto[id, value]: sources)
            sources_point.push_back(GetSourcePoint(id));
        
        return sources_point;
    }


    pair<Point, Point>  Border::GetAdjacentPoints(int source_id) const
    {   
        long unsigned left = sources.at(source_id) - 1,
            right = sources.at(source_id) + 1;

        if (sources.at(source_id) < 1)
            left = vertices.size() - 1;
        
        if (right > vertices.size() - 1)
            right = 0;

        return {vertices[left], vertices[right]};
    }


    Point Border::GetSourcePoint(int source_id) const
    {
        return vertices.at(sources.at(source_id));
    }


    double Border::GetSourceNormalAngle(int source_id) const
    {
        auto [p_left, p_right] = GetAdjacentPoints(source_id);
        auto p_source = GetSourcePoint(source_id);

        auto angle_left = (p_source - p_left).angle(),
             angle_right = (p_right - p_source).angle();

        return (angle_left + angle_right) / 2 + M_PI/2/*we are looking for perpendicular*/;
    }
}