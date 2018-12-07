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
struct Polar
{
  double dl;
  double phi;
  unsigned int index = 0;
  void print()
  {
    cout << "Polar point: " << dl << " " << phi 
      << " " << index << endl;
  }
};

/*
  Point struct and feew functions to work with it
*/
class Point
{
  public:
    double x = 0, y = 0;
    unsigned int index = 0;
    Point() = default;
    ~Point() = default;
    Point(double xval, double yval, unsigned int indexval = 0);
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


struct Line
{
  unsigned int p1, p2;
  unsigned int index = 0;
};

class vecTriangulateIO
{
public:
  vecTriangulateIO() = default;
  ~vecTriangulateIO() = default;
  //Nodes
  struct{
    vector<double> coords = {};
    vector<int> tags = {};
    int numOfAttr = 0;
    vector<double> attributes = {};
    vector<int> markers = {};
  } node;

  //Elements
  struct {
    //1-line, 2-triangle, 3-quad, 15-point
    struct{
      vector<int> nodeTags = {};
      vector<int> markers = {};
    } line;

    struct{
      vector<int> nodeTags = {};
      int numOfAttr = 0;
      vector<double> attributes = {};
      vector<int> markers = {};
      vector<double> areas = {};
      vector<int> neighbors = {};
    } triangle;
    
    struct{
      vector<int> nodeTags = {};
      int numOfAttr = 0;
      vector<double> attributes = {};
      vector<int> markers = {};

    } quad;

  } element;

  //array of coordiantes
  vector<double> holes = {};
  //array of array of coordinates
  int numOfRegions = 1;
  vector<double> regions = {};
  
  //out only
  vector<int> edges = {};
  vector<int> edgeMarkers = {};
  friend ostream& operator <<(ostream& write, const vecTriangulateIO & io);
};

} // namespace River