#pragma once

#include <vector>
#include <math.h>
#include <iostream>

using namespace std;


/*
  hash pairing function used for uordered map 
*/
namespace std
{
template <>
struct hash<pair<int, int>>
{
  size_t operator()(const pair<int, int> &x) const
  {
    int f = x.first, s = x.second;
    return hash<int>()((f + s) * (f + s + 1) / 2 + s);
  }
};
} // namespace std



namespace River
{
/*
    Is used to simply describe one branch of river. From one
    biffurcation point to another
  
  */
class Polar
{
  public:
    double dl;
    double phi;
    
    void print()
    {
      cout << "Polar point: " << dl << " " << phi << endl;
    }
};

/*
  Point struct and feew functions to work with it
*/
class Point
{
  public:
    double x, y;
    Point() = default;
    ~Point() = default;
    Point(double xval, double yval);
    Point(Polar p);

    double norm() const;
    Point getNormalized();
    Point& rotate(double phi);
    Polar getPolar() const;
    void normalize();
    double angle() const;
    double angle(Point p) const;
    void print() const;

    Point& operator=(const Point& p) = default;
    Point operator+(const Point& p) const;
    Point& operator+=(const Point& p);
    Point operator-(const Point& p) const;
    Point& operator-=(const Point& p);
    double operator*(const Point& p) const;
    Point operator*(const double gain) const;
    Point& operator*=(const double gain);
    Point operator/(const double gain) const;
    Point& operator/=(const double gain);
    bool operator==(const Point& p) const;
    friend ostream& operator <<(ostream& write, const Point & p);
};


class Line
{
  unsigned int p1, p2;
};

} // namespace River