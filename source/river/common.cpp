#include "common.hpp"

namespace River
{

    Point::Point(double xval, double yval, unsigned int indexval)
    {
      x = xval;
      y = yval;
      index = indexval;
    };

    Point::Point(Polar p)
    {
      x = p.dl * cos(p.phi);
      y = p.dl * sin(p.phi);
      index = p.index;
    };

    double Point::norm() const
    {
      return sqrt(x*x + y*y);
    }

    Point Point::getNormalized()
    {
      return Point{x/norm(), y/norm(), index};
    }

    Polar Point::getPolar() const
    {
      return Polar{norm(), angle(), index};
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
      cout << this << endl;
    }

    bool Point::operator==(const Point& p) const
    {   
        double eps = 1e-15;
        return abs(x - p.x) < eps && abs(y - p.y);
    }

    Point Point::operator+(const Point& p) const
    { 
        return Point{x + p.x, y + p.y, index};
    }

    Point& Point::operator+=(const Point& p) 
    { 
        x += p.x;
        y += p.y; 
        return *this; 
    }

    Point Point::operator-(const Point& p) const
    { 
        return Point{x - p.x, y - p.y, index};
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
        return Point{x*gain, y*gain, index};
    }

    Point& Point::operator*=(const double gain)
    { 
        x *= gain;
        y *= gain;
        return *this;
    }

    Point Point::operator/(const double gain) const
    { 
        return Point{x/gain, y/gain, index};
    }

    Point& Point::operator/=(const double gain)
    { 
        x /= gain;
        y /= gain;
        return *this;
    }

    ostream& operator << (ostream& write, const Point& p)
    {
        write << "point: " << p.x << ", " << p.y << ", " << p.index;
        return write;
    }


}//River namespace