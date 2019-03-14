#pragma once

#include <iostream>
#include <vector>
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



    };


    //generataor of boundaries from tree and boudary obejcts TODO





}
