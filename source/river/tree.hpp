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
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
///\endcond
#include "GeometryPrimitives.hpp"

using namespace std;

namespace River
{
    /*! \brief Holds all functionality that you need to work with single branch.
        \details
        Take a look on picture:
        \imageSize{BranchNewClass.jpg, height:40%;width:40%;, }
        \todo resolve problem with private members
    */
    class BranchNew
    {
        public:
            /*! \brief BranchNew construcor.
                \details Initiates branch with initial point \p source_point and initial \p angle.
                \param[in] source_point_val - Branch source point.
                \param[in] angle - Intial growth(or flow) dirrection of brahch source point.
            */
            BranchNew(const Point& source_point_val, double angle):
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
                points.push_back(p);
                return *this;
            }
            
            ///Adds polar \p __p__ coords to branch with absolute angle, but position is relative to tip
            inline BranchNew& AddAbsolutePoint(const Polar& p)
            {
                points.push_back(TipPoint() + Point{p});
                return *this;
            }
            
            ///Adds point \p __p__ to branch in tip relative coord system.
            inline BranchNew& AddPoint(const Point &p)
            {
                points.push_back(TipPoint() + p);
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
                if(Size() == 1)
                    throw Exception("Can't remove last point");   
                points.pop_back();
                return *this;
            }

            ///Clear Branch
            inline BranchNew& Clear(){ points.clear(); return *this; }
            
            /*! @} */

            /*! \name Getters and Setters
                @{
            */
            /*! \brief Return TipPoint of branch(last point in branch).
                \throw Exception if branch is empty.
            */
            inline Point TipPoint() const 
            {
                if(Size() == 0)
                    throw Exception("Can't return TipPoint size is zero");
                return points.at(Size() - 1);
            }

            /*! \brief Returns vector of tip - difference between two adjacent points.
                \throw Exception if branch consist only from one point.
                \warning name a little confusing cos there is still source_angle variable.
            */
            inline Point TipVector() const 
            {
                if(Size() == 1)
                    throw Exception("Can't return TipVector size is 1");

                return points.at(Size() - 1) - points.at(Size() - 2);
            }

            /*! \brief Returns vector of \p i th segment of branch.
                \throw Exception if branch size is 1 or is \p i > branch size.
            */
            inline Point Vector(unsigned i) const
            {
                if(Size() == 1)
                    throw Exception("Can't return Vector. Size is 1");
                if(i >= Size() || i == 0)
                    throw Exception("Can't return Vector. Index is bigger then size or is zero");

                return points.at(i) - points.at(i - 1);
            }
            
            /*! \brief Returns angle of branch tip.
                \throw Exception if branch is empty.
            */
            double TipAngle() const 
            {
                if(Size() < 1)
                    throw Exception("TipAngle: size is less then 1!");
                else if(Size() == 1)
                    return source_angle; 

                return TipVector().angle();
            }

            ///Returns SourcePoint of branch(the first one).
            inline Point SourcePoint() const{return points.at(0);}

            ///Returns SourceAngle of branch - initial __source_angle__.
            inline double SourceAngle() const {return source_angle;}
            /*! @} */

            /*! \name Different Parameters
                @{
            */

            ///Checks if branch is empyt - but it never should.
            inline bool Empty() const 
            {
                return points.empty() || points.size() == 1/*branch with one point is empty too*/ 
                    || Lenght() < eps;
            }

            ///Returns Lenght of whole branch.
            double Lenght() const 
            {
                double lenght = 0.;
                if(Size() > 1)
                    for(unsigned int i = 1; i < Size(); ++i)
                        lenght += (points.at(i) - points.at(i - 1)).norm();

                return lenght;
            };


            ///Returns number of points in branch.
            inline unsigned int Size() const {return points.size();}

            /*! \brief Returns BranchNew::Lenght() divided by BranchNew::Size().
                \throw Exception if branch consist only from one point.
            */
            double AverageSpeed() const
            {
                if(Size() == 1)
                    throw Exception("Average speed can't be evaluated of empty branch");    
                return Lenght()/(Size() - 1);
            }
            /**
             * @}
             */

            ///Prints branch and all its parameters.
            friend ostream& operator<<(ostream& write, const BranchNew & b)
            {
                int i = 0;
                write << "Branch " << endl;
                write << "  lenght - " << b.Lenght() << endl;
                write << "  size - " << b.Size() << endl;
                write << "  source angle - " << b.source_angle << endl;
                for(auto p: b.points)
                    write <<"   " << i++ << " ) " << p << endl;

                return write;
            }

