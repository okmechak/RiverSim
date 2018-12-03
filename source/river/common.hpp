#pragma once

#include <vector>

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
struct Point
{
  double x, y;
  unsigned int index = 0;
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