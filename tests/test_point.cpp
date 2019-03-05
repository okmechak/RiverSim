//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BaseClassModule"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <math.h>
#include "common.hpp"

const double eps = 1e-13;

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
using namespace River;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructors, 
  *utf::description("constructors")
  *utf::tolerance(eps))
{

  //constructor
  Point obj1;
  BOOST_TEST((obj1.x == 0. && obj1.y == 0.));
  BOOST_TEST(obj1.norm() == 0.);

  Point obj2{1, 1};
  BOOST_TEST((obj2.x == 1. && obj2.y == 1.));
  BOOST_TEST(obj2.norm() == sqrt(2.));

  Point obj3(2, 3);
  BOOST_TEST((obj3.x == 2. && obj3.y == 3.));
  BOOST_TEST(obj3.norm() == sqrt(13.));

  //copy constructor
  Point obj4(obj3);
  BOOST_TEST((obj4.x == 2. && obj4.y == 3.));
  BOOST_TEST(obj4.norm() == sqrt(13.));

  //assign 
  auto obj5 = obj4;
  BOOST_TEST((obj5.x == 2. && obj5.y == 3.));
  BOOST_TEST(obj5.norm() == sqrt(13.));

  //polar coord
  Point obj6(Polar{sqrt(2.), M_PI/4.});
  BOOST_TEST(obj6.x == 1.);
  BOOST_TEST(obj6.y == 1.);
}



BOOST_AUTO_TEST_CASE( static_methods, 
  *utf::tolerance(eps)
  *utf::description("static methods"))
{
  //norm 
  BOOST_TEST(Point::norm(1., 1.) == sqrt(2.));
  BOOST_TEST(Point::norm(-1., 1.) == sqrt(2.));
  BOOST_TEST(Point::norm(1., -1.) == sqrt(2.));
  BOOST_TEST(Point::norm(0., 0.) == sqrt(0.));

  //angle lies between [-Pi, +Pi]
  BOOST_TEST(Point::angle(1., 0.) == 0.);
  BOOST_TEST(Point::angle(1., 1.) == M_PI/4.);
  BOOST_TEST(Point::angle(0., 1.) == M_PI/2.);
  BOOST_TEST(Point::angle(-1., 1.) == 3*M_PI/4.);
  BOOST_TEST((Point::angle(-1., 0.) == M_PI 
    || Point::angle(-1., 0.) == -M_PI));
  BOOST_TEST(Point::angle(-1., -1.) == -3*M_PI/4.);
  BOOST_TEST(Point::angle(0., -1.) == -M_PI/2.);
  BOOST_TEST(Point::angle(1., -1.) == -M_PI/4.);

  BOOST_CHECK_THROW(Point::angle(0., 0.), std::invalid_argument);
}



BOOST_AUTO_TEST_CASE(methods, 
  *utf::tolerance(eps)
  *utf::description("methods")
)
{
  //norm and angle
  Point obj(1, 2);
  BOOST_TEST(obj.norm() == sqrt(5.));
  BOOST_TEST(obj.angle() == atan(2.));
  BOOST_TEST(obj.angle(obj) == 0.);
  Point obj_angl(1, -2);
  BOOST_TEST(obj.angle(obj_angl) == - 2*atan(2.));
  Point a(0, 0);
  BOOST_CHECK_THROW(a.angle(), std::invalid_argument);
  BOOST_CHECK_THROW(a.normalize(), std::invalid_argument);
  BOOST_CHECK_THROW(a.getNormalized(), std::invalid_argument);
  BOOST_CHECK_THROW(obj.angle(a), std::invalid_argument);

  //getNormalized
  Point obj2 = obj.getNormalized();
  BOOST_TEST((obj2.x == 1./sqrt(5.) && obj2.y == 2./sqrt(5.)));

  //normalize
  obj.normalize();
  BOOST_TEST(obj2 == obj);
}



BOOST_AUTO_TEST_CASE(operators, 
  *utf::tolerance(eps)
  *utf::description("operators")
)
{
  Point a(1, 1);

  a += (a + a)/*2*a*/ +( a*(-2) + a*2)/*0*/ 
    + ( a /(-2) + a/2)/*0*/;
  a -= a/3;
  a /= 2;
  BOOST_TEST((a.x == 1. && a.y == 1.));
  BOOST_TEST((a[0] == 1. && a[1] == 1.));
  BOOST_CHECK_THROW(a[2], std::invalid_argument);
}


BOOST_AUTO_TEST_CASE( polar_test, 
  *utf::description("polar constructor")
  *utf::tolerance(eps))
{
  Polar p;
  BOOST_TEST((p.dl == 0. && p.phi == 0.));
  
  p = Polar{1, 2};
  BOOST_TEST((p.dl == 1. && p.phi == 2.));
}