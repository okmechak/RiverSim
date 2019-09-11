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

///\cond
#include <fstream>
///\endcond

namespace River
{   
    /*
        Branch Class
    */
    BranchNew& BranchNew::Shrink(double lenght)
    {
        while(lenght > 0 && Lenght() > 0)
        {
            try{
                
                auto tip_lenght = TipVector().norm();
                if(lenght < tip_lenght - eps)
                {
                    auto k = 1 - lenght/tip_lenght;
                    auto new_tip = TipVector()*k;
                    RemoveTipPoint();
                    AddPoint(new_tip);
                    lenght = 0;
                }
                else if((lenght >= tip_lenght - eps) && (lenght <= tip_lenght + eps))
                {
                    RemoveTipPoint();
                    lenght = 0;
                }
                else if(lenght >= tip_lenght + eps)
                {
                    RemoveTipPoint();
                    lenght -= tip_lenght;
                }
                else 
                    throw invalid_argument("Unhandled case in Shrink method.");
            }
            catch(const invalid_argument& e)
            {
                cerr << e.what() << '\n';
                throw invalid_argument("Shrinikng error: problem with RemoveTipPoint() or TipVector()");
            }
        }
        
        return *this;
    }
    
    
    /*
        Tree Class
    */
    Tree::Tree(const Tree& t)
    {
        source_branches_id = t.source_branches_id;
        branches_relation = t.branches_relation;

        for(auto[id, br]: t.branches_index)
        {
            branches.push_back(*br);
            branches_index[id] = &branches.back();
        }
    }

    Tree& Tree::operator=(const Tree &t)
    {
        source_branches_id = t.source_branches_id;
        branches_relation = t.branches_relation;

        for(auto[id, br]: t.branches_index)
        {
            branches.push_back(*br);
            branches_index[id] = &branches.back();
        }

        return *this;
    }

    Tree& Tree::Initialize(vector<Point> sources_point, vector<double> sources_angle, vector<int> ids)
    {
        Clear();
        for(unsigned int i = 0; i < ids.size(); ++i)
            AddSourceBranch(BranchNew{
                    sources_point.at(i), 
                    sources_angle.at(i)}, 
                    ids.at(i));
        return *this;
    }

    
    int Tree::AddBranch(const BranchNew &branch, int id)
    {
        if(id == -999)
            id = GenerateNewID();
        
        if(DoesExistBranch(id))
        {
            stringstream ss;
            ss << endl;
            for(auto[key, val]: branches_index)
                ss << key << ": " << *val << endl;
            ss << "Invalid Id, such branch already exists. Id value: " << id << endl;
            ss << "inserted branch: " << endl;
            ss << branch;
            throw invalid_argument(ss.str());
        }

        if(!IsValidBranchId(id))
            throw invalid_argument("Invalid Id, id should be greater then 0. Id value: " + to_string(id));
                
        branches.push_back(branch);
        branches_index[id] = &branches.back();

        return id;
    }
   
   
    pair<int, int> Tree::AddSubBranches(int root_branch_id, BranchNew &left_branch, BranchNew &right_branch)
    {   
        if(!DoesExistBranch(root_branch_id))
            throw invalid_argument("Root branch doesn't exis");

        if(HasSubBranches(root_branch_id))
            throw invalid_argument("This branch already has subbranches");
                
        //adding new branches
        pair<int, int> sub_branches_id;

        sub_branches_id.first = GenerateNewID();
        AddBranch(left_branch, sub_branches_id.first);
                
        sub_branches_id.second = GenerateNewID();
        AddBranch(right_branch, sub_branches_id.second);

        //setting relation
        branches_relation[root_branch_id] = sub_branches_id;

        return sub_branches_id;
    }


    int Tree::GetParentBranchId(int branch_id) const
    {
        if(HasParentBranch(branch_id))
        {
            for(auto key_val: branches_relation)
                if(key_val.second.first == branch_id 
                    || key_val.second.second == branch_id)
                    return key_val.first;
        }
        else
            throw invalid_argument("Branch doesn't have source branch. probabaly it is source itself");

        return invalid_branch;
    }


    int Tree::GetAdjacentBranchId(int sub_branch_id) const
    {
        auto [left_branch, right_branch] = GetSubBranchesId(GetParentBranchId(sub_branch_id));

        if(left_branch == sub_branch_id)
            return right_branch;
        else if(right_branch == sub_branch_id)
            return left_branch;
        else
            throw invalid_argument("something wrong with GetAdjacentBranch");
    }


    Tree& Tree::AddPoints(const vector<Point>& points, const vector<int>& tips_id)
    {
        for(unsigned i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddPoint(points.at(i));
            }
            else
                throw invalid_argument("Such branch does not exist");

        return *this;
    }


    unsigned int Tree::GenerateNewID() const
    {
        unsigned max_id = 1;
                
        vector<int> branches_id;
        branches_id.reserve(branches_index.size());

        for(auto &key_val: branches_index)
            branches_id.push_back(key_val.first);

        while(find(begin(branches_id), end(branches_id), max_id)!= branches_id.end())
            max_id += 1;

        return max_id;
    }


