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
    Branch::Branch(
        const Point& source_point_val, 
        const double angle):
        source_angle(angle)
    {
        AddAbsolutePoint(source_point_val);
    }

    Branch& Branch::AddAbsolutePoint(const Point& p)
    {
        this->push_back(p);
        return *this;
    }

    Branch& Branch::AddAbsolutePoint(const Polar& p)
    {
        this->push_back(TipPoint() + Point{p});
        return *this;
    }

    Branch& Branch::AddPoint(const Point &p)
    {
        this->push_back(TipPoint() + p);
        return *this;
    }

    Branch& Branch::AddPoint(const Polar& p)
    {
        auto p_new = Polar{p};
        p_new.phi += TipAngle();
        AddAbsolutePoint(p_new);
        return *this;
    }

    Branch& Branch::Shrink(double lenght)
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

    Branch& Branch::RemoveTipPoint()
    {
        if(this->size() == 1)
            throw Exception("Last branch point con't be removed");   
        this->pop_back();
        return *this;
    }

    Point Branch::TipPoint() const 
    {
        if(this->size() == 0)
            throw Exception("Can't return TipPoint size is zero");
        return this->at(this->size() - 1);
    }

    Point Branch::TipVector() const 
    {
        if(this->size() <= 1)
            throw Exception("Can't return TipVector size is 1 or even less");

        return this->at(this->size() - 1) - this->at(this->size() - 2);
    }

    Point Branch::Vector(unsigned i) const
    {
        if(this->size() == 1)
            throw Exception("Can't return Vector. Size is 1");
        if(i >= this->size() || i == 0)
            throw Exception("Can't return Vector. Index is bigger then size or is zero");

        return this->at(i) - this->at(i - 1);
    }

    double Branch::TipAngle() const 
    {
        if(this->size() < 1)
            throw Exception("TipAngle: size is less then 1!");
        else if(this->size() == 1)
            return source_angle; 

        return TipVector().angle();
    }

    Point Branch::SourcePoint() const
    {
        return this->at(0);
    }

    double Branch::SourceAngle() const 
    {
        return source_angle;
    }

    double Branch::Lenght() const 
    {
        double lenght = 0.;
        if(this->size() > 1)
            for(unsigned int i = 1; i < this->size(); ++i)
                lenght += (this->at(i) - this->at(i - 1)).norm();

        return lenght;
    }

    ostream& operator<<(ostream& write, const Branch & b)
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

    bool Branch::operator==(const Branch& br) const
    {
        return equal(this->begin(), this->end(), br.begin()) &&
            SourceAngle() == br.SourceAngle();
    }
}