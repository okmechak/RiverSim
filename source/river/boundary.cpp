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

#include "boundary.hpp"
///\cond
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
///\endcond

using namespace std;

namespace River
{
    //BoundaryCondition
    bool BoundaryCondition::operator==(const BoundaryCondition& bc) const
    {
        return value == bc.value && type == bc.type;
    }
    ostream& operator <<(ostream& write, const BoundaryCondition & boundary_condition)
    {
        switch (boundary_condition.type)
        {
            case DIRICHLET:
                write << "boudary type: Dirichlet, ";
                break;
            case NEUMAN:
                write << "boudary type: Neuman, ";
                break;
            default:
                write << "boudary type: Undefined(" <<boundary_condition.type << "), ";
                break;
        }
        write << "value: " << boundary_condition.value;

        return write;
    }

    //BoundaryConditions
    t_BoundaryConditions BoundaryConditions::Get(t_boundary type) const
    {
        t_BoundaryConditions bd;
        if(type != DIRICHLET && type != NEUMAN)
            throw Exception("BoundaryConditions:Get: unknown boundary type: " + to_string(type));
            
        for(const auto& [boundary_id, boundary_condition]:*this)
            if(boundary_condition.type == type)
                bd[boundary_id] = boundary_condition;

        return bd;
    }

    ostream& operator <<(ostream& write, const BoundaryConditions & bcs)
    {
        for(const auto&[id, value]: bcs)
            write << "id = " << id << ", " << value << endl;
        return write;
    }

    //Sources
    vector<t_source_id> Sources::GetSourcesIds() const
    {
        vector<t_source_id> sources_ids;
        for(auto[source_id, value]: *this)
            sources_ids.push_back(source_id);

        return sources_ids;
    }

    //Line
    bool Line::operator==(const Line& line) const
    {
        return p1 == line.p1 && p2 == line.p2 && boundary_id == line.boundary_id;
    }
    ostream& operator <<(ostream& write, const Line & line)
    {
        write << "line [ " << line.p1 << ", " << line.p2 
            << "], boundary_id = " << line.boundary_id;

        return write;
    }

    //Boundary
    void Boundary::Append(const Boundary& simple_boundary)
    {
        auto size = vertices.size();

        vertices.insert(
            vertices.end(),  
            simple_boundary.vertices.begin(), 
            simple_boundary.vertices.end());
        
        auto shift = lines.size();
        lines.insert(
            lines.end(),  
            simple_boundary.lines.begin(), 
            simple_boundary.lines.end());

        for(auto i = shift; i < lines.size(); ++i)
        {
            lines.at(i).p1 += size;
            lines.at(i).p2 += size;
        }
        
        holes.insert(end(holes), begin(simple_boundary.holes), end(simple_boundary.holes));
    }

    void Boundary::ReplaceElement(t_vert_pos vertice_pos, const Boundary& simple_boundary)
    {
        if(simple_boundary.vertices.empty())
            return;
        
        if(vertice_pos > vertices.size())
            throw Exception("Boundary: trying to insert at position out of boundaries");

        //remove target element
        if(!vertices.empty())
            vertices.erase(vertices.begin() + vertice_pos);

        vertices.insert(
            vertices.begin() + vertice_pos,  
            simple_boundary.vertices.begin(), 
            simple_boundary.vertices.end());

        auto shift = simple_boundary.vertices.size() - 1;
        for(auto& line: lines)
            //line is right to insertion point
            if (line.p1 >= vertice_pos && line.p2 > vertice_pos)
            {
                line.p1 += shift;
                line.p2 += shift;
            }
            //one of line edges is left to insertion
            else if(line.p1 <= vertice_pos && line.p2 > vertice_pos)
                line.p2 += shift;
            //edge is closing the loop
            else if(line.p1 >= vertice_pos && line.p2 <= vertice_pos)
                line.p1 += shift;

        lines.insert(
            lines.begin() + vertice_pos,  
            simple_boundary.lines.begin(), 
            simple_boundary.lines.end());

        for(size_t i = 0; i < simple_boundary.lines.size(); ++i)
        {
            lines.at(i + vertice_pos).p1 += vertice_pos;
            lines.at(i + vertice_pos).p2 += vertice_pos;

        }

        holes.insert(end(holes), begin(simple_boundary.holes), end(simple_boundary.holes));
    }

    bool Boundary::operator==(const Boundary &simple_boundary) const
    {
        return vertices == simple_boundary.vertices 
            && lines == simple_boundary.lines
            && inner_boundary == simple_boundary.inner_boundary
            && holes == simple_boundary.holes
            && name == simple_boundary.name;
    }

    ostream& operator <<(ostream& write, const Boundary & boundary)
    {
        write << "Name: " << boundary.name << endl;

        write << "inner boudary: " << boundary.inner_boundary << endl;
        if(boundary.inner_boundary)
            for(auto& h: boundary.holes)
                write << h << endl;

        write << "Vertices:" << endl;
        for(const auto& v: boundary.vertices)
            write << v << endl;

        write << "Lines:" << endl;
        for(const auto& l: boundary.lines)
            write << l << endl;

        return write;
    }

    //Region
    Region::Region(t_Boundaries simple_boundaries)
    {
        for(auto &[boundary_id, simple_boundary]: simple_boundaries)
            this->at(boundary_id) = simple_boundary;
    }

