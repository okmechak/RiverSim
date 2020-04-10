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
    File holds two classes: __River::BranchNew__ and __River::Tree__.
    As you can see from picture - River::BranchNew is simple array joined points - piecewice line, 
    aproximation of real river branch. River::Tree on other hand consist from combination of Branches.
    __Bifurcation point__ - where two rivers unions into one river. So each branch has its subbranches.
    Structure of branches and sub branches is represented here in class River::Tree.

    \sa River::Tree, River::BranchNew
 */

#pragma once

///\cond
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <climits>
///\endcond

#include "GeometryPrimitives.hpp"
#include "boundary.hpp"

using namespace std;

namespace River
{
    /*! \brief Holds all functionality that you need to work with single branch.
        \details
        Take a look on picture:
        \imageSize{BranchNewClass.jpg, height:40%;width:40%;, }
        \todo resolve problem with private members
    */
    class BranchNew: public vector<Point>
    {
        public:
            BranchNew() = default;

            /*! \brief BranchNew construcor.
                \details Initiates branch with initial point \p source_point and initial \p angle.
                \param[in] source_point_val - Branch source point.
                \param[in] angle - Intial growth(or flow) dirrection of brahch source point.
            */
            BranchNew(const Point& source_point_val, const double angle):
                source_angle(angle)
            {
                AddAbsolutePoint(source_point_val);
            };

            /*! \name Modificators
                @{
            */
            ///Adds point \p __p__ to branch with absolute coords.
            inline BranchNew& AddAbsolutePoint(const Point& p)
            {
                this->push_back(p);
                return *this;
            }
            
            ///Adds polar \p __p__ coords to branch with absolute angle, but position is relative to tip
            inline BranchNew& AddAbsolutePoint(const Polar& p)
            {
                this->push_back(TipPoint() + Point{p});
                return *this;
            }
            
            ///Adds point \p __p__ to branch in tip relative coord system.
            inline BranchNew& AddPoint(const Point &p)
            {
                this->push_back(TipPoint() + p);
                return *this;
            }

            ///Adds polar \p __p__ to branch in tip relative coord and angle system.
            inline BranchNew& AddPoint(const Polar& p)
            {
                auto p_new = Polar{p};
                p_new.phi += TipAngle();
                AddAbsolutePoint(p_new);
                return *this;
            }

            /*! \brief Reduces lenght of branch by \p lenght.
                \note
                If \p lenght is greater than full lenght of branch, then __source_point__ only remains.
                \throw Exception.
            */
            BranchNew& Shrink(double lenght);

            /*! \brief Remove tip point from branch(simply pops element from vector).
                \imageSize{BranchNewShrink.jpg, height:30%;width:30%;, }
                \throw Exception if trying to remove last point.
            */
            inline BranchNew& RemoveTipPoint()
            {
                if(this->size() == 1)
                    throw Exception("Last branch point con't be removed");   
                this->pop_back();
                return *this;
            }
            
            /*! @} */

            /*! \name Getters and Setters
                @{
            */
           inline Point GetPoint(size_t i) const 
            {
                return this->at(i);
            }

            /*! \brief Return TipPoint of branch(last point in branch).
                \throw Exception if branch is empty.
            */
            inline Point TipPoint() const 
            {
                if(this->size() == 0)
                    throw Exception("Can't return TipPoint size is zero");
                return this->at(this->size() - 1);
            }

            /*! \brief Returns vector of tip - difference between two adjacent points.
                \throw Exception if branch consist only from one point.
                \warning name a little confusing cos there is still source_angle variable.
            */
            inline Point TipVector() const 
            {
                if(this->size() <= 1)
                    throw Exception("Can't return TipVector size is 1 or even less");

                return this->at(this->size() - 1) - this->at(this->size() - 2);
            }

            /*! \brief Returns vector of \p i th segment of branch.
                \throw Exception if branch size is 1 or is \p i > branch size.
            */
            inline Point Vector(unsigned i) const
            {
                if(this->size() == 1)
                    throw Exception("Can't return Vector. Size is 1");
                if(i >= this->size() || i == 0)
                    throw Exception("Can't return Vector. Index is bigger then size or is zero");

                return this->at(i) - this->at(i - 1);
            }
            
