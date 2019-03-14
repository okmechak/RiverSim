#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "common.hpp"
#include "tethex.hpp"

using namespace std;

namespace River
{
    class BranchNew
    {
        public:

            BranchNew(const Point& source_point_val, double angle):
                source_angle(angle)
            {
                AddAbsolutePoint(source_point_val);
            };

            //modificators
            BranchNew& AddAbsolutePoint(const Point& p)
            {
                points.push_back(p);
                return *this;
            }

            BranchNew& AddAbsolutePoint(const Polar& p)
            {
                points.push_back(TipPoint() + Point{p});
                return *this;
            }

            BranchNew& AddPoint(const Point &p)
            {
                points.push_back(TipPoint() + p);
                return *this;
            }

            BranchNew& AddPoint(const Polar& p)
            {
                auto p_new = Polar{p};
                p_new.phi += TipAngle();
                AddAbsolutePoint(p_new);
                return *this;
            }

            BranchNew& Shrink(double lenght);//TODO

            BranchNew& RemoveTipPoint()
            {
                if(Size() == 1)
                    throw invalid_argument("Can't remove last point");   
                points.pop_back();
                return *this;
            }
            
            //getters and setters
            Point TipPoint() const 
            {
                if(Size() == 0)
                    throw invalid_argument("Can't return TipPoint size is zero");
                return points.at(Size() - 1);
            }

            Point TipVector() const 
            {
                if(Size() == 1)
                    throw invalid_argument("Can't return TipVector size is 1");

                return points.at(Size() - 1) - points.at(Size() - 2);
            }

            double TipAngle() const 
            {
                if(Size() == 1)
                    return source_angle; 
                return TipVector().angle();
            }

            Point SourcePoint() const{return points.at(0);}

            double SourceAngle() const {return source_angle;}

            //different params
            bool Empty() const {return points.empty();}

            double Lenght() const 
            {
                double lenght = 0.;
                if(Size() > 1)
                    for(unsigned int i = 1; i < Size(); ++i)
                        lenght += (points.at(i) - points.at(i - 1)).norm();

                return lenght;
            };

            unsigned int Size() const {return points.size();}

            double AverageSpeed() const
            {
                if(Size() == 1)
                    throw invalid_argument("Average speed can't be evaluated of empty branch");    
                return Lenght()/(Size() - 1);
            }

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

            
        private:
            double source_angle;
            vector<Point> points;

    };

    class Tree
    {
        public: 
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

            Tree& AddSourceBranch(const BranchNew &branch, int id)
            {
                if(branches_index.count(id))
                    throw invalid_argument("Invalid Id, such branch already exists");

                branches.push_back(branch);
                source_branches_id.push_back(id);
                branches_index[id] = branches.size() - 1;
                
                return *this;
            }

            Tree& AddSubBranches(int root_branch_id, BranchNew &left_branch, BranchNew &right_branch)
            {
                
                return *this;
            }

            vector<int> TipBranchesId()
            {
                vector<int> tip_branches_id;
                for(auto p: branches_index)
                    if(!HasSubBranches(p.first))
                        tip_branches_id.push_back(p.first);

                return tip_branches_id;
            }

            int NumberOfSourceBranches(){return source_branches_id.size();}

        //private:  FIXME: cos i need somehow test private members
            map<int, pair<int, int>> branches_relation;
            map<int, unsigned int> branches_index;
            vector<BranchNew> branches;
            vector<int> source_branches_id;

            bool DoesExistBranch(int id){return branches_index.count(id);}

            BranchNew& GetBranch(int id)
            {
                if(!DoesExistBranch(id))
                    throw invalid_argument("there is no such branch");
                
                return branches.at(branches_index[id]);
            }

            bool HasSubBranches(int branch_id)
            {
                return branches_relation.count(branch_id);
            }

            unsigned int GenerateNewID(unsigned int prevID, bool isLeft = true)
            {return (prevID << 1) + (int)isLeft;}




    };


    //generataor of boundaries from tree and boudary obejcts TODO





}
