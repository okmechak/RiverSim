/**
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

namespace River
{

    Border& Border::MakeRectangular(
                vector<double> regionSize, vector<int> boundariesId,
                vector<double> sourcesXCoord, vector<int> sourcesId)
    {
        if(sourcesXCoord.at(0) <= 0 || sourcesXCoord.back() >= regionSize.at(0))
            throw invalid_argument("X coords of sources should be in interval (0, width)");

        auto width = regionSize.at(0),
            height = regionSize.at(1);

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
            new tet::Line(0, 1, boundariesId.at(0)),//indexes starts from zero
            new tet::Line(1, 2, boundariesId.at(1)),
            new tet::Line(2, 3, boundariesId.at(2))
        };
        
        //Iterating over sources points
        vector<tet::MeshElement *> sources;
        unsigned i = 0;
        for(; i < sourcesId.size(); ++i)
        {
            //Pushing left point
            regionPoints.push_back(
                tet::Point(sourcesXCoord[i] - eps/2, 0/*y coord*/));
            sources.push_back(
                new tet::PhysPoint(regionPoints.size() - 1 , sourcesId[i]));

            if(i < sourcesId.size())
                borderLines.push_back(
                    new tet::Line(regionPoints.size() - 2, regionPoints.size() - 1,
                        boundariesId.at(3)));


            //Pushing right point
            regionPoints.push_back(
                tet::Point(sourcesXCoord[i] + eps/2, 0/*y coord*/));
            sources.push_back(
                new tet::PhysPoint(regionPoints.size() - 1 , sourcesId[i]));
            
            //last one point is ommited
        }
        borderLines.push_back(
                    new tet::Line(regionPoints.size() - 1, 0, boundariesId[3]));


        //passing values to meshBorder object
        borderMesh.set_vertexes(regionPoints);
        borderMesh.set_points(sources);
        borderMesh.set_lines(borderLines);

        return *this;
    }

    Border& Border::CloseSources(int boundary_id)
    {
        vector<tethex::MeshElement *> border_lines;
        for(auto id: GetSourcesId())
        {
            auto[vert_left, vert_right] = Border::GetSourceVerticesIndexById(id);
            border_lines.push_back(
                    new tet::Line(vert_left, vert_right, boundary_id));
            

        }
            
        borderMesh.append_lines(border_lines);

        return *this;
    }

    vector<int> Border::GetSourcesId() const
    {
        vector<int> sources_id;
        auto points = borderMesh.get_points();
        for(unsigned i = 0; i < points.size(); ++i)
        {
            //we are using __material_id__ as simply __id__ to distinguish different source points
            auto id = points[i]->get_material_id();
            if(IsSource(id))
            {
                sources_id.push_back(id);
                //cos next clockwise point will be  same point
                i++;
            }
        }
        return sources_id;
    }

    vector<Point> Border::GetSourcesPoint() const
    {
        vector<Point> sources_point;
        auto points = borderMesh.get_points();

        for(unsigned i = 0; i < points.size(); i+=2)
            //we are using __material_id__ as simply __id__ to distinguish different source points
            if(IsSource(points.at(i)->get_material_id()))
            {    
                auto vert_left = borderMesh.get_vertex(points.at(i)->get_vertex(0));
                //cos we are moving in clockwise order if one point is source then next one should be too
                tethex::Point vert_right;
                if(i == points.size() - 1)
                    vert_right = borderMesh.get_vertex(points.at(0)->get_vertex(0));
                else
                    vert_right = borderMesh.get_vertex(points.at(i + 1)->get_vertex(0));

                sources_point.push_back(
                    Point{vert_left.get_coord(0)/2 + vert_right.get_coord(0)/2, 
                    vert_left.get_coord(1)/2 + vert_right.get_coord(1)/2});
            }
        
        return sources_point;
    }

    vector<int> Border::GetHolesId() const
    {
        vector<int> holes_id;
        for(auto point: borderMesh.get_points()){
            //we are using __material_id__ as simply __id___ to distinguish different source points
            auto id = point->get_material_id();
            if(IsHole(id))
                holes_id.push_back(id);
        }
        return holes_id;
    }


    int Border::GetAdjacentPointId(int point_id) const
    {
        //it should have only one point, cos it is a border
        tethex::MeshElement* adjacent_line = NULL;
        for(auto line: borderMesh.get_lines())
            if(line->contains(point_id))
                adjacent_line = line;
        int vertexIndex = -1;
        if(adjacent_line == NULL)
            throw invalid_argument("Adjacent line was not found.");
        else
        {
            vertexIndex = adjacent_line->get_vertex(0);
            if(point_id == vertexIndex)
                vertexIndex = adjacent_line->get_vertex(1);
        }
        
        return vertexIndex;
    }


    ///Returns source ver
    pair<int, int> Border::GetSourceVerticesIndexById(int source_id) const
    {
        for(int i = 0; i < borderMesh.get_n_points(); ++i)
            if(borderMesh.get_point(i).get_material_id() == source_id)
                return {borderMesh.get_point(i).get_vertex(0), borderMesh.get_point(i).get_vertex(0) + 1};

        throw std::invalid_argument("PhysPoint with such source id doesn't exist");

        return {-1, -2};
    }

    double Border::GetSourceNormalAngle(int source_id) const
    {
        auto [p_left, p_right] = GetSourceVerticesIndexById(source_id);
        auto v_left = borderMesh.get_vertex(p_left);
        auto v_right = borderMesh.get_vertex(p_right);

        auto adjacent_left = borderMesh.get_vertex(GetAdjacentPointId(p_left));
        auto adjacent_right = borderMesh.get_vertex(GetAdjacentPointId(p_right));

        auto angle_left = River::Point::angle(
                v_left.get_coord(0) - adjacent_left.get_coord(0),
                v_left.get_coord(1) - adjacent_left.get_coord(1));

        auto angle_right = River::Point::angle(
                adjacent_right.get_coord(0) - v_right.get_coord(0),
                adjacent_right.get_coord(1) - v_right.get_coord(1));

        return (angle_left + angle_right) / 2 + M_PI/2/*we are looking for perpendicular*/;
    }
}