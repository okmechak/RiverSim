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

/** @file tree.hpp
 *   Holds all functionality that you need to work with tree of river, its separate branches and generation of final boundary geometry
 *   
 *   This file holds several classes like __BranchNew__, __Tree__, __GeometryNew__.
 *   BranchNew class represents single branch without any biffuracation points. 
 *   These branches are combined into Tree by means 
 *   of __Tree__ class. And finnaly __Tree__ class and __Border__ class are generated 
 *   into geometry that is used in simmulation by means of
 *   __GeometryNew__ class.
 */

#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include "common.hpp"
#include "tethex.hpp"
#include "physmodel.hpp"
#include "border.hpp"

using namespace std;

namespace River
{
    ///BranchNew class holds all functionality that you need to work with single branch
    class BranchNew
    {
        public:
            /**
             * BranchNew construcor.
             * Initiates branch with initial point __source_point__ and initial __angle__.
             */
            BranchNew(const Point& source_point_val, double angle):
                source_angle(angle)
            {
                AddAbsolutePoint(source_point_val);
            };

            /**
             * @name Modificators
             * @{
             */
            ///Adds point __p__ to branch with absolute coords.
            BranchNew& AddAbsolutePoint(const Point& p)
            {
                points.push_back(p);
                return *this;
            }
            
            ///Adds polar __p__ coords to branch with absolute angle, but position is relative to tip
            BranchNew& AddAbsolutePoint(const Polar& p)
            {
                points.push_back(TipPoint() + Point{p});
                return *this;
            }
            
            ///Adds point __p__ to branch in tip relative coord system.
            BranchNew& AddPoint(const Point &p)
            {
                points.push_back(TipPoint() + p);
                return *this;
            }

            ///Adds polar __p__ to branch in tip relative coord and angle system.
            BranchNew& AddPoint(const Polar& p)
            {
                auto p_new = Polar{p};
                p_new.phi += TipAngle();
                AddAbsolutePoint(p_new);
                return *this;
            }

            ///Reduces lenght of branch by __lenght__.
            ///If __lenght__ is greater than full lenght of branch, then __source_point__ only remains.
            BranchNew& Shrink(double lenght);

            ///Remove tip point from branch(simply pops element from vector)
            BranchNew& RemoveTipPoint()
            {
                if(Size() == 1)
                    throw invalid_argument("Can't remove last point");   
                points.pop_back();
                return *this;
            }
            /**
             * @}
             */

            /**
             * @name Getters and Setters
             * @{
             */
            ///Return TipPoint of branch(last point in branch)
            Point TipPoint() const 
            {
                if(Size() == 0)
                    throw invalid_argument("Can't return TipPoint size is zero");
                return points.at(Size() - 1);
            }

            ///Returns vector of tip - difference between two adjacent points.
            ///If __size__ is <= 0exception is rised.
            Point TipVector() const 
            {
                if(Size() == 1)
                    throw invalid_argument("Can't return TipVector size is 1");

                return points.at(Size() - 1) - points.at(Size() - 2);
            }

            ///Returns vector of i-th segment of branch.
            Point Vector(unsigned i) const
            {
                if(Size() == 1)
                    throw invalid_argument("Can't return Vector. Size is 1");
                if(i >= Size() || i == 0)
                    throw invalid_argument("Can't return Vector. Index is bigger then size or is zero");

                return points.at(i) - points.at(i - 1);
            }
            
            ///Returns angle of tip of branch
            double TipAngle() const 
            {
                if(Size() < 1)
                    throw invalid_argument("TipAngle: size is less then 1!");
                else if(Size() == 1)
                    return source_angle; 

                return TipVector().angle();
            }

            ///Returns SourcePoint of branch(the first one)
            Point SourcePoint() const{return points.at(0);}

            ///Returns SourceAngle of branch - initial __source_angle__
            double SourceAngle() const {return source_angle;}
            /**
             * @}
             */

            /**
             * @name Different Parameters
             * @{
             */

            ///Checks if branch is empyt - but it never should
            bool Empty() const {return points.empty();}

            ///Returns Lenght of whole branch
            double Lenght() const 
            {
                double lenght = 0.;
                if(Size() > 1)
                    for(unsigned int i = 1; i < Size(); ++i)
                        lenght += (points.at(i) - points.at(i - 1)).norm();

                return lenght;
            };


            ///Returns number of points in branch
            unsigned int Size() const {return points.size();}

            ///Returns BranchNew::Lenght() divided by BranchNew::Size()
            double AverageSpeed() const
            {
                if(Size() == 1)
                    throw invalid_argument("Average speed can't be evaluated of empty branch");    
                return Lenght()/(Size() - 1);
            }
            /**
             * @}
             */

            ///Prints branch and all its parameters
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

            ///Comparsion of branches
            bool operator==(const BranchNew& br) const
            {
                if(br.points != points)
                    return false;
                if(SourceAngle() != br.SourceAngle())
                    return false;

                return true;
            }

            ///Returns points vector
            vector<Point> GetPoints(){return points;}
            ///Returns i-th point vector
            Point GetPoint(unsigned i) const 
            {
                return points.at(i);
            }

            
        //private: FIXME
            ///Initial angle of source(or direction of source)
            double source_angle;

            ///Vector which holds all points of branch
            vector<Point> points;

            ///Used in Shrink function call
            ///If after shrink lenght of between adjacent to tip point
            ///is less then eps then we delete it.
            double eps = 1e-5;

    };



