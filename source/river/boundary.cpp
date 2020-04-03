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

    ostream& operator <<(ostream& write, const Line & line)
    {
        write << "line [ " << line.p1 << ", " << line.p2 
            << "], boundary_id = " << line.boundary_id;

        return write;
    }

    void SimpleBoundary::Append(SimpleBoundary simple_boundary)
    {
        auto size = vertices.size();

        vertices.insert(
            vertices.end(),  
            simple_boundary.vertices.begin(), 
            simple_boundary.vertices.end());
        
        for(auto& line: simple_boundary.lines)
        {
            line.p1 += size;
            line.p2 += size;
        }

        lines.insert(
            lines.end(),  
            simple_boundary.lines.begin(), 
            simple_boundary.lines.end());
    }

    void SimpleBoundary::ReplaceElement(t_vert_pos vertice_pos, SimpleBoundary simple_boundary)
    {
        if(simple_boundary.vertices.empty())
            return;
        
        if(vertice_pos > vertices.size())
            throw Exception("SimpleBoundary: trying to insert at position out of boundaries");

        //remove target element
        if(!vertices.empty())
            vertices.erase(vertices.begin() + vertice_pos);

        vertices.insert(
            vertices.begin() + vertice_pos,  
            simple_boundary.vertices.begin(), 
            simple_boundary.vertices.end());

        auto shift = simple_boundary.vertices.size() - 1;
        for(auto& line: lines)
            if (line.p1 >= vertice_pos && line.p2 >= vertice_pos)
            {
                line.p1 += shift;
                line.p2 += shift;
            }
            else if(line.p1 < vertice_pos && line.p2 > vertice_pos)
                line.p2 += shift;
            else if(line.p2 < vertice_pos && line.p1 > vertice_pos)
                line.p1 += shift;

        for(auto& line: simple_boundary.lines)
        {
            line.p1 += vertice_pos;
            line.p2 += vertice_pos;
        }

        lines.insert(
            lines.begin() + vertice_pos,  
            simple_boundary.lines.begin(), 
            simple_boundary.lines.end());
    }

    bool SimpleBoundary::operator==(const SimpleBoundary &simple_boundary) const
    {
        return vertices == simple_boundary.vertices 
            && lines == simple_boundary.lines
            && sources == simple_boundary.sources
            && inner_boundary == simple_boundary.inner_boundary
            && hole == simple_boundary.hole
            && name == simple_boundary.name;
    }

    ostream& operator <<(ostream& write, const SimpleBoundary & boundary)
    {
        write << "Name: " << boundary.name << endl;

        write << "inner boudary: " << boundary.inner_boundary << endl;
        if(boundary.inner_boundary)
            write << "Hole point: " << boundary.hole << endl;

        write << "Vertices:" << endl;
        for(const auto& v: boundary.vertices)
            write << v << endl;

        write << "Lines:" << endl;
        for(const auto& l: boundary.lines)
            write << l << endl;

        write << "Sources:" << endl;
        for(const auto& [id, pos]: boundary.sources)
            write << "id: " << id << ", vertice: " << pos << endl;

        return write;
    }

    void Boundaries::Check()
    {
        if (this->empty())
            throw Exception("Boundary is empty.");
        
        t_vert_pos 
            counter = 0, 
            outer_boudaries = 0;
        vector<t_sources_ids> sources_ids;

        for(const auto& boundary: *this)
        {
            if(boundary.vertices.size() != boundary.lines.size())
                throw Exception("Vertices and lines sizes, are different in boudary: " + to_string(counter));
            
            auto vertices_num = boundary.vertices.size();
    
            for(const auto& [source_id, vertice_pos]: boundary.sources)
            {
                if (vertice_pos >= vertices_num )
                    throw Exception("Vertice position is boigger then number of vertices: source id " + to_string(source_id));
                sources_ids.push_back(source_id);
            }

            if(!boundary.inner_boundary)
                ++outer_boudaries;

            ++counter;
        }

        if(outer_boudaries == 0)
            throw Exception("There is no outer boudaries!");
        else if(outer_boudaries > 1)
            throw Exception("Only one outer boudary is allowed: " + to_string(outer_boudaries));

        auto num_of_sources = sources_ids.size();

        //deletion of duplicates, if there are so
        sort( sources_ids.begin(), sources_ids.end() );
        sources_ids.erase( 
            unique( sources_ids.begin(), sources_ids.end() ), sources_ids.end() );
        
        if(num_of_sources != sources_ids.size())
            throw Exception("Boundary: Sources ids should be unique numbers!");
    }

    vector<t_sources_ids> Boundaries::GetSourcesIds() const
    {
        vector<t_sources_ids> sources_ids;
        for(const auto& simple_boundary: *this)
            for(const auto& [source_id, vertice_pos]: simple_boundary.sources)
                sources_ids.push_back(source_id);

        return sources_ids;
    }

    vector<Point> Boundaries::GetHolesList() const
    {
        vector<Point> holes;
        for(const auto& simple_boundary: *this)
            if(simple_boundary.inner_boundary)
                holes.push_back(simple_boundary.hole);

        return holes;
    }

    void Boundaries::MakeRectangular(double width, double height, double source_x_position)
    {
        this->push_back(
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
                {{1, 1}}/*sources*/, 
                false/*this is not inner boudary*/,
                {}/*hole*/,
                "outer rectangular boudary"
            }/*Outer Boundary*/
        );
    }

    void Boundaries::MakeRectangularWithHole(double width, double height, double source_x_position)
    {
        this->push_back(
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
                {{1, 1}}/*sources*/, 
                false/*this is not inner boudary*/,
                {}/*hole*/,
                "outer boudary"/*just name*/
            }
        );

        this->push_back(
            {/*Hole Boundary*/
                {/*vertices*/
                    {0.25*width, 0.25*height},
                    {0.75*width, 0.25*height}, 
                    {0.75*width, 0.75*height}, 
                    {0.25*width, 0.75*height}
                }, 
                {/*lines*/
                    {0, 1, 6},
                    {1, 2, 7},
                    {2, 3, 8},
                    {3, 0, 9} 
                }, 
                {}/*sources*/, 
                true/*this is inner boudary*/,
                {0.5*width, 0.5*height}/*hole*/,
                "hole"/*just name*/
            });

        this->push_back(
            {/*Hole Boundary*/
                {/*vertices*/
                    {0.8*width, 0.8*height},
                    {0.9*width, 0.8*height}, 
                    {0.9*width, 0.9*height}, 
                    {0.8*width, 0.9*height}
                }, 
                {/*lines*/
                    {0, 1, 10},
                    {1, 2, 11},
                    {2, 3, 12},
                    {3, 0, 13} 
                }, 
                {}/*sources*/, 
                true/*this is inner boudary*/,
                {0.85*width, 0.85*height}/*hole*/,
                "hole"/*just name*/
            });
    }

    SimpleBoundary& Boundaries::GetOuterBoundary()
    {
        for(auto& simple_boundary: *this)
            if(!simple_boundary.inner_boundary)
                return simple_boundary;

        throw Exception("Boundary: There is no Outer Boundary");

        return this->at(0);
    }

    pair<t_vert_pos, t_vert_pos> Boundaries::GetAdjacentVerticesPositions(
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

    double Boundaries::NormalAngle(const Point& left, const Point& center, const Point& right)
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

    double Boundaries::GetVerticeNormalAngle(const vector<Point>& vertices, const t_vert_pos vertice_pos, bool inner_boundary)
    {
        const auto vertices_size = vertices.size();
        const auto [vertice_pos_left, vertice_pos_right] = GetAdjacentVerticesPositions(vertices_size, vertice_pos);

        const auto& p_source = vertices.at(vertice_pos), 
            p_left = vertices.at(vertice_pos_left), 
            p_right = vertices.at(vertice_pos_right);
        
        auto normal_angle = NormalAngle(p_left, p_source, p_right);
        if(inner_boundary)
            normal_angle += M_PI;
        
        if(normal_angle >= 2*M_PI)
            normal_angle -= 2*M_PI;
        
        return normal_angle;
    }

    Boundaries::trees_interface_t Boundaries::GetSourcesIdsPointsAndAngles() const
    {
        trees_interface_t sources_ids_points_and_angles;
        for(const auto& boudary: *this)
            for(const auto& [id, v_pos]: boudary.sources)
                sources_ids_points_and_angles[id] = {
                    boudary.vertices.at(v_pos),
                    GetVerticeNormalAngle(boudary.vertices, v_pos, boudary.inner_boundary)
                };

        return sources_ids_points_and_angles;
    }

    ostream& operator <<(ostream& write, const Boundaries & boundary)
    {
        for(const auto& simple_boundary:  boundary)
            write << simple_boundary << endl;

        return write;
    }
}