            /*! \brief Returns angle of branch tip.
                \throw Exception if branch is empty.
            */
            double TipAngle() const 
            {
                if(this->size() < 1)
                    throw Exception("TipAngle: size is less then 1!");
                else if(this->size() == 1)
                    return source_angle; 

                return TipVector().angle();
            }

            ///Returns SourcePoint of branch(the first one).
            inline Point SourcePoint() const
            {
                return this->at(0);
            }

            ///Returns SourcePoint of branch(the first one).
            inline void SetSourcePoint(const Point& p)
            {
                this->at(0) = p;
            }

            ///Returns SourceAngle of branch - initial __source_angle__.
            inline double SourceAngle() const 
            {
                return source_angle;
            }

            inline void SetSourceAngle(double angle) 
            {
                source_angle = angle;
            }
            /*! @} */

            /*! \name Different Parameters
                @{
            */

            ///Checks if branch is empyt - but it never should.
            inline bool Empty() const 
            {
                return this->empty() || this->size() == 1/*branch with one point is empty too*/ 
                    || Lenght() < eps;
            }

            ///Returns Lenght of whole branch.
            double Lenght() const 
            {
                double lenght = 0.;
                if(this->size() > 1)
                    for(unsigned int i = 1; i < this->size(); ++i)
                        lenght += (this->at(i) - this->at(i - 1)).norm();

                return lenght;
            };
            /**
             * @}
             */

            ///Prints branch and all its parameters.
            friend ostream& operator<<(ostream& write, const BranchNew & b)
            {
                int i = 0;
                write << "Branch " << endl;
                write << "  lenght - " << b.Lenght() << endl;
                write << "  size - " << b.size() << endl;
                write << "  source angle - " << b.source_angle << endl;
                for(auto p: b)
                    write <<"   " << i++ << " ) " << p << endl;

                return write;
            }

            ///Comparison of branches.
            bool operator==(const BranchNew& br) const
            {
                return equal(this->begin(), this->end(), br.begin()) &&
                    SourceAngle() == br.SourceAngle();
            }
            
        //private:

            ///Initial angle of source(or direction of source).
            double source_angle;

            ///Used in Shrink function call.
            ///If after shrink lenght of between adjacent to tip point
            ///is less then eps then we delete it.
            double eps = 5e-7;
    };

    typedef t_source_id t_branch_id;

    /*! \brief Combines __BranchNew__ into tree like structure.
        \details
        Tree class represents next structure: 
        \imageSize{TreeClass.jpg, height:40%;width:40%;, }

        \todo resolve problem with private methods.
     */
    class Tree: public map<t_branch_id, BranchNew>
    {
        public:
            /*! \brief Creates empty tree without source branches. */
            Tree() = default; 

            ///Copy constructor.
            Tree(const Tree& t);

            ///Assignment.
            Tree& operator= (const Tree &t);

            ///Comparison
            bool operator==(const Tree &t) const;
            
            /*! \name Modificators
                @{
            */
            /*! \brief Initialize __tree__ with source points vector \p source_point and source angle vector \p source_angle.
                \details
                This function is designed to fit River::Boundary class.

                It creates empty source branches which further will be developed in
                river networks by design

                \todo does it work for many sources(>1)?
            */
            void Initialize(const Boundaries::trees_interface_t ids_points_angles);
            

            //Additions
            /*! \brief Adds new \p branch with \p id (\p id should be unique).
            */
            t_branch_id AddBranch(const BranchNew &branch, t_branch_id id = UINT_MAX);
            
            ///Adds Sub Branches \p left_brach, \p right_branch to \p root_branch_id.
            pair<t_branch_id, t_branch_id> AddSubBranches(t_branch_id root_branch_id, 
                const BranchNew &left_branch, const BranchNew &right_branch);


            //Getters of ids
            /*! \brief Returns root(or source) branch of branch __branch_id__(if there is no such - throw exception).
                \throw Exception if there is no parent branch.
            */
            t_branch_id GetParentBranchId(t_branch_id branch_id) const;

            /*! \brief Returns pair of ids of subranches.
                \throw Exception if there is no sub branches.
            */
            pair<t_branch_id, t_branch_id> GetSubBranchesIds(t_branch_id branch_id) const
            {   
                if(!HasSubBranches(branch_id))
                    throw Exception("branch does't have sub branches");

                return branches_relation.at(branch_id);
            }

