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
    Polar() = default;
    Polar(double dlval, double phival):
      r{dlval}, phi{phival} {};
    double r = 0.;
    double phi = 0.;
    
    friend ostream& operator <<(ostream& write, const Polar & p)
    {
      cout << "Polar: " << p.r << " " << p.phi << endl;
      return write;
    }
};

/*
  Point struct and feew functions to work with it
*/
class Point
{
  private:
    double eps = 1e-13;

  public:
    double x = 0, y = 0;

    Point() = default;
    ~Point() = default;
    Point(double xval, double yval);
    Point(const Point &p) = default;
    Point(const Polar &p);

    double norm() const;
    static double norm(double x, double y);
    Point getNormalized();
    Point& rotate(double phi);
    Polar getPolar() const;
    Point& normalize();
    double angle() const;
    static double angle(double x, double y);
    double angle(Point &p) const;
    Point& print();

    Point& operator=(const Point& p) = default;
    Point operator+(const Point& p) const;
    Point& operator+=(const Point& p);
    Point operator-(const Point& p) const;
    Point& operator-=(const Point& p);
    double operator*(const Point& p) const;
    double operator[](const int index) const;
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