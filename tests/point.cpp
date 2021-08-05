//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Points(Vector), Polar and their transforamtion test"

#include "point.hpp"
//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

const double eps = EPS;

namespace utf = boost::unit_test;
namespace bdata = boost::unit_test::data;
using namespace River;

//---------------------------------------------------------
// Point class tests
//---------------------------------------------------------
BOOST_TEST_DECORATOR( *utf::description("constructors and copy constructor and assignment") *utf::tolerance(eps) )
BOOST_DATA_TEST_CASE( point_constructor, bdata::xrange<double>(-5., 5., 0.5) * bdata::xrange<double>(-5., 5., 0.5), x, y)
{
    //norm
    Point 
        a{x, y}, //construcor from coords
        b{a}; // copy construcotr
    Polar p{x, y}; 
    Point 
        c{p}, // construcotr from polar coords
        d = p, // Polar to point assignment
        e = a;

    BOOST_TEST(a.x == x);
    BOOST_TEST(a.y == y);

    BOOST_TEST(b.x == x);
    BOOST_TEST(b.y == y);

    BOOST_TEST(c.x == x * cos(y));
    BOOST_TEST(c.y == x * sin(y));

    BOOST_TEST(d.x == x * cos(y));
    BOOST_TEST(d.y == x * sin(y));

    BOOST_TEST(e.x == x);
    BOOST_TEST(e.y == y);
}

BOOST_TEST_DECORATOR(*utf::description("norma and static norma")*utf::tolerance(eps))
BOOST_DATA_TEST_CASE(norm, bdata::xrange(-5., 5., 0.5) * bdata::xrange(-5., 5., 0.5), x, y)
{
    //norm
    Point a{x, y};

    BOOST_TEST(a.norm() == sqrt(x*x + y*y));
    BOOST_TEST(Point::norm(a.x, a.y) == sqrt(x*x + y*y));
}


