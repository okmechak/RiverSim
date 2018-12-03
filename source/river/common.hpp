#pragma once

#include <vector>
#include <math.h>

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
struct Polar
{
  double dl;
  double phi;
  unsigned int index = 0;
};

/*
  Point struct and feew functions to work with it
*/
class Point
{
  public:
    double x, y;
    unsigned int index = 0;
    Point() = default;
    Point(double x, double y, unsigned int index = 0)
    {
      x=x;
      y=y;
      index=index;
    };

    Point(Polar p)
    {
      x = p.dl * cos(p.phi);
      y = p.dl * sin(p.phi);
      index = p.index;
    };

    double norm()
    {
      return sqrt(x*x + y*y);
    }

    Point getNormalized()
    {
      return Point{x/norm(), y/norm(), index};
    }

    Polar getPolar()
    {
      return Polar{norm(), angle(), index};
    }

    void normalize()
    {
      auto l = norm();
      x /= l;
      y /= l;
    }

    double angle()
    {
      double phi = acos(x/norm());
      if(y < 0)
        phi = -phi;
      return phi;
    }

    double angle(Point p)
    {
      //order of points is important
      double phi = acos((x*p.x + y*p.y)/norm()/p.norm());
      double sign = x*p.y - p.x*y > 0 ? 1 : -1;//FIXME: is this sign correct?
      phi *= sign;
      return phi;
    }
};


struct Line
{
  unsigned int p1, p2;
  unsigned int index = 0;
};

class vecTriangulateIO
{
public:
  vector<double> points = {};
  vector<int> pointTags = {};
  int numOfAttrPerPoint = 0;
  vector<double> pointAttributes = {};
  vector<int> pointMarkers = {};
  vector<int> segments = {};
  vector<int> segmentMarkers = {};
  vector<int> triangles = {};
  int numOfAttrPerTriangle = 0;
  vector<double> triangleAttributes = {};
  vector<double> triangleAreas = {};
  vector<int> neighbors = {};
  //array of coordiantes
  vector<double> holes = {};
  //array of array of coordinates
  int numOfRegions = 1;
  vector<double> regions = {};
  //out only
  vector<int> edges = {};
  vector<int> edgeMarkers = {};
};

} // namespace River