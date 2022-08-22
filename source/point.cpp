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

    Point GetNormalizedPoint(const Point &p)
    {
    	auto n = p.norm();
    	if (n < EPS)
    		throw Exception("norm is equal to zero");

    	return Point{p.x / n, p.y / n};
    }

    Polar ToPolar(const Point& p)
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
    	if (l < EPS)
    		throw Exception("norm is equal to zero");
    	x /= l;
    	y /= l;

    	return *this;
    }

    double Point::angle() const
    {
    	auto n = norm();

    	if (n < EPS)
    		throw Exception("norm is less then zero");

    	double phi = acos_s(x / n);
    	if (y < 0)
    		phi = -phi;
    	return phi;
    }

    double Point::angle(const double x, const double y)
    {
    	if (abs(x) < EPS && abs(y) < EPS)
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
    	if (n < EPS || pn < EPS)
    		throw Exception("Point::angle: norm of one or another vector is 0");

    	double phi = acos_s((x * p.x + y * p.y) / n / pn);
    	double sign = x * p.y - p.x * y > 0 ? 1 : -1;
    	phi *= sign;
    	return phi;
    }

    bool Point::operator==(const Point &p) const
    {
    	return (abs(x - p.x) < EPS) && (abs(y - p.y) < EPS);
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
    	else if (index == 2)
            return 0;/*z coord equals 0*/
        else
    		throw Exception("index should be 0 or 1");
    }
} // namespace River