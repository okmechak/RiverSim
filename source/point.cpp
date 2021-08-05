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

#include "point.hpp"

namespace River
{
    double acos_s(const double x)
    {
        if(x > 1.)
            return acos(1);
        else if(x < -1.)
            return acos(-1);
        return acos(x);
    }

    void print(const bool flag, const string str)
    {
    	if (flag) cout << str << endl;
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

    Point &Point::rotate(const double phi)
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

    double Point::norm(const double x, const double y)
    {
    	return sqrt(x * x + y * y);
    }

    Point &Point::normalize()
    {
    	auto l = norm();
    	if (l < eps)
    		throw Exception("norm is equal to zero");

    	x /= l;
    	y /= l;

    	return *this;
    }

    double Point::angle() const
    {
    	auto n = norm();

    	if (n < eps)
    		throw Exception("norm is less then zero");

    	double phi = acos_s(x / n);
    	if (y < 0)
    		phi = -phi;
    	return phi;
    }

    double Point::angle(const double x, const double y)
    {
    	if (abs(x) < eps && abs(y) < eps)
    		throw Exception("norm is less then zero");

    	double phi = acos_s(x / norm(x, y));
    	if (y < 0)
    		phi = -phi;

    	return phi;
    }

    double Point::angle(const Point &p) const
    {
    	//order of points is important
    	auto n = norm();
    	auto pn = p.norm();
    	if (n < eps || pn < eps)
    		throw Exception("norm of one or another vector is 0");

    	double phi = acos_s((x * p.x + y * p.y) / n / pn);
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

    Point operator*(const double gain, const Point& p)
    {
        return Point{gain * p.x, gain * p.y};
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

    Point operator/(const double gain, const Point& p)
    {
        return Point{gain / p.x, gain / p.y};
    }

    Point &Point::operator/=(const double gain)
    {
    	x /= gain;
    	y /= gain;
    	return *this;
    }

    double& Point::operator[](const int index)
    {
    	if (index == 0)
    		return x;
    	else if (index == 1)
    		return y;
    	//else if (index == 2)
        //    return 0;/*z coord equals 0*/
        else
    		throw Exception("index should be 0 or 1");
    }

    double Point::operator[](const int index) const
    {
    	if (index == 0)
    		return x;
    	else if (index == 1)
    		return y;
    	//else if (index == 2)
        //    return 0;/*z coord equals 0*/
        else
    		throw Exception("index should be 0 or 1");
    }

    Point Point::getNormalized() const
    {
        auto n = norm();
    	if (n < EPS)
    		throw Exception("norm is equal to zero");

    	return Point{x / n, y / n};
    }

    ostream &operator<<(ostream &write, const Point &p)
    {
    	write << "point: " << p.x << ", " << p.y;
    	return write;
    }
} // namespace River