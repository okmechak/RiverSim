/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
 */

/*! \file common.hpp
    @{
    Common geometric entities to whole program.

    Like Point(or Vector), Polar - same vector but in Polar coordinates and simple
    Line element.
    @}
 */
#pragma once

#include <vector>
#include <iostream>

using namespace std;


/*!
    Hash pairing function used for unordered map in Solver::TryInsertCellBoundary()
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
  ///Prints logs depending of log configuration(quiet or verbose)
  void print(bool flag, string str);


  /**
   * Vector represented in polar coordinates.
   */
  class Polar
  {
    public:

      Polar() = default;
      Polar(double dlval, double phival):
        r{dlval}, phi{phival} {};
      ///radius of points
      double r = 0.;
      ///angle of point
      double phi = 0.;

      friend ostream& operator <<(ostream& write, const Polar & p)
      {
        cout << "Polar: " << p.r << " " << p.phi << endl;
        return write;
      }
  };

  /**
   * Point struct and feew functions to work with it.
   */
  class Point
  {
    private:
      ///Absolute precission parameter, used in Point comparsion
      double eps = 1e-13;

    public:
      ///Point coordinates;
      double x = 0, y = 0;

      Point() = default;
      ~Point() = default;
      Point(double xval, double yval);
      Point(const Point &p) = default;
      ///Converts Polar coordinates to Cartesian of Point.
      Point(const Polar &p);

      ///Returns norm of vector.
      double norm() const;

      ///Evaluates norm of vector {x, y}.
      static double norm(double x, double y);

      ///Returns normalized vector of current Point.
      Point getNormalized();

      ///Rotates point on __phi__ angle(counterclockwise)
      Point& rotate(double phi);

      ///Returns Polar representation of vector.
      Polar getPolar() const;

      ///Normalizes current Point.
      Point& normalize();

      ///Returns angle.
      double angle() const;

      ///Returns angle of {x, y} vector.
      static double angle(double x, double y);

      ///Returns angle relatively to __p__ Point
      double angle(const Point &p) const;

      ///Prints point
      void print() const;

      /**
      * @name Math operations
      * @{
      */
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
      /**
       * @}
       */

      friend ostream& operator <<(ostream& write, const Point & p);
  };

  ///Line - holds indexes of __p1__ and __p2__ vertices.
  class Line
  {
    public:
      Line() = default;
      Line(long unsigned p1v, long unsigned p2v, int idv):p1(p1v), p2(p2v), id(idv){};
      long unsigned p1, p2;
      int id;
  };

} // namespace River