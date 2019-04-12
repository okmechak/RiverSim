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
    void TreeVector(vector<Point> &tree_vector, int id, const Tree& tr, double eps)
    {
        const BranchNew& br = tr.GetBranch(id);
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

    tethex::Mesh BoundaryGenerator(const Model& mdl, const Tree& tree, const Border &br)
    {
        vector<tethex::Point> tet_vertices;
        vector<tethex::MeshElement *> tet_lines, tet_triangles;
        //TODO reserve size.

        auto m = br.SourceByVerticeIdMap();
        auto lines = br.lines;
        auto vertices = br.vertices;
        
        for(long unsigned i = 0; i < vertices.size(); ++i)
        {
            if(!m.count(i))
            {
                tet_vertices.push_back({vertices.at(i).x, vertices.at(i).y});
                tet_lines.push_back(new tethex::Line(lines.at(i).p1, lines.at(i).p2, lines.at(i).id));
            }
            else
            {
                vector<Point> tree_vector;
                TreeVector(tree_vector, m.at(i)/*source id*/, tree, mdl.mesh.eps);
                long unsigned shift = tet_vertices.size(); 
                for(long unsigned i = 0; i < tree_vector.size(); ++i)
                {
                    tet_vertices.push_back({tree_vector.at(i).x, tree_vector.at(i).y});
                    tet_lines.push_back(
                        new tethex::Line(
                            shift + i, 
                            shift + i + 1, 
                            mdl.river_boundary_id));
                }
                for(auto& line: lines)
                    if(line.p2 > i)//TODO Test this
                    {
                        line.p1 += tree_vector.size();
                        line.p2 += tree_vector.size();
                    }
            }
        }
        //close line loop
        tet_lines.back()->set_vertex(1, 0);

        return tethex::Mesh{tet_vertices, tet_lines, tet_triangles};
    }
}