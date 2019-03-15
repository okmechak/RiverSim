#include "border.hpp"

namespace River
{

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
        
        //Iterating over sources points
        vector<tet::MeshElement *> sources;
        auto indexShift = regionPoints.size() - 1;
        int i = 0;
        for(; i < sourcesId.size(); ++i)
        {
            regionPoints.push_back(
                tet::Point(sourcesXCoord[i], 0/*y coord*/));
            sources.push_back(
                new tet::PhysPoint(regionPoints.size() - 1 , sourcesId[i]));
            
            //last one point is ommited
            if(i < sourcesId.size())
                borderLines.push_back(
                    new tet::Line(i + indexShift, i + 1 + indexShift,
                        boundariesId[3]));
        }
        borderLines.push_back(
                    new tet::Line(i + indexShift, 0, boundariesId[3]));


        //passing values to meshBorder object
        borderMesh.set_vertexes(regionPoints);
        borderMesh.set_points(sources);
        borderMesh.set_lines(borderLines);

        return *this;
    }

    vector<int> Border::GetSourcesId() const
    {
        vector<int> sources_id;
        for(auto point: borderMesh.get_points()){
            //we are using __material_id__ as simply __id___ to distinguish different source points
            auto id = point->get_material_id();
            if(IsSource(id))
                sources_id.push_back(id);
        }
        return sources_id;
    }

    vector<Point> Border::GetSourcesPoint() const
    {
        vector<Point> sources_point;
        for(auto point: borderMesh.get_points())
            //we are using __material_id__ as simply __id___ to distinguish different source points
            if(IsSource(point->get_material_id()))
            {    
                auto vertice = borderMesh.get_vertex(point->get_vertex(0));
                sources_point.push_back(Point{vertice.get_coord(0), vertice.get_coord(1)});
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

    vector<tethex::MeshElement *> Border::GetPointLines(int point_id) const
    {
        vector<tethex::MeshElement *> point_lines;
        for(auto line: borderMesh.get_lines())
            if(line->contains(point_id))
                point_lines.push_back(line);
        
        return point_lines;
    }

    vector<int> Border::GetAdjacentPointsId(int point_id) const
    {
        vector<int> adjacent_points;
        //it should have only two points, cos it is a border
        auto point_lines = GetPointLines(point_id);
        for(auto line: point_lines)
        {
            auto vertexIndex = line->get_vertex(0);
            if(point_id == vertexIndex)
                vertexIndex = line->get_vertex(1);

            adjacent_points.push_back(vertexIndex);
        }        

        return adjacent_points;
    }

    tet::MeshElement& Border::GetSourceById(int source_id) const
    {
        for(int i = 0; i < borderMesh.get_n_points(); ++i)
            if(borderMesh.get_point(i).get_material_id() == source_id)
                return borderMesh.get_point(i);

        throw std::invalid_argument("PhysPoint with such source id doesn't exist");

        return borderMesh.get_point(0);
    }

    double Border::GetSourceNormalAngle(int source_id) const
    {
        auto source_point_id = GetSourceById(source_id).get_vertex(0);

        auto adjacent_points_id = GetAdjacentPointsId(source_point_id);
        auto source_point = borderMesh.get_vertex(source_point_id);

        auto angle1 = River::Point::angle(
                - borderMesh.get_vertex(adjacent_points_id[0]).get_coord(0) 
                + source_point.get_coord(0),
                - borderMesh.get_vertex(adjacent_points_id[0]).get_coord(1) 
                + source_point.get_coord(1));

        auto angle2 = River::Point::angle(
                + borderMesh.get_vertex(adjacent_points_id[1]).get_coord(0) 
                - source_point.get_coord(0),
                + borderMesh.get_vertex(adjacent_points_id[1]).get_coord(1) 
                - source_point.get_coord(1));


        //Point::angle()
        return (angle1 + angle2) / 2 + M_PI/2/*we are looking for perpendicular*/;
    }
}