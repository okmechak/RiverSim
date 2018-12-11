#include "common.hpp"

namespace River
{

    Point::Point(double xval, double yval)
    {
      x = xval;
      y = yval;
    };

    Point::Point(Polar p)
    {
      x = p.dl * cos(p.phi);
      y = p.dl * sin(p.phi);
    };

    Point& Point::rotate(double phi)
    {
        auto tempx = x,
            tempy = y;
        x = tempx*cos(phi) - tempy*sin(phi);
        y = tempx*sin(phi) + tempy*cos(phi);
        
        return *this;
    }

    double Point::norm() const
    {
      return sqrt(x*x + y*y);
    }

    Point Point::getNormalized()
    {
      return Point{x/norm(), y/norm()};
    }

    Polar Point::getPolar() const
    {
      return Polar{norm(), angle()};
    }

    void Point::normalize()
    {
      auto l = norm();
      x /= l;
      y /= l;
    }

    double Point::angle() const
    {
      double phi = acos(x/norm());
      if(y < 0)
        phi = -phi;
      return phi;
    }

    double Point::angle(Point p) const
    {
      //order of points is important
      double phi = acos((x*p.x + y*p.y)/norm()/p.norm());
      double sign = x*p.y - p.x*y > 0 ? 1 : -1;//FIXME: is this sign correct?
      phi *= sign;
      return phi;
    }
    void Point::print() const
    {
      cout << *this << endl;
    }

    bool Point::operator==(const Point& p) const
    {   
        double eps = 1e-15;
        return abs(x - p.x) < eps && abs(y - p.y);
    }

    Point Point::operator+(const Point& p) const
    { 
        return Point{x + p.x, y + p.y};
    }

    Point& Point::operator+=(const Point& p) 
    { 
        x += p.x;
        y += p.y; 
        return *this; 
    }

    Point Point::operator-(const Point& p) const
    { 
        return Point{x - p.x, y - p.y};
    }

    Point& Point::operator-=(const Point& p) 
    { 
        x -= p.x;
        y -= p.y; 
        return *this; 
    }
    
    double Point::operator*(const Point& p) const
    { 
        return (x*p.x + y*p.y); 
    }

    Point Point::operator*(const double gain) const
    { 
        return Point{x*gain, y*gain};
    }

    Point& Point::operator*=(const double gain)
    { 
        x *= gain;
        y *= gain;
        return *this;
    }

    Point Point::operator/(const double gain) const
    { 
        return Point{x/gain, y/gain};
    }

    Point& Point::operator/=(const double gain)
    { 
        x /= gain;
        y /= gain;
        return *this;
    }

    ostream& operator << (ostream& write, const Point& p)
    {
        write << "point: " << p.x << ", " << p.y;
        return write;
    }




    /*
        Vector Triangle Input Output data structure
    */

    ostream& operator <<(ostream& write, const vecTriangulateIO & io)
    {
        //TODO:
    }
}//River namespace