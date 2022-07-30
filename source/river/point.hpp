#pragma once

///\cond
#include <vector>
#include <iostream>
#include <exception>
#include <math.h>
///\endcond

///Absolute preccision comparison between double numbers
#define EPS 1.e-13

using namespace std;

namespace River
{
    ///Exception used for handling errors in program.
    struct Exception : public exception
    {
        string s;
        Exception(const string ss) : s(ss) {}
        ~Exception() throw () {} // Updated
        const char* what() const throw() { return s.c_str(); }
    };
    
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
    
            ///Angle of point in radians.
            double phi = 0.;

            ///Equal operator
            bool operator ==(const Polar & p) const
            {
                return abs(p.r - r)/2 + (p.phi - phi)/2 < EPS;
            }
            
            ///Not-equal operator
            bool operator !=(const Polar & p) const
            {
                return !(*this == p);
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
            /*! \brief Comparison of points with \ref EPS accuracy.
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
  };
} // namespace River