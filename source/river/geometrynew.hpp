/*! \file geometrynew.hpp
    \brief Holds all functionality that you need to work with tree of river, its separate branches and generation of final boundary geometry
    
    This file holds several classes like @BranchNew, @Tree, @GeometryNew.
    BranchNew class represents single branch without any biffuracation points. These branches are combined into Tree by means 
    of @Tree class. And finnaly @Tree class and @Border class are generated into geometry that is used in simmulation by means of
    @GeometryNew class.
*/

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
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
            ///@BranchNew construcor.
            /**
                             Initiates branch with initial point @source_point and initial @angle
                            */
            BranchNew(const Point& source_point_val, double angle):
                source_angle(angle)
            {
                AddAbsolutePoint(source_point_val);
            };

            //modificators
            ///Adds point @p to branch with absolute coords
            BranchNew& AddAbsolutePoint(const Point& p)
            {
                points.push_back(p);
                return *this;
            }
            
            ///Adds polar @p coords to branch with absolute angle, but position is relative to tip
            BranchNew& AddAbsolutePoint(const Polar& p)
            {
                points.push_back(TipPoint() + Point{p});
                return *this;
            }
            
            ///Adds point @p  to branch in tip relative coord system.
            BranchNew& AddPoint(const Point &p)
            {
                points.push_back(TipPoint() + p);
                return *this;
            }

            ///Adds polar @p  to branch in tip relative coord and angle system.
            BranchNew& AddPoint(const Polar& p)
            {
                auto p_new = Polar{p};
                p_new.phi += TipAngle();
                AddAbsolutePoint(p_new);
                return *this;
            }

            ///Reduces lenght of branch by @lenght.
            ///If @lenght is greater than full lenght of branch, then @source_point only remains.
            BranchNew& Shrink(double lenght);

            ///Remove tip point from branch(simply pops element from vector)
            BranchNew& RemoveTipPoint()
            {
                if(Size() == 1)
                    throw invalid_argument("Can't remove last point");   
                points.pop_back();
                return *this;
            }
            
            //getters and setters
            ///Return TipPoint of branch(last point in branch)
            Point TipPoint() const 
            {
                if(Size() == 0)
                    throw invalid_argument("Can't return TipPoint size is zero");
                return points.at(Size() - 1);
            }

            ///Returns vector of tip - difference between two adjacent points
            ///if size is <= 0exception is rised.
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
                if(Size() == 1)
                    return source_angle; 
                return TipVector().angle();
            }

            ///Returns SourcePoint of branch(the first one)
            Point SourcePoint() const{return points.at(0);}

            ///Returns SourceAngle of branch - initial @source_angle
            double SourceAngle() const {return source_angle;}

            //different params
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

            ///Returns @Lenght divided by @Size
            double AverageSpeed() const
            {
                if(Size() == 1)
                    throw invalid_argument("Average speed can't be evaluated of empty branch");    
                return Lenght()/(Size() - 1);
            }

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

            ///Returns points vector
            vector<Point> GetPoints(){return points;}
            Point GetPoint(unsigned i){return points.at(i);}

            
        private:
            ///Initial angle of source(or direction of source)
            double source_angle;
            ///Vector which holds all points of branch
            vector<Point> points;

    };








    ///Combines @BranchNew into tree like structure.
    ///
    ///At first its contains few source points and its directions, then they can be developed into a tree like structure
    ///Using AddPoints and other fucntions.
    class Tree
    {
        public: 
            ///Constructor of @Tree.
            ///
            ///Takes as input vector of @sources_point, @sources_angle and their @ids, and for each entry creates instance of @BranchNew object.
            Tree(vector<Point> sources_point, vector<double> sources_angle, vector<int> ids)
            {
                for(unsigned int i = 0; i < ids.size(); ++i)
                {
                    AddSourceBranch(BranchNew{
                            sources_point.at(i), 
                            sources_angle.at(i)}, 
                            ids.at(i));
                }
            }

            ///Adds  relatively @points to Branches @tips_id.
            Tree& AddPoints(vector<Point> points, vector<int> tips_id)
            {
                for(unsigned int i = 0; i < tips_id.size(); ++i)
                    if(DoesExistBranch(tips_id.at(i)))
                    {
                        BranchNew& br = GetBranch(tips_id.at(i));
                        br.AddPoint(points.at(i));
                    }
                    else
                        throw invalid_argument("Such branch does not exist");

                return *this;
            }


            ///Adds Sub Branches @left_bracnhs, @right_branch to @root_branch_id
            pair<int, int> AddSubBranches(int root_branch_id, BranchNew &left_branch, BranchNew &right_branch)
            {   
                if(!DoesExistBranch(root_branch_id))
                    throw invalid_argument("root branch doesn't exis");

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

            ///Returns vector of tip branches ds.
            vector<int> TipBranchesId()
            {
                vector<int> tip_branches_id;
                for(auto p: branches_index)
                    if(!HasSubBranches(p.first))
                        tip_branches_id.push_back(p.first);

                return tip_branches_id;
            }

            ///Returns number of source branches.
            int NumberOfSourceBranches(){return source_branches_id.size();}

        //private:  FIXME: cos i need somehow test private members
            
            ///Holds realations between root branhces and its subbranches.
            map<int, pair<int, int>> branches_relation;
            ///Holds branches ids and its position in @branches vector.
            map<int, unsigned int> branches_index;
            ///Holds all branches.
            vector<BranchNew> branches;
            ///Holds all source branches.
            vector<int> source_branches_id;
            ///Invalid branch index. Used in error handling.
            int invalid_branch = -2;

            ///Adds new source @branch with @id.
            Tree& AddSourceBranch(const BranchNew &branch, int id)
            {
                source_branches_id.push_back(id);
                return AddBranch(branch, id);
            }

            Tree& AddBranch(const BranchNew &branch, int id)
            {
                if(DoesExistBranch(id))
                    throw invalid_argument("Invalid Id, such branch already exists");
                if(!IsValidBranchId(id))
                    throw invalid_argument("Invalid Id, id should be greater then 0");
                
                branches.push_back(branch);
                branches_index[id] = branches.size() - 1;

                return *this;
            }

            ///Checks if branch with @id exists.
            bool DoesExistBranch(int id){return branches_index.count(id);}

            ///Returns link to branch with @id.
            BranchNew& GetBranch(int id)
            {
                if(!DoesExistBranch(id))
                    throw invalid_argument("there is no such branch");
                
                return branches.at(branches_index[id]);
            }

            ///Checks if Branch @branch_id has subbranches.
            bool HasSubBranches(int branch_id)
            {
                return branches_relation.count(branch_id);
            }

            ///Checks if Branchs @branch_id has root(or source) branch.
            bool IsSubBranch(int branch_id)
            {
                if(!DoesExistBranch(branch_id))
                    throw invalid_argument("there is no such branch");
                
                for(auto key_val: branches_relation)
                    if(key_val.second.first == branch_id || key_val.second.second == branch_id)
                        return true;

                return false;
            }

            ///Returns root(or source) branch of branch @branch_id.
            int GetSourceBranch(int branch_id)
            {
                if(IsSubBranch(branch_id))
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

            pair<int, int> GetSubBranches(int branch_id)
            {   
                if(!HasSubBranches(branch_id))
                    throw invalid_argument("branch does't have sub branches");
                return branches_relation[branch_id];
            }



            bool IsValidBranchId(int id)
            {
                return id >= 1;
            }

            ///Generates unique id number for new subbranch.
            unsigned int GenerateNewID()
            {
                unsigned max_id = 1;
                
                vector<int> branches_id;
                branches_id.reserve(branches_index.size());

                for(auto &key_val: branches_index)
                    branches_id.push_back(key_val.first);

                while(find(begin(branches_id), end(branches_id), ++max_id)!= branches_id.end());

                return max_id;
            }

            friend ostream& operator<<(ostream& write, const Tree & b)
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
    };

    ///Finnal Boudary Geneartor Class
    ///
    ///Sticks together all components: Tree class, boudary class and model parameters
    void TreeVector(vector<Point> &tree_vector, int id, Tree& tr, double eps);
    tethex::Mesh BoundaryGenerator(const Model& mdl, Tree& tr, Border &br, int boundary_id);

}
