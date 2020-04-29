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
   //todo can we remove this? is there default copy constructor?
    Tree::Tree(const Tree& t)
    {
        for(const auto&[branch_id, branch]: t)
            (*this)[branch_id] = branch;

        this->branches_relation = t.branches_relation;
    }

    //todo can we remove this? is there default assignment function?
    Tree& Tree::operator=(const Tree &t)
    {
        for(const auto&[branch_id, branch]: t)
            (*this)[branch_id] = branch;

        this->branches_relation = t.branches_relation;

        return *this;
    }

    //todo can we remove this? is there default comparison function?
    bool Tree::operator== (const Tree &t) const
    {
        return (this->branches_relation == t.branches_relation) && 
            equal(this->begin(), this->end(), t.begin());
    }

    void Tree::Initialize(const Boundaries::trees_interface_t ids_points_angles)
    {
        Clear();
        for(auto &[id, point_angle]: ids_points_angles)
            AddBranch(
                BranchNew{point_angle.first, point_angle.second}, 
                id);
    }

    t_branch_id Tree::AddBranch(const BranchNew &branch, t_branch_id id)
    {
        if(id == UINT_MAX)
            id = GenerateNewID();
        
        if(this->count(id))
            throw Exception("Can't add branch. Such branch already exist: " + to_string(id));

        if(!IsValidBranchId(id))
            throw Exception("Invalid Id, id should be greater then 0. Id value: " + to_string(id));
                
        (*this)[id] = branch;

        return id;
    }
   
    pair<t_branch_id, t_branch_id> Tree::AddSubBranches(t_branch_id root_branch_id, 
        const BranchNew &left_branch, const BranchNew &right_branch)
    {   
        handle_non_existing_branch_id(root_branch_id);

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
        handle_non_existing_branch_id(branch_id);
        
        for(const auto&[parent_id, sub_ids]: branches_relation)
            if(sub_ids.first == branch_id 
                || sub_ids.second == branch_id)
                return parent_id;

        throw Exception("Branch doesn't have source branch. probabaly it is source itself");

        return 0;
    }

    t_branch_id Tree::GetAdjacentBranchId(t_branch_id sub_branch_id) const
    {
        handle_non_existing_branch_id(sub_branch_id);

        auto [left_branch, right_branch] = GetSubBranchesIds(GetParentBranchId(sub_branch_id));

        if(left_branch == sub_branch_id)
            return right_branch;
        else if(right_branch == sub_branch_id)
            return left_branch;
        else
            throw Exception("something wrong with GetAdjacentBranch");
    }

    //todo can input arguments be replaced by map?
    void Tree::AddPoints(const vector<t_branch_id>& tips_id, const vector<Point>& points)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(this->count(tips_id.at(i)))
            {
                auto& br = this->at(tips_id.at(i));
                br.AddPoint(points.at(i));
            }
            else
                throw Exception("AddPoints: no such id.");
            
    }

    t_branch_id Tree::GenerateNewID() const
    {
        t_branch_id max_id = 1;
                
        vector<t_branch_id> branches_id;
        branches_id.reserve(this->size());

        for(const auto &[branch_id, branch]: *this)
            branches_id.push_back(branch_id);

        sort(branches_id.begin(), branches_id.end()); 

        for(const auto& id: branches_id)
        {
            if(id != max_id)
                return max_id;
            else 
                max_id++;
        }

        return max_id;
    }

    void Tree::AddPolars(const vector<t_branch_id>& tips_id, const vector<Polar> &points)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(this->count(tips_id.at(i)))
            {
                auto& br = this->at(tips_id.at(i));
                br.AddPoint(points.at(i));
            }
            else 
                throw Exception("AddPoints: no such id.");
    }

    void Tree::AddAbsolutePolars(const vector<t_branch_id>& tips_id, const vector<Polar>& points)
    {
        for(size_t i = 0; i < tips_id.size(); ++i)
            if(this->count(tips_id.at(i)))
            {
                auto& br = this->at(tips_id.at(i));
                br.AddAbsolutePoint(br.TipPoint() + points.at(i));
            }
            else 
                throw Exception("AddPoints: no such id.");
    }

    void Tree::DeleteSubBranches(t_branch_id root_branch_id)
    {
        handle_non_existing_branch_id(root_branch_id);

        if(!HasSubBranches(root_branch_id))
            throw Exception("This branch doesn't have subbranches.");

        auto[sub_left, sub_right] = GetSubBranchesIds(root_branch_id);
        
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
    }

    pair<t_branch_id, t_branch_id> Tree::GrowTestTree(t_branch_id branch_id, double ds, unsigned n, double dalpha)
    {
        handle_non_existing_branch_id(branch_id);
        
        if(HasSubBranches(branch_id))
            throw Exception("GrowTestTree: This branch have subbranches.");

        auto& 
            branch_source = this->at(branch_id);
        for(unsigned i = 0; i < n; ++i)
        {
            auto p = Polar{ds, dalpha};
            branch_source.AddPoint(p);
        }

        auto branch_left = BranchNew{
                branch_source.TipPoint(), 
                branch_source.TipAngle() + M_PI/4.},
            branch_right = BranchNew{
                branch_source.TipPoint(), 
                branch_source.TipAngle() - M_PI/4.};

        for(unsigned i = 0; i < n; ++i)
        {
            auto p = Polar{ds, dalpha};
            branch_left.AddPoint(p);
            branch_right.AddPoint(p);
        }
        
        auto ids = AddSubBranches(branch_id, branch_left, branch_right);

        return ids;
    }

    vector<t_branch_id> Tree::TipBranchesIds() const
    {
        vector<t_branch_id> tip_branches_ids;

        for(const auto&[branch_id, branch]: *this)
            if(!HasSubBranches(branch_id))
                tip_branches_ids.push_back(branch_id);

        return tip_branches_ids;
    }

    vector<t_branch_id> Tree::zero_lenght_tip_branches_ids(double zero_lenght) const
    {
        vector<t_branch_id> zero_lenght_branches_id;
        for (const auto id: TipBranchesIds())
            if(HasParentBranch(id) && this->at(id).Lenght() <= zero_lenght)
                zero_lenght_branches_id.push_back(GetParentBranchId(id));

        sort(zero_lenght_branches_id.begin(), zero_lenght_branches_id.end()); 
        
        zero_lenght_branches_id.erase( 
            unique(zero_lenght_branches_id.begin(), zero_lenght_branches_id.end() ), 
            zero_lenght_branches_id.end());


        return zero_lenght_branches_id;
    }
    
    vector<Point> Tree::TipPoints() const
    {   
        vector<Point> tip_points;
        auto tip_branches_id = TipBranchesIds();
        tip_points.reserve(tip_branches_id.size());
        for(auto id: tip_branches_id)
            tip_points.push_back(this->at(id).TipPoint());
                
        return tip_points;
    }

    map<t_branch_id, Point> Tree::TipIdsAndPoints() const
    {   
        map<t_branch_id, Point> ids_points_map;
        auto points = TipPoints();
        auto ids = TipBranchesIds();

        for(size_t i = 0; i < ids.size(); ++i)
            ids_points_map[ids.at(i)] = points.at(i);
                
        return ids_points_map;
    }

    double Tree::maximal_tip_curvature_distance() const
    {
        double max_dist = 0;
        for(auto id: TipBranchesIds())
        {
            auto branch = this->at(id);
            auto tip_point_pos = branch.size() - 1;

            if(tip_point_pos < 2)
                break;
                
            auto 
                av_midle_point = (branch.at(tip_point_pos) + branch.at(tip_point_pos - 2))/2,
                midle_point = branch.at(tip_point_pos - 1);

            auto dist = (av_midle_point - midle_point).norm();

            if(dist > max_dist)
                max_dist = dist;
        }

        return max_dist;
    }

    void Tree::flatten_tip_curvature()
    {
        for(auto id: TipBranchesIds())
        {
            auto& branch = this->at(id);
            auto tip_p_pos = branch.size() - 1;
            if(tip_p_pos < 2)
                throw Exception("flatten_tip_curvature: size of branch should be at least three points");

            auto av_midle_point = (branch.at(tip_p_pos) + branch.at(tip_p_pos - 2))/2;

            branch.at(tip_p_pos - 1) = av_midle_point;
        }
    }

    void Tree::remove_tip_points()
    {
        vector<t_branch_id> zero_lenght_branches;
        for (auto id: TipBranchesIds())
        {
            auto& branch = this->at(id);

            if(branch.size() >= 2)
                branch.pop_back();

            if (branch.size() == 1)
                zero_lenght_branches.push_back(id);
        }

        for(auto id: zero_lenght_branches)
            if (this->count(id) && HasParentBranch(id))
                DeleteSubBranches(GetParentBranchId(id));
    }
    
    ostream& operator<<(ostream& write, const Tree & b)
    {
        write << "branches relations: " << endl;
        for(auto key_val: b.branches_relation)
            write << key_val.first << " left: " << key_val.second.first 
                << " right: " << key_val.second.second << endl;

        write << "branches: " << endl;
        for(const auto& br: b)
            write << br.second << endl;
        
        return write;
    }
}