BOOST_TEST_DECORATOR( * utf::description("rotate point") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(rotate, bdata::xrange(-500., 500., 5.1), angle_in_radians)
{
    Point a{1, 1};

    a.rotate(angle_in_radians);

    BOOST_TEST(a.x == cos(angle_in_radians) - sin(angle_in_radians));
    BOOST_TEST(a.y == sin(angle_in_radians) + cos(angle_in_radians));

}


BOOST_TEST_DECORATOR( * utf::description("angle of point and its static implementation") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(angle, bdata::xrange(-5., 5., 1.) * bdata::xrange(-5., 5., 1.), x, y)
{
    Point a{x, y};

    if(x == 0. && y == 0.) {

        BOOST_CHECK_THROW(a.angle(), Exception);
        BOOST_CHECK_THROW(Point::angle(x, y), Exception);

    }
    else {

        double 
            p_angle = a.angle(),
            static_angle = Point::angle(x, y);

        BOOST_TEST(static_angle == p_angle);

        double phi = acos(x / sqrt(x*x + y*y));
    	if (y < 0)
    		phi = -phi;

        BOOST_TEST(p_angle == phi);

    }
}


BOOST_TEST_DECORATOR( * utf::description("nomalize") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(nomalize, bdata::xrange(-5., 5., 0.1) * bdata::xrange(-5., 5., 0.1), x, y)
{
    Point a{x, y};
    if (abs(x) <= EPS && abs(y) <= EPS)
        BOOST_CHECK_THROW(a.normalize(), Exception);
    else {
        a.normalize();
        double len = sqrt(x*x + y*y);
        BOOST_TEST(a.x == x / len);
        BOOST_TEST(a.y == y / len);
    }
}


BOOST_TEST_DECORATOR( * utf::description("angle between points") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(angle_between_points, bdata::xrange(-2., 2., 1.) * bdata::xrange(-2., 2., 1.) * bdata::xrange(-2., 2., 1.) * bdata::xrange(-2., 2., 1.), x1, y1, x2, y2)
{
    Point 
        a{x1, y1},
        b{x2, y2};  
    
    if (a.norm() <= EPS || b.norm() <= EPS )

        BOOST_CHECK_THROW(a.angle(b), Exception);

    else {

        double 
    	    expected_angle = x1 * y2 - x2 * y1 > 0 
                ? acos((x1 * x2 + y1 * y2) / sqrt(x1 * x1 + y1 * y1) / sqrt(x2 * x2 + y2 * y2)) 
                : - acos((x1 * x2 + y1 * y2) / sqrt(x1 * x1 + y1 * y1) / sqrt(x2 * x2 + y2 * y2)),
            actual_angle = a.angle(b);

            BOOST_TEST(actual_angle == expected_angle); 
    }
}


BOOST_TEST_DECORATOR( * utf::description("point operators test") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(point_operators, bdata::xrange(-2., 2., 0.5) * bdata::xrange(-2., 2., 0.5) * bdata::xrange(-2., 2., 0.5) * bdata::xrange(-2., 2., 0.5), x1, y1, x2, y2)
{
    //mathematical operations
    Point 
        a{x1, y1},
        b{x2, y2},
        c;

    double alpha = y1 + 10;

    // multiplication and division by scalar
    c = a*alpha;
    BOOST_TEST(c.x == x1 * alpha);
    BOOST_TEST(c.y == y1 * alpha);

    c = alpha * a;
    BOOST_TEST(c.x == x1 * alpha);
    BOOST_TEST(c.y == y1 * alpha);

    c = a / alpha;
    BOOST_TEST(c.x == x1 / alpha);
    BOOST_TEST(c.y == y1 / alpha);

    if(x1 != 0. && y1 != 0.) {
        c = alpha / a;
        BOOST_TEST(c.x == alpha / x1);
        BOOST_TEST(c.y == alpha / y1);
    }

    c = a;
    c *= alpha;
    BOOST_TEST(c.x == x1 * alpha);
    BOOST_TEST(c.y == y1 * alpha);

    c = a;
    c /= alpha;
    BOOST_TEST(c.x == x1 / alpha);
    BOOST_TEST(c.y == y1 / alpha);

    // point addition
    c = a + b;
    BOOST_TEST(c.x == x1 + x2);
    BOOST_TEST(c.y == y1 + y2);

    c = a;
    c += b;
    BOOST_TEST(c.x == x1 + x2);
    BOOST_TEST(c.y == y1 + y2);

    // point difference
    c = a - b;
    BOOST_TEST(c.x == x1 - x2);
    BOOST_TEST(c.y == y1 - y2);

    c = a;
    c -= b;
    BOOST_TEST(c.x == x1 - x2);
    BOOST_TEST(c.y == y1 - y2);

    //comparison
    BOOST_TEST((a == b) == (x1 == x2 && y1 == y2));

    // scalar product difference
    BOOST_TEST(a * b == x1 * x2 + y1 * y2);

    //operators
    BOOST_TEST(b[0] == x2);
    BOOST_TEST(b[1] == y2);

    //asignment by parentheses
    c[0] = x1;
    c[1] = x2;
    BOOST_TEST(c.x == x1);
    BOOST_TEST(c.y == x2);
}

BOOST_TEST_DECORATOR( * utf::description("get nomalized point") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(get_normalized, bdata::xrange(-5., 5., 1.) * bdata::xrange(-5., 5., 1.), x, y)
{
    Point a{x, y};
    if (x == 0. && y == 0.)
        BOOST_CHECK_THROW(a.getNormalized(), Exception);
    else {
        auto b = a.getNormalized();
        double len = sqrt(x*x + y*y);
        BOOST_TEST(b.x == x / len);
        BOOST_TEST(b.y == y / len);
    }
}

//---------------------------------------------------------
// Polar class tests
//---------------------------------------------------------

BOOST_TEST_DECORATOR( * utf::description("Polar constructors") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(polar_constructors, bdata::xrange(-5., 5., 1.) * bdata::xrange(-5., 5., 1.), r, phi)
{
    Point 
        a = {r, phi};
    
    Polar
        p1, 
        p2 = {r, phi}, 
        p3 = a;
        

    BOOST_TEST(p2.r == r);
    BOOST_TEST(p2.phi == phi);

    BOOST_TEST(p3.r == a.norm());
    if(p3.r >= EPS)
        BOOST_TEST(p3.phi == a.angle());
    else
        BOOST_TEST(p3.phi == 0.);
}

BOOST_TEST_DECORATOR( * utf::description("Polar comparison") * utf::tolerance(eps))
BOOST_DATA_TEST_CASE(polar_comparison, bdata::xrange(-1., 1., 1.) * bdata::xrange(-1., 1., 1.) * bdata::xrange(-1., 1., 1.) * bdata::xrange(-1., 1., 1.), r1, phi1, r2, phi2)
{
    Polar
        p1{r1, phi1}, 
        p2{r2, phi2};
        
    BOOST_TEST((p1 == p2) == (r1 == r2 && phi1 == phi2));
    BOOST_TEST((p1 != p2) == !(r1 == r2 && phi1 == phi2));

}