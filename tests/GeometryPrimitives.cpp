//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BaseClassModule"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <math.h>
#include "GeometryPrimitives.hpp"

const double eps = EPS;

namespace utf = boost::unit_test;
using namespace River;

BOOST_AUTO_TEST_CASE( Get_Normalized_Point, 
    *utf::description("GetNormalizedPoint")
    *utf::tolerance(eps))
{
    Point 
        a = {1, 1}, 
        b = {10, 10}, 
        c = {2, 0}, 
        d = {0, 0},
        e = {-2, 0};

    BOOST_TEST((GetNormalizedPoint(a) == Point{1/sqrt(2), 1/sqrt(2)}));
    BOOST_TEST((GetNormalizedPoint(b) == Point{1/sqrt(2), 1/sqrt(2)}));
    BOOST_TEST((GetNormalizedPoint(c) == Point{1, 0}));
    BOOST_CHECK_THROW(GetNormalizedPoint(d), Exception);
    BOOST_TEST((GetNormalizedPoint(e) == Point{-1, 0}));
}

BOOST_AUTO_TEST_CASE( To_Polar, 
    *utf::description("ToPolar")
    *utf::tolerance(eps))
{
    Point 
        a = {1, 0}, 
        b = {1, 1}, 
        c = {0, 1}, 
        d = {-1, 1},
        e = {-1, 0},
        f = {-1, -1}, 
        g = {0, -1},
        k = {1, -1}, 
        l = {0, 0};
    
    Polar
        pa = {1, 0},
        pb = {sqrt(2), M_PI/4},
        pc = {1, M_PI/2},
        pd = {sqrt(2), 3*M_PI/4},
        pe = {1, M_PI},
        pf = {sqrt(2), -3*M_PI/4},
        pg = {1, -M_PI/2},
        pk = {sqrt(2),  - M_PI/4},
        pl = {0, 0};
        

    BOOST_TEST(ToPolar(a) == pa);
    BOOST_TEST(ToPolar(b) == pb);
    BOOST_TEST(ToPolar(c) == pc);
    BOOST_TEST(ToPolar(d) == pd);
    BOOST_TEST(ToPolar(e) == pe);
    BOOST_TEST(ToPolar(g) == pg);
    BOOST_TEST(ToPolar(k) == pk);
    BOOST_CHECK_THROW(ToPolar(l), Exception);
}

BOOST_AUTO_TEST_CASE( polar, 
    *utf::description("polar class")
    *utf::tolerance(eps))
{
    //constructor
    Polar 
        a,
        c{2, 3},
        b{2, 3};

    BOOST_TEST(a.r == 0);
    BOOST_TEST(a.phi == 0);
    BOOST_TEST(c.r == 2);
    BOOST_TEST(c.phi == 3);

    //equal and not equal operator
    BOOST_TEST(a != c);
    BOOST_TEST(a == a);
    BOOST_TEST(c == c);
    BOOST_TEST(b == c);
}

BOOST_AUTO_TEST_CASE( point_constructor, 
    *utf::description("pooint class constructor")
    *utf::tolerance(eps))
{
    //constructors and copyconstructors
    Point 
        a,
        b{1, 2},
        c{b},
        d{Polar{10., M_PI/4.}},
        e{Polar{10., M_PI/2.}};

    BOOST_TEST(a.x == 0.);
    BOOST_TEST(a.y == 0.);
    BOOST_TEST(b.x == 1.);
    BOOST_TEST(b.y == 2.);
    BOOST_TEST(c.x == 1.);
    BOOST_TEST(c.y == 2.);
    BOOST_TEST(d.x == 10./sqrt(2.));
    BOOST_TEST(d.y == 10./sqrt(2.));
    BOOST_TEST(e.x == 0.);
    BOOST_TEST(e.y == 10.);

    //point and polar assignment
    Point 
        f = b,
        g,
        k = Polar{sqrt(2.), -M_PI/4.},
        keq = {1, -1};
    
    BOOST_TEST(f == b);
    
    g = c;
    BOOST_TEST(g == c);
    
    BOOST_TEST(k == keq);

    g = k;
    BOOST_TEST(g == keq);
}