    /**
     * Combines __BranchNew__ into tree like structure.
     * 
     * At first its contains few source points and its directions, then they can be developed into a tree 
     * like structure using AddPoints and other fucntions.
     */
    class Tree
    {
        public:
            /**
             *Default constructor 
             */
            Tree() = default; 
            
            /**
             * @name Modificators
             * @{
             */
            /**
             * Constructor of __Tree__.
             * 
             * Takes as input vector of __sources_point__, __sources_angle__ and their __ids__, 
             * and for each entry creates instance of __BranchNew__ object.
             */
            Tree& Initialize(vector<Point> sources_point, vector<double> sources_angle, vector<int> ids);
            

            //Additions
            ///Adds new __branch__ with __id__(id should be unique).
            int AddBranch(const BranchNew &branch, int id = -999);//FIXME: this should be private

            ///Adds new source __branch__ with __id__.
            int AddSourceBranch(const BranchNew &branch, int id = -999)
            {   
                if(id == -999)
                    id = GenerateNewID();

                if(!IsValidBranchId(id))
                    throw invalid_argument("Invalid id of branch at adding to source branch of tree.");

                source_branches_id.push_back(id);
                
                return AddBranch(branch, id);
            }
            
            ///Adds Sub Branches __left_brach__, __right_branch__ to __root_branch_id__.
            pair<int, int> AddSubBranches(int root_branch_id, BranchNew &left_branch, BranchNew &right_branch);


            //Getters of ids
            ///Returns root(or source) branch of branch __branch_id__(if there is no such - throw exception).
            int GetParentBranchId(int branch_id);

            ///Returns pair of ids of subranches.
            pair<int, int> GetSubBranchesId(int branch_id) const
            {   
                if(!HasSubBranches(branch_id))
                    throw invalid_argument("branch does't have sub branches");

                return branches_relation.at(branch_id);
            }

            ///Returns id of adjacent branch with __id__.
            int GetAdjacentBranchId(int sub_branch_id);

            ///Generates unique id number for new subbranch.
            unsigned int GenerateNewID();


            //Getters of Branches
            ///Returns link to branch with __id__.
            const BranchNew* GetBranch(int id) const;

            ///Returns link to branch with __id__.
            BranchNew* GetBranch(int id);

            BranchNew* GetParentBranch(int branch_id)
            {return GetBranch(GetParentBranchId(branch_id));}

            ///Returns link to adjacent branch with __id__.
            BranchNew* GetAdjacentBranch(int sub_branch_id)
            {
                return GetBranch(GetAdjacentBranchId(sub_branch_id));
            }

            ///Returns reference to subbranches
            pair<BranchNew*, BranchNew*> GetSubBranches(int branch_id)
            {
                auto[left_branch, right_branch] = GetSubBranchesId(branch_id);
                return{GetBranch(left_branch), GetBranch(right_branch)};
            }

            //Delete
            ///Clear data from class
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
            ///Adds  relatively __points__ to Branches __tips_id__.
            Tree& AddPoints(vector<Point> points, vector<int> tips_id);

            ///Adds  relatively __points__ to Branches __tips_id__.
            Tree& AddPolars(vector<Polar> points, vector<int> tips_id);

            ///Adds  absolute __points__ to Branches __tips_id__.
            Tree& AddAbsolutePolars(vector<Polar> points, vector<int> tips_id);


            //Checks
            ///Checks if branch with __id__ exists.
            bool DoesExistBranch(int id)const{return branches_index.count(id);}

            ///Checks if current id of branch is source or not.
            int IsSourceBranch(int branch_id)
            {
                if(!DoesExistBranch(branch_id))
                    throw invalid_argument("HasParentBranch: there is no such branch");

                return find(source_branches_id.begin(), source_branches_id.end(), branch_id) != source_branches_id.end();
            }

            ///Checks if Branchs __branch_id__ has root(or source) branch.
            bool HasParentBranch(int branch_id)
            {
                if(!DoesExistBranch(branch_id))
                    throw invalid_argument("HasParentBranch: there is no such branch");
                
                for(auto key_val: branches_relation)
                    if(key_val.second.first == branch_id || key_val.second.second == branch_id)
                        return true;

                return false;
            }
            
            ///Checks if Branch __branch_id__ has subbranches.
            bool HasSubBranches(int branch_id) const
            {
                if(!DoesExistBranch(branch_id))
                    throw invalid_argument("HasSubBranches: there is no such branch");
                return branches_relation.count(branch_id);
            }

            ///Checks if tree has non zero sized branches.
            bool HasEmptySourceBranch()
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

            ///Checks for validity of __id__.
            bool IsValidBranchId(int id)
            {return id >= 1;}
            /**
             * @}
             */

            /**
             * @name Functional
             * @{
             */

            ///Returns vector of tip branches ids.
            vector<int> TipBranchesId();

            ///Returns vector of tip branches Points.
            vector<Point> TipPoints();

            ///Returns vector of tip branches Points.
            map<int, Point> TipIdsAndPoints();
            /**
             * @}
             */

            //Some properties
            ///Returns number of source branches.
            int NumberOfSourceBranches(){return source_branches_id.size();}
            ///Return vector of source branches ids.
            vector<int> SourceBranchesID(){return source_branches_id;}

        //private:  FIXME: cos i need somehow test private members
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

    /**
     * Finnal Boudary Geneartor Class
     * 
     * Sticks together all components: Tree class, boudary class and model parameters
     */ 
    tethex::Mesh BoundaryGenerator(const Model& mdl, const Tree& tree, const Border &br);
}
