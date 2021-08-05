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

/*! \file tree.hpp
    \brief
    Holds all functionality that you need to work with tree(network) of river, 
    its separate branches and generation of final boundary geometry.

    \details
    File holds two classes: __River::Branch__ and __River::Tree__.
    As you can see from picture - River::Branch is simple array joined points - piecewice line, 
    aproximation of real river branch. River::Tree on other hand consist from combination of Branches.
    __Bifurcation point__ - where two rivers unions into one river. So each branch has its subbranches.
    Structure of branches and sub branches is represented here in class River::Tree.

    \sa River::Tree, River::Branch
 */

#pragma once

///\cond
#include <climits>
///\endcond

#include "boundary.hpp"

using namespace std;

namespace River
{
    typedef t_source_id t_branch_id;
    typedef pair<t_branch_id, t_branch_id> t_branch_id_pair;
    
    typedef map<t_branch_id, Branch> t_Tree;
    /*! \brief Combines __Branch__ into tree like structure.
        \details
        Tree class represents next structure: 
        \imageSize{TreeClass.jpg, height:40%;width:40%;, }

        \todo resolve problem with private methods.
    */
    class Tree: public t_Tree
    {
        public:
            ///Creates empty tree without any branches.
            Tree() = default; 

            Tree(const Tree& t);

            Tree& operator= (const Tree &t);

            bool operator==(const Tree &t) const;
            
            /*! \brief Initialize __tree__ with source points vector \p source_point and source angle vector \p source_angle.
                \details
                This function is designed to fit River::Boundary class.

                It creates empty source branches which further will be developed in
                river networks by design
            */
            void Initialize(const Boundaries::trees_interface_t ids_points_angles);
            
            /// Adds new \p branch with \p id (\p id should be unique).
            t_branch_id AddBranch(const Branch &branch, t_branch_id id = UINT_MAX);
            
            /// Adds two subbranches with
            t_branch_id_pair AddSubBranches(t_branch_id root_branch_id, 
                const Branch &left_branch, const Branch &right_branch);
            
            ///Delete branch.
            void DeleteBranch(t_branch_id branch_id);

            ///Delete sub branch of current sub branch.
            void DeleteSubBranches(t_branch_id root_branch_id);

            ///Clear whole tree.
            void Clear();

            /*! \brief Returns root(or source) branch of branch __branch_id__(if there is no such - throw exception).
                \throw Exception if there is no parent branch.
            */
            t_branch_id GetParentBranchId(t_branch_id branch_id) const;
            
            /// Returns link to parent branch.
            Branch& GetParentBranch(t_branch_id branch_id);

            /*! \brief Returns pair of ids of subranches.
                \throw Exception if there is no sub branches.
            */
            t_branch_id_pair GetSubBranchesIds(t_branch_id branch_id) const;

            /// Returns reference to subbranches
            pair<Branch&, Branch&> GetSubBranches(t_branch_id branch_id);

            /*! \brief Returns id of adjacent branch to current \p sub_branch_id branch.
                \throw Exception if there is no adjacent branch.
            */
            t_branch_id GetAdjacentBranchId(t_branch_id sub_branch_id) const;

            ///Returns link to adjacent branch with \p id.
            Branch& GetAdjacentBranch(t_branch_id sub_branch_id);

            //Growth
            ///Adds  relatively vector of \p points to Branches \p tips_id.
            void AddPoints(const vector<t_branch_id>& tips_id, const t_PointList& points);

            ///Adds  relatively \p points to Branches \p tips_id.
            void AddPolars(const vector<t_branch_id>& tips_id, const vector<Polar> &points);

            ///Adds  absolute \p points to Branches \p tips_id.
            void AddAbsolutePolars(const vector<t_branch_id>& tips_id, const vector<Polar>& points);

            t_branch_id_pair GrowTestTree(
                const t_branch_id branch_id = 1, const double ds = 0.05, const unsigned n = 3, const double dalpha = 0);

            ///Returns vector of tip branches ids.
            vector<t_branch_id> TipBranchesIds() const;

            vector<t_branch_id> zero_lenght_tip_branches_ids(double zero_lenght) const;

            ///Returns vector of tip branches Points.
            t_PointList TipPoints() const;

            ///Returns vector of tip branches Points.
            map<t_branch_id, Point> TipIdsAndPoints() const;

            ///Checks if current id of branch is source or not.
            bool IsSourceBranch(const t_branch_id branch_id) const;

            ///Evaluates curvature of tips. Used in non-euler growth
            double maximal_tip_curvature_distance() const;

            void flatten_tip_curvature();

            ///Removes tips points, or in other words reverts one step of simulation.
            void remove_tip_points();
            
            ///Checks if Branch \p branch_id has subbranches.
            bool HasSubBranches(const t_branch_id branch_id) const;

            bool HasParentBranch(t_branch_id sub_branch_id) const;

            ///Checks for validity of \p id.
            bool IsValidBranchId(const t_branch_id id) const;

            void handle_non_existing_branch_id(const t_branch_id id) const;

            ///Generates unique id number for new subbranch.
            t_branch_id GenerateNewID() const;

            ///Holds realations between root branhces and its subbranches.
            map<t_branch_id, t_branch_id_pair> branches_relation;   

            ///Prints tree to stream.
            friend ostream& operator<<(ostream& write, const Tree & b);
    };
}
