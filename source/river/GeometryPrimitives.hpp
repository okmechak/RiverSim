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

/*! \file GeometryPrimitives.hpp
    \brief Common geometric entities like Vectors, Lines etc., to whole program.
    \details
    Program is higly depended on geometry which consist of Points, Vectors, Polar Vectors, Lines
    and other which are implemented below.
    Class River::Point is the most important and is basic class for rest geometrical classes like \ref River::Branch and \ref River::Boundary.

    \see River::Point
 */
#pragma once

///\cond
#include <vector>
#include <iostream>
#include <exception>
///\endcond

///Absolute preccision comparison between dobule numbers
#define EPS 1.e-13

using namespace std;

namespace River
{
    struct Exception : public exception
    {
        string s;
        Exception(const string ss) : s(ss) {}
        ~Exception() throw () {} // Updated
        const char* what() const throw() { return s.c_str(); }
    };
    
    ///Prints logs depending of log configuration(quiet or verbose)
    void print(const bool flag, const string str);
    
    class Point;

    class Polar;

    /*! \brief Returns normalized vector of current Point.
        \throw Exception if vector lenght is zero
        \return Normalized vector
    */
    Point GetNormalizedPoint(const Point &p);

    ///Returns Polar representation of vector.
    Polar ToPolar(const Point &p);


    /*! \class Polar
        \brief Vector represented in polar coordinates.
        
        \details Contains only two public variables: __r__ and __phi__
        \imageSize{PolarClass.jpg, height:40%;width:40%;, }
    */
    class Polar
    {
        private:
            static constexpr double eps = EPS;

        public:

            ///Creates \"zero\"-polar vector(r = 0, phi = 0)
            Polar() = default;
    
            ///Creates \"non-zero\"-polar vector(r = \p dlval, phi = \p phival)
            Polar(const double dlval, const double phival):
                r{dlval}, 
                phi{phival} 
                {};
    
            ///Radius of points.
            double r = 0.;
    
            ///Angle of point.
            double phi = 0.;

            ///Equal operator
            bool operator ==(const Polar & p) const
            {
                return abs(p.r - r)/2 + (p.phi - phi)/2 < eps;
            }
            
            ///Not-equal operator
            bool operator !=(const Polar & p) const
            {
                return !(*this == p);
            }
    
            ///Converts Polar Object \p to string and redirects it to stream object \p write.
            friend ostream& operator <<(ostream& write, const Polar & p)
            {
                write << "Polar: " << p.r << " " << p.phi;
                return write;
            }
    };

    /*! \class Point
        \brief Point object represented as radius vector.
        
        \details Mathematically - it is a vector, contains only two public variables, coordinates: __x__ and __y__.
        Also it has a lot of operations which enables mathematical operations with two vectors:
        vector and scalar, comparsion of two vectors, angle between vectors, rotations etc.
        \imageSize{PointClass.jpg, height:40%;width:40%;, }
    */
     class Point
    {
        private:
            ///Absolute precission parameter, used in Point comparsion \ref operator== function.
            static constexpr double eps = EPS;

        public:
            ///x coordinate.
            double x = 0; 
            ///y coordinate.
            double y = 0;

            ///Default constructor
            Point() = default;

            ///Default destructor
            ~Point() = default;

            ///Point constructor.
            Point(const double xval, const double yval);

            ///Copy constructor
            Point(const Point &p) = default;

            ///Converts Polar coordinates to Cartesian of Point.
            Point(const Polar &p);

            ///Returns lenght(norm) of vector.
            double norm() const;

            ///Evaluates norm of vector {\p x, \p y}.
            static double norm(const double x, const double y);

            ///Rotates point on \p phi angle(counterclockwise)
            Point& rotate(const double phi);

            /*! \brief Normalizes current Point.
                \throw Exception if vector lenght is zero
            */
            Point& normalize();

            /*! \brief Returns angle of vector.
                \throw Exception if vector lenght is zero
            */
            double angle() const;

            /*! \brief Returns angle of {\p x,\p y} vector.
                \throw Exception if vector lenght is zero
            */
            static double angle(const double x, const double y);

            /*! \brief Returns angle relatively to \p p Point.
                \throw Exception if vector lenght one of poits is zero
            */
            double angle(const Point &p) const;

            /*! @name Math Operations 
                @{
            */
            ///Assignment
            Point& operator=(const Point& p) = default;
            ///Sum
            Point operator+(const Point& p) const;
            Point& operator+=(const Point& p);
            Point operator-(const Point& p) const;
            Point& operator-=(const Point& p);
            ///Scalar product
            double operator*(const Point& p) const;
            /*! \brief Comparison of points with \ref eps accuracy.
            */
            bool operator==(const Point& p) const;
            /*! @} */
            /*! @name Lenght gain operations
                @{
            */
            Point operator*(const double gain) const;
            Point& operator*=(const double gain);
            Point operator/(const double gain) const;
            Point& operator/=(const double gain);
            /*! @} */
            ///Retrives cordinate using array syntaxis.
            double operator[](const int index) const;
            double operator()(const int index) const;

            ///Converts object to string and redirects it to stream.
            friend ostream& operator <<(ostream& write, const Point & p);
  };
} // namespace River