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
    }

    Boundary Boundary::generateSmoothBoundary(const double min_degree, const double ignored_distance) const
    {
        if(ignored_distance < 0)
            throw Exception("Boundary::generateSmoothBoundary: ignored_distance smoothnes param should be greater or equal then zero.");

        //just make a copy if min_degree is zero. Cos thats mean there is no     
        if (abs(min_degree) < EPS || vertices.size() <= 4)
            return Boundary{vertices, lines};

        if(min_degree < 0)
            throw Exception("Boundary::generateSmoothBoundary: min_degree smoothnes param should be greater or equal then zero.");

        Boundary smooth_boundary;
        
        //first point is ancor..
        smooth_boundary.vertices.push_back(vertices.back());
        smooth_boundary.lines.push_back(lines.back());

        double 
            accum_degree = 0.,
            accum_ds = 0.;
        for(size_t i = vertices.size() - 2; i > 0; --i)
        {
            auto 
                p0 = vertices[i + 1],
                p1 = vertices[i],
                p2 = vertices[i - 1],

                v1 = p1 - p0,
                v2 = p2 - p1;

            auto 
                n1 = v1.norm();

            accum_degree += v1.angle(v2) * 180. / M_PI;
            accum_ds += n1;

            if (abs(accum_degree) >= min_degree - EPS/*reduce some numerical noise*/ || accum_ds <= ignored_distance)
            {
                accum_degree = 0.;
                smooth_boundary.vertices.push_back(p1);
                smooth_boundary.lines.push_back(lines[i]);
            }
        }

        //and last point is also ancor..
        smooth_boundary.vertices.push_back(vertices.front());
        smooth_boundary.lines.push_back(lines.front());

        //cos this function makes smoothnes from the end, now we need to reverse it
        reverse(smooth_boundary.vertices.begin(), smooth_boundary.vertices.end());
        reverse(smooth_boundary.lines.begin(), smooth_boundary.lines.end());
        

        smooth_boundary.FixLinesIndices(lines.back().p2 == 0);

        return smooth_boundary;
    }

    void Boundary::FixLinesIndices(bool is_closed_boundary)
    {
        if (vertices.empty())
        {
            lines.clear();
            return;
        }

        unsigned index = 0;
        auto temp_lines = lines;
        if(!is_closed_boundary)
        {
            lines.resize(vertices.size() - 1);
            for(auto& line: lines)
            {
                line.boundary_id = temp_lines.at(index).boundary_id;
                line.p1 = index;
                line.p2 = ++index;
            }
        }
        else
        {
            lines.resize(vertices.size());
            for(auto& line: lines)
            {
                line.boundary_id = temp_lines.at(index).boundary_id;
                line.p1 = index;
                line.p2 = ++index;
            }
            lines.back().p2 = 0;
        }
    }

    bool Boundary::operator==(const Boundary &boundary) const
    {
        return vertices == boundary.vertices 
            && lines == boundary.lines;
    }

    ostream& operator <<(ostream& write, const Boundary & boundary)
    {
        write << "Vertices:" << endl;
        for(const auto& v: boundary.vertices)
            write << v << endl;

        write << "Lines:" << endl;
        for(const auto& l: boundary.lines)
            write << l << endl;

        return write;
    }
}