            ///Comparison of branches.
            bool operator==(const BranchNew& br) const
            {
                if(br.points != points)
                    return false;
                if(SourceAngle() != br.SourceAngle())
                    return false;

                return true;
            }

            ///Returns points vector.
            inline vector<Point> GetPoints(){return points;}

            ///Returns \p i th point vector.
            inline Point GetPoint(unsigned i) const 
            {
                return points.at(i);
            }

            
        //private:

            ///Initial angle of source(or direction of source).
            double source_angle;

            ///Vector which holds all points of branch.
            vector<Point> points;

            ///Used in Shrink function call.
            ///If after shrink lenght of between adjacent to tip point
            ///is less then eps then we delete it.
            double eps = 5e-6;

    };



    /*! \brief Combines __BranchNew__ into tree like structure.
        \details
        Tree class represents next structure: 
        \imageSize{TreeClass.jpg, height:40%;width:40%;, }

        \todo resolve problem with private methods.
     */
    class Tree
    {
        public:
            /*! \brief Creates empty tree without source branches. */
            Tree() = default; 

            ///Copy constructor.
            Tree(const Tree& t);

            ///Destructor
            ~Tree()
            {
                Clear();
            }

            ///Assignment.
            Tree& operator= (const Tree &t);
            
            /*! \name Modificators
                @{
            */
            /*! \brief Initialize __tree__ with source points vector \p source_point and source angle vector \p source_angle.
                \details
                This function is designed to fit River::Border class.

                It creates empty source branches which further will be developed in
                river networks by design

                \todo does it work for many sources(>1)?
            */
            Tree& Initialize(vector<Point> sources_point, vector<double> sources_angle, vector<int> ids);
            

            //Additions
            /*! \brief Adds new \p branch with \p id (\p id should be unique).
                \todo make the function this private.
            */
            int AddBranch(const BranchNew &branch, int id = -999);

            /*! Adds new source __branch__ with __id__.
                \throw Exception if \p id is invalid(invalid value or already exist).
            */
            int AddSourceBranch(const BranchNew &branch, int id = -999)
            {   
                if(id == -999)
                    id = GenerateNewID();

                if(!IsValidBranchId(id))
                    throw Exception("Invalid id of branch at adding to source branch of tree.");

                source_branches_id.push_back(id);
                
                return AddBranch(branch, id);
            }
            
            ///Adds Sub Branches \p left_brach, \p right_branch to \p root_branch_id.
            pair<int, int> AddSubBranches(int root_branch_id, BranchNew &left_branch, BranchNew &right_branch);


            //Getters of ids
            /*! \brief Returns root(or source) branch of branch __branch_id__(if there is no such - throw exception).
                \throw Exception if there is no parent branch.
            */
            int GetParentBranchId(int branch_id) const;

            /*! \brief Returns pair of ids of subranches.
                \throw Exception if there is no sub branches.
            */
            pair<int, int> GetSubBranchesId(int branch_id) const
            {   
                if(!HasSubBranches(branch_id))
                    throw Exception("branch does't have sub branches");

                return branches_relation.at(branch_id);
            }

            /*! \brief Returns id of adjacent branch to current \p sub_branch_id branch.
                \throw Exception if there is no adjacent branch.
            */
            int GetAdjacentBranchId(int sub_branch_id) const;

            ///Generates unique id number for new subbranch.
            unsigned int GenerateNewID() const;


            //Getters of Branches
            /*! \brief Returns link to branch with \p id.
                \throw Exception if there is no such branch.
            */
            const BranchNew* GetBranch(int id) const;

            /*! \brief Returns link to branch with \p id.
                \throw Exception if there is no such branch.
            */
            BranchNew* GetBranch(int id);

            /*! \brief Returns link to parent branch.
                \throw Exception if there is no parent branch.
            */
            BranchNew* GetParentBranch(int branch_id)
            {return GetBranch(GetParentBranchId(branch_id));}

            /*! \brief Returns link to adjacent branch with \p id.
                \throw Exception if there is no adjacent branch.
            */
            BranchNew* GetAdjacentBranch(int sub_branch_id)
            {
                return GetBranch(GetAdjacentBranchId(sub_branch_id));
            }

            /*! \brief Returns reference to subbranches
                \throw Exception if there is no sub branches.
            */
            pair<BranchNew*, BranchNew*> GetSubBranches(int branch_id)
            {
                auto[left_branch, right_branch] = GetSubBranchesId(branch_id);
                return{GetBranch(left_branch), GetBranch(right_branch)};
            }

            //Delete
            ///Clear all branches from tree.
            Tree& Clear()
            {
                branches_relation.clear();
                source_branches_id.clear();
                branches.clear();
                branches_index.clear();

                return *this;
            }
            
            Tree& DeleteSubBranches(int root_branch_id);

            Tree& DeleteBranch(int branch_id)
            {
                branches.remove(*GetBranch(branch_id));
                branches_index.erase(branch_id);
                remove(source_branches_id.begin(), source_branches_id.end(), branch_id);
                branches_relation.erase(branch_id);
                return *this;
            }


            //Growth
            ///Adds  relatively vector of \p points to Branches \p tips_id.
            Tree& AddPoints(const vector<Point>& points, const vector<int>& tips_id);

            ///Adds  relatively \p points to Branches \p tips_id.
            Tree& AddPolars(const vector<Polar>& points, const vector<int>& tips_id);

            ///Adds  absolute \p points to Branches \p tips_id.
            Tree& AddAbsolutePolars(const vector<Polar>& points, const vector<int>& tips_id);


            //Checks
            ///Checks if branch with \p id exists.
            inline bool DoesExistBranch(int id)const{return branches_index.count(id);}

            ///Checks if current id of branch is source or not.
            int IsSourceBranch(int branch_id) const
            {
                if(!DoesExistBranch(branch_id))
                    throw Exception("HasParentBranch: there is no such branch");

                return find(source_branches_id.begin(), source_branches_id.end(), branch_id) != source_branches_id.end();
            }

            ///Checks if Branchs \p branch_id has root(or source) branch.
            bool HasParentBranch(int branch_id) const
            {
                if(!DoesExistBranch(branch_id))
                    throw Exception("HasParentBranch: there is no such branch");
                
                for(auto key_val: branches_relation)
                    if(key_val.second.first == branch_id || key_val.second.second == branch_id)
                        return true;

                return false;
            }
            
            ///Checks if Branch \p branch_id has subbranches.
            bool HasSubBranches(int branch_id) const
            {
                if(!DoesExistBranch(branch_id))
                    throw Exception("HasSubBranches: there is no such branch");
                return branches_relation.count(branch_id);
            }

            ///Checks if tree has non zero sized branches.
            bool HasEmptySourceBranch() const
            {   
                //No branches at all means empty source too.
                //It will stop simulation immediately.
                if(source_branches_id.empty())
                    return true;

                for(auto id: source_branches_id)
                    if(GetBranch(id)->Empty())
                        return true;

                return false;
            }

            ///Checks for validity of \p id.
            bool IsValidBranchId(int id) const
            {return id >= 1;}
            /**
             * @}
             */

            /**
             * @name Functional
             * @{
             */

            ///Returns vector of tip branches ids.
            vector<int> TipBranchesId() const;

            ///Returns vector of tip branches Points.
            vector<Point> TipPoints() const;

            ///Returns vector of tip branches Points.
            map<int, Point> TipIdsAndPoints() const;
            /**
             * @}
             */

            //Some properties
            ///Returns number of source branches.
            inline int NumberOfSourceBranches() const {return source_branches_id.size();}
            ///Return vector of source branches ids.
            inline vector<int> SourceBranchesID() const {return source_branches_id;}

        //private:
            /**
             * @name Private
             * @{
             */

            ///Holds realations between root branhces and its subbranches.
            map<int, pair<int, int>> branches_relation;

            ///Holds branches ids and its position in BranchNew::branches vector.
            map<int, BranchNew*> branches_index;

            ///Holds all branches.
            list<BranchNew> branches;

            ///Holds all source branches.
            vector<int> source_branches_id;

            ///Invalid branch index. Used in error handling.
            int invalid_branch = -2;        

            ///Prints tree to stream.
            friend ostream& operator<<(ostream& write, const Tree & b);
            /**
             * @}
             */
    };

    ///Generates trees boundary
    void TreeVector(vector<Point> &tree_vector, int id, const Tree& tree, double eps);
}
