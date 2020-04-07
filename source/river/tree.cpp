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
#include <sstream>
#define _USE_MATH_DEFINES
#include <cmath>
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
                    throw Exception("Unhandled case in Shrink method.");
            }
            catch(const exception& e)
            {
                cerr << e.what() << '\n';
                throw Exception("Shrinikng error: problem with RemoveTipPoint() or TipVector()");
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

    bool Tree::operator== (const Tree &t) const
    {
        for(auto&[id, p_br]: branches_index)
        {
            if(t.branches_index.count(id))
            {
                if(!(*branches_index.at(id) == *t.branches_index.at(id)))
                    return false;
            }
            else 
                return false;
        }
        return t.branches_relation == branches_relation;
    }

    void Tree::Initialize(const Boundaries::trees_interface_t ids_points_angles)
    {
        Clear();
        for(auto &[id, point_angle]: ids_points_angles)
            AddSourceBranch(
                BranchNew{point_angle.first, point_angle.second}, 
                id);
    }

    unsigned Tree::AddBranch(const BranchNew &branch, t_branch_id id)
    {
        if(id == UINT_MAX)
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
            throw Exception(ss.str());
        }

        if(!IsValidBranchId(id))
            throw Exception("Invalid Id, id should be greater then 0. Id value: " + to_string(id));
                
        branches.push_back(branch);
        branches_index[id] = &branches.back();

        return id;
    }
   
    pair<t_branch_id, t_branch_id> Tree::AddSubBranches(t_branch_id root_branch_id, BranchNew &left_branch, BranchNew &right_branch)
    {   
        if(!DoesExistBranch(root_branch_id))
            throw Exception("Root branch doesn't exis");

        if(HasSubBranches(root_branch_id))
            throw Exception("This branch already has subbranches");
                
        //adding new branches
        pair<t_branch_id, t_branch_id> sub_branches_id;

        sub_branches_id.first = GenerateNewID();
        AddBranch(left_branch, sub_branches_id.first);
                
        sub_branches_id.second = GenerateNewID();
        AddBranch(right_branch, sub_branches_id.second);

        //setting relation
        branches_relation[root_branch_id] = sub_branches_id;

        return sub_branches_id;
    }

    t_branch_id Tree::GetParentBranchId(t_branch_id branch_id) const
    {
        if(HasParentBranch(branch_id))
        {
            for(auto key_val: branches_relation)
                if(key_val.second.first == branch_id 
                    || key_val.second.second == branch_id)
                    return key_val.first;
        }
        else
            throw Exception("Branch doesn't have source branch. probabaly it is source itself");

        return UINT_MAX;
    }

    t_branch_id Tree::GetAdjacentBranchId(t_branch_id sub_branch_id) const
    {
        auto [left_branch, right_branch] = GetSubBranchesId(GetParentBranchId(sub_branch_id));

        if(left_branch == sub_branch_id)
            return right_branch;
        else if(right_branch == sub_branch_id)
            return left_branch;
        else
            throw Exception("something wrong with GetAdjacentBranch");
    }

    Tree& Tree::AddPoints(const vector<Point>& points, const vector<t_branch_id>& tips_id)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddPoint(points.at(i));
            }
            else
                throw Exception("Such branch does not exist");

        return *this;
    }

    t_branch_id Tree::GenerateNewID() const
    {
        t_branch_id max_id = 1;
                
        vector<t_branch_id> branches_id;
        branches_id.reserve(branches_index.size());

        for(auto &key_val: branches_index)
            branches_id.push_back(key_val.first);

        while(find(begin(branches_id), end(branches_id), max_id)!= branches_id.end())
            max_id += 1;

        return max_id;
    }

    const BranchNew* Tree::GetBranch(t_branch_id id) const
    {
        if(!DoesExistBranch(id))
            throw Exception("there is no such branch");
                
        return branches_index.at(id);
    }

    BranchNew* Tree::GetBranch(t_branch_id id)
    {
        if(!DoesExistBranch(id))
            throw Exception("there is no such branch");
                
        return branches_index.at(id);
    }

    Tree& Tree::AddPolars(const vector<Polar> &points, const vector<t_branch_id>& tips_id)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddPoint(points.at(i));
            }
            else
                throw Exception("Such branch does not exist");

        return *this;
    }

    Tree& Tree::AddAbsolutePolars(const vector<Polar>& points, const vector<t_branch_id>& tips_id)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(DoesExistBranch(tips_id.at(i)))
            {
                auto br = GetBranch(tips_id.at(i));
                br->AddAbsolutePoint(br->TipPoint() + points.at(i));
            }
            else
                throw Exception("Such branch does not exist");
        
        return *this;
    }

    Tree& Tree::DeleteSubBranches(t_branch_id root_branch_id)
    {
        if(!DoesExistBranch(root_branch_id))
            throw Exception("root branch doesn't exis.");

        if(!HasSubBranches(root_branch_id))
            throw Exception("This branch doesn't have subbranches.");

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

    pair<t_branch_id, t_branch_id> Tree::GrowTestTree(t_branch_id branch_id, double ds, unsigned n, double dalpha)
    {
        if(!DoesExistBranch(branch_id))
            throw Exception("GrowTestTree: root branch doesn't exis.");
        
        if(HasSubBranches(branch_id))
            throw Exception("GrowTestTree: This branch have subbranches.");

        auto 
            branch_source = GetBranch(branch_id);

        auto branch_left = BranchNew{
                branch_source->TipPoint(), 
                branch_source->TipAngle() + M_PI/4.},
            branch_right = BranchNew{
                branch_source->TipPoint(), 
                branch_source->TipAngle() - M_PI/4.};

        for(unsigned i = 0; i < n; ++i)
        {
            auto p = Polar{ds, dalpha};
            branch_source->AddPoint(p);
            branch_left.AddPoint(p);
            branch_right.AddPoint(p);
        }
        
        auto ids = AddSubBranches(branch_id, branch_left, branch_right);

        return ids;
    }

    vector<t_branch_id> Tree::TipBranchesId() const
    {
        vector<t_branch_id> tip_branches_id;
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

    map<t_branch_id, Point> Tree::TipIdsAndPoints() const
    {   
        map<t_branch_id, Point> ids_points_map;
        auto points = TipPoints();
        auto ids = TipBranchesId();

        for(size_t i = 0; i < ids.size(); ++i)
            ids_points_map[ids.at(i)] = points.at(i);
                
        return ids_points_map;
    }
    
    ostream& operator<<(ostream& write, const Tree & b)
    {
        write << "source branches ids: " << endl;
        for(auto s_id: b.source_branches_id)
            write << s_id << ", ";
        write << endl;

        write << "branches relations: " << endl;
        for(auto key_val: b.branches_relation)
            write << key_val.first << " left: " << key_val.second.first 
                << " right: " << key_val.second.second << endl;

        write << "branches: " << endl;
        for(auto& br: b.branches)
            write << br << endl;
        
        return write;
    }
}