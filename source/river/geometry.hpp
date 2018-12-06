#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>

#include <iostream>

#include <math.h>

#include "common.hpp"

using namespace std;


namespace River
{

class Branch
{
  //TODO: Implement this class
public:
  unsigned long int id;
  string name = "branch";
  vector<Point> leftPoints;
  vector<Point> rightPoints;

  Branch(unsigned long int id, Point sourcePoint, double phi);
  ~Branch() = default;

  //modify branch
  void addPoint(Point p);
  void addDPoint(Point p);
  void addPolar(Polar p);
  void removeHeadPoint();
  void shrink(double dl);
  double width();
  void setWidth(double epsVal);

  //geom entities
  Point getHead();
  double getHeadAngle();
  Point getTail();

  //statistics
  bool empty();
  double length();
  unsigned int size();
  double averageSpeed();
  //lot of others to be implemented

  //debug
  void print();

private:
  pair<Point, Point> splitPoint(Point p, double phi);
  Point mergePoints(Point p1, Point p2);
  double eps = 1e-1;
};





/*
  Geometry class

  this is simiple geometry class used to easilly do 
  different operations with geometry which describes river
  mesh. This class strongly dependce of Branch class
*/

class Geometry
{
public:
  //Geometry region indicators/markers
  enum Markers
  {
    None = 0,
    Bottom,
    Right,
    Top,
    Left,
    River
  };

  //Nodes and lines - main interface of this class
  vector<Point> points;
  vector<Line> lines;

  //branches functionality
  map<unsigned int, pair<unsigned int, unsigned int>> branchRelation;
  map<unsigned int, unsigned int> branchIndexes;
  vector<Branch> branches;

  //Segments or Edges or Elements

  Geometry() = default;
  ~Geometry() = default;

  void initiateRootBranch(unsigned int id = 1);
  void addPoints(vector<Point> points);
  void addDPoints(vector<Point> dpoints);
  void addPolar(Polar p, bool bRelativeAngle = false);
  void SetSquareBoundary(
      Point BottomBoxCorner,
      Point TopBoxCorner,
      double dx);
  vector<unsigned int> GetTipIds();
  Branch& GetBranch(unsigned int id);
  vector<Polar> GetTipPolars();
  void generateCircularBoundary();
  void SetEps(double epsVal);

private:
  double eps = 1e-1;
  double bifAngle = M_PI/5;

  unsigned int rootBranchId = 0;//0 means no root/first Branch

  //Boundary
  //Box parameters
  vector<Point> boundaryPoints;
  vector<Line> boundaryLines;
  double dx = 0.5;

  //river geometry parameters
  double alpha = M_PI/3.; // biffurcation angle
  double len = 0.1; // biffurcation initial length 
  
  pair<Point, double> GetEndPointOfSquareBoundary();
  void InserBranchTree(unsigned int id);
};

} // namespace River