    void Region::Check()
    {
        if (this->empty())
            throw Exception("Boundary is empty.");
        
        t_vert_pos 
            outer_boudaries = 0;
        vector<t_boundary_id> boundary_ids;

        for(const auto& [boundary_id, simple_boundary]: *this)
        {
            boundary_ids.push_back(boundary_id);

            if(simple_boundary.vertices.size() != simple_boundary.lines.size())
                throw Exception("Vertices and lines sizes, are different in boudary");

            if(!simple_boundary.inner_boundary)
                ++outer_boudaries;
        }

        if(outer_boudaries == 0)
            throw Exception("There is no outer boudaries!");
        else if(outer_boudaries > 1)
            throw Exception("Only one outer boudary is allowed: " + to_string(outer_boudaries));

        auto num_of_boundary_ids = boundary_ids.size();
        sort( boundary_ids.begin(), boundary_ids.end() );
        boundary_ids.erase( 
            unique( boundary_ids.begin(), boundary_ids.end() ), boundary_ids.end() );
        
        if(num_of_boundary_ids != boundary_ids.size())
            throw Exception("Boundary: Boundary ids should be unique numbers!");
    }

    t_PointList Region::GetHolesList() const
    {
        t_PointList holes;
        for(const auto& [boundary_id, simple_boundary]: *this)
            if(simple_boundary.inner_boundary)
                for(auto& h: simple_boundary.holes)
                    holes.push_back(h);

        return holes;
    }

    Sources Region::MakeRectangular(double width, double height, double source_x_position)
    {
        (*this)[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {source_x_position, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            },
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/

        Sources sources;
        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};
            
        return sources;
    }

    Sources Region::MakeRectangularWithHole(double width, double height, double source_x_position)
    {
        (*this)[1] = 
        {/*Outer Boundary*/
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {source_x_position, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            }, 
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer boudary"/*just name*/
        };

        Sources sources;
        sources[1] = {1, 1};

        (*this)[2] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.25*width, 0.75*height},
                {0.75*width, 0.75*height}, 
                {0.75*width, 0.25*height}, 
                {0.25*width, 0.25*height}
            }, 
            {/*lines*/
                {0, 1, 6},
                {1, 2, 7},
                {2, 3, 8},
                {3, 0, 9} 
            }, 
            true/*this is inner boudary*/,
            {{0.5*width, 0.5*height}}/*holes*/,
            "hole"/*just name*/
        };
        sources[2] = {2, 1};

        (*this)[3] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.8*width, 0.9*height},
                {0.9*width, 0.9*height}, 
                {0.9*width, 0.8*height}, 
                {0.8*width, 0.8*height}
            }, 
            {/*lines*/
                {0, 1, 10},
                {1, 2, 11},
                {2, 3, 12},
                {3, 0, 13} 
            }, 
            true/*this is inner boudary*/,
            {{0.85*width, 0.85*height}}/*holes*/,
            "hole"/*just name*/
        };
        sources[3] = {3, 3};
        
        return sources;
    }

    Boundary& Region::GetOuterBoundary()
    {
        for(auto& [boundary_id, simple_boundary]: *this)
            if(!simple_boundary.inner_boundary)
                return simple_boundary;

        throw Exception("Boundary: There is no outer boundary");

        return this->at(1);
    }

    pair<t_vert_pos, t_vert_pos> Region::GetAdjacentVerticesPositions(
            const t_vert_pos vertices_size, 
            const t_vert_pos vertice_pos)
    {
        if (vertice_pos >= vertices_size)
            throw Exception("Vertice position is bigger then number of vertices: vertice pos " + to_string(vertice_pos));

        t_vert_pos 
            left_pos = vertice_pos - 1, 
            right_pos = vertice_pos + 1;

        if(vertice_pos == vertices_size - 1 )
            right_pos = 0;
        else if(vertice_pos == 0)
            left_pos = vertices_size - 1;
            
        return {left_pos, right_pos};
    }

    double Region::NormalAngle(const Point& left, const Point& center, const Point& right)
    {
        const auto
            left_vector = center - left,
            right_vector = right - center;

        const auto
            angle_relative  = left_vector.angle(right_vector),
            angle_absolute = left_vector.angle();
        
        auto normal_angle = (M_PI + angle_relative) / 2 + angle_absolute;

        if(normal_angle >= 2*M_PI)
            normal_angle -= 2*M_PI;
        else if(normal_angle <= -2*M_PI)
            normal_angle += 2*M_PI;

        return normal_angle;
    }

    double Region::GetVerticeNormalAngle(const t_PointList& vertices, const t_vert_pos vertice_pos)
    {
        const auto vertices_size = vertices.size();
        const auto [vertice_pos_left, vertice_pos_right] = GetAdjacentVerticesPositions(vertices_size, vertice_pos);

        const auto& p_source = vertices.at(vertice_pos), 
            p_left = vertices.at(vertice_pos_left), 
            p_right = vertices.at(vertice_pos_right);
        
        auto normal_angle = NormalAngle(p_left, p_source, p_right);
        
        if(normal_angle >= 2*M_PI)
            normal_angle -= 2*M_PI;
        
        return normal_angle;
    }

    Region::trees_interface_t Region::GetSourcesIdsPointsAndAngles(const Sources& sources) const
    {
        trees_interface_t sources_ids_points_and_angles;

        for(const auto& [source_id, value]: sources)
        {
            auto boundary_id = value.first;
            auto vertice_pos = value.second;
            auto& simple_boundary = this->at(boundary_id);

            sources_ids_points_and_angles[source_id] = {
                simple_boundary.vertices.at(vertice_pos),
                GetVerticeNormalAngle(
                    simple_boundary.vertices,
                    vertice_pos)
                };
        }

        return sources_ids_points_and_angles;
    }

    ostream& operator <<(ostream& write, const Region& boundaries)
    {
        for(const auto& [boundary_id, simple_boundary]: boundaries)
            write << "id = " << boundary_id << ", " << simple_boundary << endl;

        return write;
    }
}