BOOST_AUTO_TEST_CASE( point_methods, 
    *utf::description("pooint class methods")
    *utf::tolerance(eps))
{
    //norm
    Point 
        a{0, 0},
        b{2, 3},
        c{-3, -3};

    BOOST_TEST(a.norm() == 0.);
    BOOST_TEST(b.norm() == sqrt(2.*2. + 3.*3.));
    BOOST_TEST(c.norm() == 3.*sqrt(2.));
    
    BOOST_TEST(a.norm() == Point::norm(a.x, a.y));
    BOOST_TEST(b.norm() == Point::norm(b.x, b.y));
    BOOST_TEST(c.norm() == Point::norm(c.x, c.y));
    
    //rotate and angle
    c = Point{0, 0};
    BOOST_CHECK_THROW(c.angle(), Exception);

    a.rotate(rand()/RAND_MAX*100.);
    BOOST_TEST((a.x == 0. && a.y == 0.));
    
    BOOST_TEST(b.angle() == atan(3./2.));
    b.rotate(M_PI/2);
    BOOST_TEST((b.x == -3. && b.y == 2.));
    BOOST_TEST(b.angle() == atan(3./2.) + M_PI/2);

    b.rotate(M_PI/2);
    BOOST_TEST((b.x == -2. && b.y == -3));
    BOOST_TEST(b.angle() == atan(3./2.) - M_PI);

    b.rotate(M_PI/2);
    BOOST_TEST((b.x == 3. && b.y == -2));
    BOOST_TEST(b.angle() == atan(3./2.) - M_PI/2);

    b.rotate(M_PI/2);
    BOOST_TEST((b.x == 2. && b.y == 3.));
    BOOST_TEST(b.angle() == atan(3./2.));

    //normalize
    a = {0, 0};
    BOOST_CHECK_THROW(a.normalize(), Exception);

    b = {2, 3};
    b.normalize();
    BOOST_TEST(b.x == 2./sqrt(2.*2. + 3.*3.));
    BOOST_TEST(b.y == 3./sqrt(2.*2. + 3.*3.));

    c = {-3, -3};
    c.normalize();
    BOOST_TEST(c.x == -1/sqrt(2));
    BOOST_TEST(c.y == -1/sqrt(2));
}

BOOST_AUTO_TEST_CASE( point_static_methods, 
    *utf::tolerance(eps)
    *utf::description("point class static methods"))
{
    //norm 
    BOOST_TEST(Point::norm(1., 1.)  == sqrt(2.));
    BOOST_TEST(Point::norm(-1., 1.) == sqrt(2.));
    BOOST_TEST(Point::norm(1., -1.) == sqrt(2.));
    BOOST_TEST(Point::norm(0., 0.)  == sqrt(0.));

    //angle lies between [-Pi, +Pi]
    BOOST_CHECK_THROW(Point::angle(0., 0.), Exception);

    BOOST_TEST(Point::angle(1., 0.)  == 0.);
    BOOST_TEST(Point::angle(1., 1.)  == M_PI/4.);
    BOOST_TEST(Point::angle(0., 1.)  == M_PI/2.);
    BOOST_TEST(Point::angle(-1., 1.) == 3*M_PI/4.);
    BOOST_TEST(Point::angle(-1., 0.)  == M_PI);
    BOOST_TEST(Point::angle(-1., -1.) == -3*M_PI/4.);
    BOOST_TEST(Point::angle(0., -1.)  == -M_PI/2.);
    BOOST_TEST(Point::angle(1., -1.)  == -M_PI/4.);
}

BOOST_AUTO_TEST_CASE(point_angle_between_points, 
    *utf::tolerance(eps)
    *utf::description("angle between points"))
{
    Point 
        a{1, 0},
        b{1, 0};
    
    for(unsigned i = 0; i < 20; ++i)
    {
        if(a.angle() <= 0.)
            BOOST_TEST(a.angle(b) == i*M_PI/10);
        else
            BOOST_TEST(a.angle(b) == i*M_PI/10 - 2*M_PI);
        a.rotate(-M_PI/10);
    }

    for(unsigned i = 0; i < 20; ++i)
    {
        if(b.angle() >= 0.)
            BOOST_TEST(a.angle(b) == i*M_PI/10);
        else
            BOOST_TEST(a.angle(b) == i*M_PI/10 - 2*M_PI);
        b.rotate(M_PI/10);
    }

    //edge condition check
    a = {0, 0};
    BOOST_CHECK_THROW(a.angle(b), Exception);

    a = {1, 0};
    b = {0, 0};
    BOOST_CHECK_THROW(a.angle(b), Exception);

    a = {0, 0};
    BOOST_CHECK_THROW(a.angle(b), Exception);
}

BOOST_AUTO_TEST_CASE(point_operators, 
    *utf::tolerance(eps)
    *utf::description("point operators test"))
{
    //mathematical operations
    Point 
        a{1, 2},
        b{3, 4};
    
    a = a+= (a + a)/2 - a + (a*(-2) + a*2);
    BOOST_TEST((a.x == 1. && a.y == 2.));

    a = {1, 2};
    a /= 2;
    a *= 2;
    BOOST_TEST((a.x == 1. && a.y == 2.));

    a = {1, 2};
    BOOST_TEST((a[0] == 1. && a[1] == 2.));
    BOOST_CHECK_THROW(a[3], Exception);

    //scalar product
    a = {1, 2};
    b = {3, 4};
    BOOST_TEST(a*b == 1.*3. + 2.*4.);
}