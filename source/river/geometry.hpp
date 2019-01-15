#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>

#include <iostream>

#include <math.h>

#include "common.hpp"
#include "tethex.hpp"

using namespace std;



namespace River
{


class GeomTag
{
  public:
    GeomTag(unsigned int idVal = 0, unsigned int rVal = 0);
    int branchId = 0;
    int regionTag = 0;
};

class GeomPolar : public Polar
{
  public:
    GeomPolar(double r, double phiVal, 
      int branchIdVal = 0, 
      int regionTagVal = 0,
      double meshSizeVal = 1.);

    int branchId = 0, regionTag;
    double meshSize;
};

class GeomLine
{
  public:
    GeomLine(unsigned int p1Val, unsigned int p2Val, 
      int branchIdVal, int regionTagVal);
    unsigned int p1, p2;
    int branchId = 0, 
       regionTag = 0;

};

/*
  Point struct and feew functions to work with it
*/
class GeomPoint
{
  public:
    double x, y;
    int branchId = 0, regionTag = 0;
    double meshSize = 1.;
    GeomPoint() = default;
    ~GeomPoint() = default;
    GeomPoint(double xval, double yval, 
        int branchIdVal = 0, int regionTagVal = 0, double msize = 1.);
    GeomPoint(GeomPolar &p);

    double norm() const;
    GeomPoint getNormalized();
    GeomPoint& rotate(double phi);
    GeomPolar getPolar() const;
    void normalize();
    double angle() const;
    double angle(GeomPoint p) const;
    void print() const;

    GeomPoint& operator=(const GeomPoint& p) = default;
    GeomPoint operator+(const GeomPoint& p) const;
    GeomPoint& operator+=(const GeomPoint& p);
    GeomPoint operator-(const GeomPoint& p) const;
    GeomPoint& operator-=(const GeomPoint& p);
    double operator*(const GeomPoint& p) const;
    GeomPoint operator*(const double gain) const;
    GeomPoint& operator*=(const double gain);
    GeomPoint operator/(const double gain) const;
    GeomPoint& operator/=(const double gain);
    bool operator==(const GeomPoint& p) const;
    friend ostream& operator<<(ostream& write, const GeomPoint & p);
};



class Branch
{
public:
  unsigned long int id;
  string name = "branch";
  vector<GeomPoint> leftPoints;
  vector<GeomPoint> rightPoints;

  Branch(unsigned long int id, GeomPoint sourcePoint, double phi, double epsVal = 1e-10);
  ~Branch() = default;

  //modify branch
  void addPoint(GeomPoint p);
  void addDPoint(GeomPoint p);
  void addPolar(GeomPolar p, bool bRelativeAngle = true);
  void removeHeadPoint();
  void shrink(double dl);
  double width();
  void setWidth(double epsVal);

  //geom entities
  GeomPoint getHead();
  double getHeadAngle();
  double getTailAngle();
  GeomPoint getTail();

  //statistics
  bool empty();
  double length();
  unsigned int size();
  double averageSpeed();
  //lot of others to be implemented

  //debug
  void print();

private:
  pair<GeomPoint, GeomPoint> splitPoint(GeomPoint p, double phi);
  GeomPoint mergePoints(GeomPoint p1, GeomPoint p2);
  double eps = 3e-2;
  double tailAngle;
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


  //branches functionality
  map<unsigned int, pair<unsigned int, unsigned int>> branchRelation;
  map<unsigned int, unsigned int> branchIndexes;
  vector<Branch> branches;

  //Segments or Edges or Elements

  Geometry() = default;
  ~Geometry() = default;

  Branch& initiateRootBranch(unsigned int id = 1);
  void addPoints(vector<GeomPoint> points);
  void addDPoints(vector<GeomPoint> dpoints);
  void addPolar(GeomPolar p, bool bRelativeAngle = true);
  void SetSquareBoundary(
      GeomPoint BottomBoxCorner,
      GeomPoint TopBoxCorner,
      double dx);

  //TODO implement constraint on eps and dl values!!!
  //important
  pair<unsigned int, unsigned int> AddBiffurcation(unsigned int id, double dl);

  vector<unsigned int> GetTipIds();
  Branch& GetBranch(unsigned int id);
  vector<GeomPolar> GetTipPolars();
  void InitiateMesh(tethex::Mesh & meshio);
  void SetEps(double epsVal);


private:
  double eps = 3e-2;
  double bifAngle = M_PI/5.;

  //mesh size of different regions
  double tipMeshSize = 0.003;
  double riverMeshSize = 0.01;
  double boundariesMeshSize = 0.1;

  unsigned int rootBranchId = 0;//0 means no root/first Branch
  
  //Nodes and lines - main interface of this class
  vector<GeomPoint> points;
  vector<GeomLine> lines;

  //Boundary
  //Box parameters
  vector<GeomPoint> boundaryPoints;
  vector<GeomLine> boundaryLines;
  double dx = 0.5;


  //river geometry parameters
  double alpha = M_PI/3.; // biffurcation angle
  double len = 0.1; // biffurcation initial length 
  
  pair<GeomPoint, double> GetEndPointOfSquareBoundary();
  void InserBranchTree(unsigned int id, double phi, bool isRoot = false);
  void generateCircularBoundary();
  //at generation time of whole circular boundary
  //it gives us a points of crossection of boundaries
  GeomPoint mergedLeft(double phi);
  GeomPoint mergedRight(double phi);
  GeomPoint mergedCenter(double phi);
  unsigned int generateID(unsigned int prevID, bool isRight = false);
};

} // namespace River