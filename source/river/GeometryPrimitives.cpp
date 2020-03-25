/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "GeometryPrimitives.hpp"

///\cond
#include <math.h>
///\endcond

namespace River
{
void print(bool flag, string str)
{
	if (flag)
		cout << str << endl;
}

Point GetNormalizedPoint(Point p)
{
	auto n = p.norm();
	if (n == 0.)
		throw Exception("norm is equal to zero");

	return Point{p.x / n, p.y / n};
}

Polar ToPolar(Point p)
{
	return Polar{p.norm(), p.angle()};
}

Point::Point(double xval, double yval)
{
	x = xval;
	y = yval;
}

Point::Point(const Polar &p)
{
	x = p.r * cos(p.phi);
	y = p.r * sin(p.phi);
}

Point &Point::rotate(double phi)
{
	auto tempx = x,
		 tempy = y;
	x = tempx * cos(phi) - tempy * sin(phi);
	y = tempx * sin(phi) + tempy * cos(phi);

	return *this;
}

double Point::norm() const
{
	return sqrt(x * x + y * y);
}

double Point::norm(double x, double y)
{
	return sqrt(x * x + y * y);
}

Point &Point::normalize()
{
	auto l = norm();
	if (l == 0.)
		throw Exception("norm is equal to zero");
	x /= l;
	y /= l;

	return *this;
}

double Point::angle() const
{
	auto n = norm();

	if (n == 0.)
		throw Exception("vector doesn't have arc");

	double phi = acos(x / n);
	if (y < 0)
		phi = -phi;
	return phi;
}

double Point::angle(double x, double y)
{
	if (x == 0. && y == 0.)
		throw Exception("vector doesn't have arc");

	double phi = acos(x / norm(x, y));
	if (y < 0)
		phi = -phi;

	return phi;
}

double Point::angle(const Point &p) const
{
	//order of points is important
	auto n = norm();
	auto pn = p.norm();
	if (n == 0. || pn == 0.)
		throw Exception("norm of one or another vector is 0");

	double phi = acos((x * p.x + y * p.y) / n / pn);
	double sign = x * p.y - p.x * y > 0 ? 1 : -1;
	phi *= sign;
	return phi;
}

bool Point::operator==(const Point &p) const
{
	return (abs(x - p.x) < eps) && (abs(y - p.y) < eps);
}

Point Point::operator+(const Point &p) const
{
	return Point{x + p.x, y + p.y};
}

Point &Point::operator+=(const Point &p)
{
	x += p.x;
	y += p.y;
	return *this;
}

Point Point::operator-(const Point &p) const
{
	return Point{x - p.x, y - p.y};
}

Point &Point::operator-=(const Point &p)
{
	x -= p.x;
	y -= p.y;
	return *this;
}

double Point::operator*(const Point &p) const
{
	return (x * p.x + y * p.y);
}

Point Point::operator*(const double gain) const
{
	return Point{x * gain, y * gain};
}

Point &Point::operator*=(const double gain)
{
	x *= gain;
	y *= gain;
	return *this;
}

Point Point::operator/(const double gain) const
{
	return Point{x / gain, y / gain};
}

Point &Point::operator/=(const double gain)
{
	x /= gain;
	y /= gain;
	return *this;
}

double Point::operator[](const int index) const
{
	if (index == 0)
		return x;
	else if (index == 1)
		return y;
	else
		throw Exception("index should be 0 or 1");
}

ostream &operator<<(ostream &write, const Point &p)
{
	write << "point: " << p.x << ", " << p.y;
	return write;
}

} // namespace River