    const BranchNew* Tree::GetBranch(int id) const
    {
        if(!DoesExistBranch(id))
            throw invalid_argument("there is no such branch");
                
        return branches_index.at(id);
    }


    BranchNew* Tree::GetBranch(int id)
    {
        if(!DoesExistBranch(id))
            throw invalid_argument("there is no such branch");
                
        return branches_index.at(id);
    }


    Tree& Tree::AddPolars(const vector<Polar> &points, const vector<int>& tips_id)
    {
        for(unsigned i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddPoint(points.at(i));
            }
            else
                throw invalid_argument("Such branch does not exist");

        return *this;
    }


    Tree& Tree::AddAbsolutePolars(const vector<Polar>& points, const vector<int>& tips_id)
    {
        for(unsigned int i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddAbsolutePoint(br->TipPoint() + points.at(i));
            }
            else
                throw invalid_argument("Such branch does not exist");
        
        return *this;
    }



    Tree& Tree::DeleteSubBranches(int root_branch_id)
    {
        if(!DoesExistBranch(root_branch_id))
            throw invalid_argument("root branch doesn't exis.");

        if(!HasSubBranches(root_branch_id))
            throw invalid_argument("This branch doesn't has subbranches.");

        auto[sub_left, sub_right] = GetSubBranchesId(root_branch_id);
        
        //recursively look up for left branch
        if(HasSubBranches(sub_left))
            DeleteSubBranches(sub_left);
        DeleteBranch(sub_left);

        //recursively look up for right branch
        if(HasSubBranches(sub_right))
            DeleteSubBranches(sub_right);
        DeleteBranch(sub_right);

        //delete relation
        branches_relation.erase(root_branch_id);
                
        return *this;
    }


    vector<int> Tree::TipBranchesId() const
    {
        vector<int> tip_branches_id;
        for(auto p: branches_index)
            if(!HasSubBranches(p.first))
                tip_branches_id.push_back(p.first);

        return tip_branches_id;
    }
    

    vector<Point> Tree::TipPoints() const
    {   
        vector<Point> tip_points;
        auto tip_branches_id = TipBranchesId();
        tip_points.reserve(tip_branches_id.size());
        for(auto id: tip_branches_id)
            tip_points.push_back(GetBranch(id)->TipPoint());
                
        return tip_points;
    }


    map<int, Point> Tree::TipIdsAndPoints() const
    {   
        map<int, Point> ids_points_map;
        auto points = TipPoints();
        auto ids = TipBranchesId();

        for(unsigned i = 0; i < ids.size(); ++i)
            ids_points_map[ids.at(i)] = points.at(i);
                
        return ids_points_map;
    }

    
    ostream& operator<<(ostream& write, const Tree & b)
    {
        write << "source branches ids" << endl;
        for(auto s_id: b.source_branches_id)
            write << s_id << ", ";
        write << endl;

        write << "branches relation" << endl;
        for(auto key_val: b.branches_relation)
            write << key_val.first << " left: " << key_val.second.first 
                << " right: " << key_val.second.second << endl;
        
        return write;
    }




    /*
        Tree Vector Generation
    */
    void TreeVector(vector<Point> &tree_vector, int id, const Tree& tr, double eps)
    {
        const auto br = tr.GetBranch(id);
        vector<Point> left_vector, right_vector;
        left_vector.reserve(br->Size());
        right_vector.reserve(br->Size());

        for(unsigned i = 0; i < br->Size(); ++i)
        {
            if(i == 0)
            {
                left_vector.push_back(
                    br->GetPoint(i) + Point{Polar{eps/2, br->SourceAngle() + M_PI/2}});
                right_vector.push_back(
                    br->GetPoint(i) + Point{Polar{eps/2, br->SourceAngle() - M_PI/2}});
            }
            else
            {
                left_vector.push_back(
                    br->GetPoint(i) + br->Vector(i).normalize().rotate(M_PI/2)*eps/2);
                right_vector.push_back(
                    br->GetPoint(i) + br->Vector(i).normalize().rotate(-M_PI/2)*eps/2);
            }
        }


        if(tr.HasSubBranches(id))
        {
            auto[left_b_id, right_b_id] = tr.GetSubBranchesId(id);
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
            tree_vector.push_back(br->GetPoint(br->Size() - 1));
            tree_vector.insert(end(tree_vector), 
                right_vector.rbegin() + 1, right_vector.rend());
        }
    }

    tethex::Mesh BoundaryGenerator(const Model& mdl, const Tree& tree, const Border &br)
    {
        vector<tethex::Point> tet_vertices;
        vector<tethex::MeshElement *> tet_lines, tet_triangles;

        auto m = br.SourceByVerticeIdMap();
        auto vertices = br.GetVertices();
        auto lines = br.GetLines();
        
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
                    if(line.p2 > i)
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