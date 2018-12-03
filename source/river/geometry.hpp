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
  ~Branch();

  //modify branch
  void addPoint(Point p);
  void addPolar(Polar p);
  void removeHeadPoint();
  void shrink(double dl);
  double width();
  void setWidth(double eps);

  //geom entities
  Point getHead();
  Point getDirection();
  Point getTail();

  //statistics
  bool empty();
  double length();
  unsigned int size();
  double averageSpeed();
  //lot of others to be implemented

private:
  pair<Point, Point> splitPoint(Point p, double phi);
  double eps = 1e-8;
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
  map<int, 
    pair<unsigned int, unsigned int>> branchRelation;
  map<int, Branch> branches;

  //Segments or Edges or Elements

  Geometry();
  ~Geometry();

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
  void SetEps(double eps);

private:
  double eps = 1e-8;

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