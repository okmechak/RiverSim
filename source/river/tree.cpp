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

#include "tree.hpp"

#include <fstream>

namespace River
{   
    /*
        Branch Class
    */
    BranchNew& BranchNew::Shrink(double lenght)
    {
        while(lenght > 0)
        {
            auto dl = TipVector().norm();
            if(lenght >= dl)
                RemoveTipPoint();
            else 
            {
                auto k = lenght/dl;
                auto new_tip = TipVector()*k;
                RemoveTipPoint();
                AddPoint(new_tip);
                lenght = 0;
            }
            lenght -= dl;
        }
        

        return *this;
    }
    

    /*
        Tree Vector Generation
    */
    void TreeVector(vector<Point> &tree_vector, int id, Tree& tr, double eps)
    {
        BranchNew& br = tr.GetBranch(id);
        vector<Point> left_vector, right_vector;
        left_vector.reserve(br.Size());
        right_vector.reserve(br.Size());

        for(unsigned i = 0; i < br.Size(); ++i)
        {
            if(i == 0)
            {
                left_vector.push_back(
                    br.GetPoint(i) + Point{Polar{eps/2, br.SourceAngle() + M_PI/2}});
                right_vector.push_back(
                    br.GetPoint(i) + Point{Polar{eps/2, br.SourceAngle() - M_PI/2}});
            }
            else
            {
                left_vector.push_back(
                    br.GetPoint(i) + br.Vector(i).normalize().rotate(M_PI/2)*eps/2);
                right_vector.push_back(
                    br.GetPoint(i) + br.Vector(i).normalize().rotate(-M_PI/2)*eps/2);
            }
        }


        if(tr.HasSubBranches(id))
        {
            auto[left_b_id, right_b_id] = tr.GetSubBranches(id);
            tree_vector.insert(end(tree_vector), 
                left_vector.begin(), left_vector.end() - 1);
            TreeVector(tree_vector, left_b_id, tr, eps);
            tree_vector.pop_back();
            TreeVector(tree_vector, right_b_id, tr, eps);
            tree_vector.insert(end(tree_vector), 
                right_vector.rbegin() + 1, right_vector.rend());
        }
        else
        {
            tree_vector.insert(end(tree_vector), 
                left_vector.begin(), left_vector.end() - 1);
            tree_vector.push_back(br.GetPoint(br.Size() - 1));
            tree_vector.insert(end(tree_vector), 
                right_vector.rbegin() + 1, right_vector.rend());
        }
    }

    void AppendTreeToMesh(vector<Point> tree_vector, int vertice_left, int vertice_right, int boundary_id, tethex::Mesh& mesh)
    {
        vector<tethex::Point> vertices;
        vertices.reserve(tree_vector.size());
        for(auto &el: tree_vector)
            vertices.push_back(tethex::Point{el.x, el.y});

        //FIXME: What if tree_vector has only one point. we should handle this cases

        auto first_index = mesh.get_n_vertices();
        mesh.append_vertexes(vertices);
        auto last_index = mesh.get_n_vertices() - 1;

        vector<tethex::MeshElement *> lines;
        lines.reserve(tree_vector.size() + 4/*some enough positive number cos lines number is a little bigger then points*/);
        lines.push_back(new tethex::Line(vertice_left, first_index, boundary_id));
        lines.push_back(new tethex::Line(vertice_right, last_index, boundary_id));
        for(unsigned i = first_index; i < last_index; ++i)
        {
            lines.push_back(new tethex::Line(i, i + 1, boundary_id));
        }
        mesh.append_lines(lines);
    }
    
    tethex::Mesh BoundaryGenerator(const Model& mdl, Tree& tr, Border &br)
    {
        tethex::Mesh boundary_mesh = br.borderMesh;

        for(auto source_id: br.GetSourcesId())
        {
            auto[vertice_left, vertice_right] = br.GetSourceVerticesIndexById(source_id);
            vector<Point> tree_vector;
            TreeVector(tree_vector, source_id, tr, mdl.eps);
            
            if(tree_vector.size() >= 3)
            {
                //if we do not delete first and last point it will coincidice with  border points
                tree_vector.erase(begin(tree_vector));
                tree_vector.erase(end(tree_vector));
            }
            AppendTreeToMesh(tree_vector, vertice_left, vertice_right, br.river_boundary_id, boundary_mesh);
            tree_vector.clear();
        }

        return boundary_mesh;
    }
}