            /*! \brief Returns id of adjacent branch to current \p sub_branch_id branch.
                \throw Exception if there is no adjacent branch.
            */
            t_branch_id GetAdjacentBranchId(t_branch_id sub_branch_id) const;

            ///Generates unique id number for new subbranch.
            unsigned GenerateNewID() const;

            bool HasParentBranch(t_branch_id sub_branch_id) const
            {
                if(!this->count(sub_branch_id))
                    throw Exception("HasParentBranch: no such branch.");

                for(const auto&[parent_id, sub_ids]: branches_relation)
                    if(sub_ids.first == sub_branch_id || sub_ids.second == sub_branch_id)
                        return true;

                return false;
            }

            //Getters of Branches
            /*! \brief Returns link to parent branch.
                \throw Exception if there is no parent branch.
            */
            BranchNew& GetParentBranch(t_branch_id branch_id)
            {
                return this->at(GetParentBranchId(branch_id));
            }

            /*! \brief Returns link to adjacent branch with \p id.
                \throw Exception if there is no adjacent branch.
            */
            BranchNew& GetAdjacentBranch(t_branch_id sub_branch_id)
            {
                return this->at(GetAdjacentBranchId(sub_branch_id));
            }

            /*! \brief Returns reference to subbranches
                \throw Exception if there is no sub branches.
            */
            pair<BranchNew&, BranchNew&> GetSubBranches(t_branch_id branch_id)
            {
                auto[left_branch, right_branch] = GetSubBranchesIds(branch_id);
                return {this->at(left_branch), this->at(right_branch)};
            }

            //Delete
            ///Clear all branches from tree.
            void Clear()
            {
                this->clear();
                branches_relation.clear();
            }
            
            void DeleteSubBranches(t_branch_id root_branch_id);

            void DeleteBranch(t_branch_id branch_id)
            {
                if(!this->count(branch_id))
                    throw Exception("DeleteBranch: no such branch: " + to_string(branch_id));

                this->erase(branch_id);
                branches_relation.erase(branch_id);
            }
            //Growth
            ///Adds  relatively vector of \p points to Branches \p tips_id.
            void AddPoints(const vector<t_branch_id>& tips_id, const vector<Point>& points);

            ///Adds  relatively \p points to Branches \p tips_id.
            void AddPolars(const vector<t_branch_id>& tips_id, const vector<Polar> &points);

            ///Adds  absolute \p points to Branches \p tips_id.
            void AddAbsolutePolars(const vector<t_branch_id>& tips_id, const vector<Polar>& points);

            pair<t_branch_id, t_branch_id> GrowTestTree(
                const t_branch_id branch_id = 1, const double ds = 0.05, const unsigned n = 3, const double dalpha = 0);

            //Checks
            ///Checks if current id of branch is source or not.
            bool IsSourceBranch(const t_branch_id branch_id) const
            {
                if(!this->count(branch_id))
                    throw Exception("IsSourceBranch: no such branch id: " + to_string(branch_id));

                for(const auto&[id, sub_ids]: branches_relation)
                    if(branch_id == sub_ids.first || branch_id == sub_ids.second)
                        return false;

                return true;
            }
            
            ///Checks if Branch \p branch_id has subbranches.
            bool HasSubBranches(const t_branch_id branch_id) const
            {
                if(!this->count(branch_id))
                    throw Exception("HasSubBranches: there is no such branch");
                return branches_relation.count(branch_id);
            }

            ///Checks for validity of \p id.
            bool IsValidBranchId(const t_branch_id id) const
            {
                return id >= 1 and id != UINT_MAX;
            }
            /**
             * @}
             */

            /**
             * @name Functional
             * @{
             */

            ///Returns vector of tip branches ids.
            vector<t_branch_id> TipBranchesIds() const;

            ///Returns vector of tip branches Points.
            vector<Point> TipPoints() const;

            ///Returns vector of tip branches Points.
            map<t_branch_id, Point> TipIdsAndPoints() const;
            /**
             * @}
             */

            /**
             * @name Private
             * @{
             */

            ///Holds realations between root branhces and its subbranches.
            map<t_branch_id, pair<t_branch_id, t_branch_id>> branches_relation;   

            ///Prints tree to stream.
            friend ostream& operator<<(ostream& write, const Tree & b);
            /**
             * @}
             */
    };
}
