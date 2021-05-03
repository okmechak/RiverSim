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

/*! \file branch.hpp
    \brief
    Holds definition of branch object.

    \sa River::Tree, River::Branch
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

#include "point.hpp"
#include "boundary.hpp"

using namespace std;

namespace River
{

    /*! \brief Holds all functionality that you need to work with single branch.
        \details
        Take a look on picture:
        \imageSize{BranchClass.jpg, height:40%;width:40%;, }
        \todo resolve problem with private members
    */
    class Branch: public t_PointList
    {
        public:
            Branch() = default;

            /*! \brief Branch construcor.
                \details Initiates branch with initial point \p source_point and initial \p angle.
                \param[in] source_point_val - Branch source point.
                \param[in] angle - Intial growth(or flow) dirrection of brahch source point.
            */
            Branch(const Point& source_point_val, const double angle);

            /*! \name Modificators
                @{
            */
            ///Adds point \p __p__ to branch with absolute coords.
            Branch& AddAbsolutePoint(const Point& p);
            
            ///Adds polar \p __p__ coords to branch with absolute angle, but position is relative to tip.
            Branch& AddAbsolutePoint(const Polar& p);
            
            ///Adds point \p __p__ to branch in tip relative coord system.
            Branch& AddPoint(const Point &p);

            ///Adds polar \p __p__ to branch in tip relative coord and angle system.
            Branch& AddPoint(const Polar& p);

            /*! \brief Reduces lenght of branch by \p lenght.
                \note
                If \p lenght is greater than full lenght of branch, then __source_point__ only remains.
                \throw Exception.
            */
            Branch& Shrink(double lenght);

            /*! \brief Remove tip point from branch(simply pops element from vector).
                \imageSize{BranchShrink.jpg, height:30%;width:30%;, }
                \throw Exception if trying to remove last point.
            */
            Branch& RemoveTipPoint();
            
            /*! @} */

            /*! \name Getters and Setters
                @{
            */
            /*! \brief Return TipPoint of branch(last point in branch).
                \throw Exception if branch is empty.
            */
            Point TipPoint() const;

            /*! \brief Returns vector of tip - difference between two adjacent points.
                \throw Exception if branch consist only from one point.
                \warning name a little confusing cos there is still source_angle variable.
            */
            Point TipVector() const;

            /*! \brief Returns vector of \p i th segment of branch.
                \throw Exception if branch size is 1 or is \p i > branch size.
            */
            Point Vector(unsigned i) const;
            
            /*! \brief Returns angle of branch tip.
                \throw Exception if branch is empty.
            */
            double TipAngle() const;

            ///Returns source point of branch(the first one).
            Point SourcePoint() const;

            ///Returns source angle of branch - initial __source_angle__.
            double SourceAngle() const;
            /*! @} */

            /*! \name Different Parameters
                @{
            */

            ///Returns lenght of whole branch.
            double Lenght() const;
            /**
             * @}
             */

            ///Prints branch and all its parameters.
            friend ostream& operator<<(ostream& write, const Branch & b);

            ///Comparison of branches.
            bool operator==(const Branch& br) const;
            
        //private:

            ///Initial angle of source(or direction of source).
            double source_angle;

            ///Used in Shrink function call.
            ///If after shrink lenght of between adjacent to tip point
            ///is less then eps then we delete it.
            double eps = 5e-7;
    };
}