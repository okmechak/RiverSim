//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "GeomPoint Test Module"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_parameters.hpp>

#include <iostream>
#include <math.h>

#include "geometry.hpp"

const auto eps = 1e-13;

namespace utf = boost::unit_test;
using namespace River;

// ------------- Tests Follow --------------

BOOST_AUTO_TEST_CASE(geom_tag, *utf::tolerance(eps))
{
  GeomTag gt;
  BOOST_TEST((gt.branchId == 0 && gt.regionTag == 0));

  auto gt_new = GeomTag{1, 2};
  BOOST_TEST((gt_new.branchId == 1 && gt_new.regionTag == 2));
}



BOOST_AUTO_TEST_CASE( geom_polar, *utf::tolerance(eps))
{
  //default constructor
  GeomPolar gp;
  BOOST_TEST((gp.r == 0. && gp.phi == 0. 
    && gp.branchId == 0 && gp.regionTag == 0 
    && gp.meshSize == 1. ));
  
  //constructor
  gp = GeomPolar{1, 2, 3, 4, 5};
  BOOST_TEST((gp.r == 1. && gp.phi == 2. 
    && gp.branchId == 3 && gp.regionTag == 4 
    && gp.meshSize == 5. ));
}



BOOST_AUTO_TEST_CASE(geom_line, *utf::tolerance(eps))
{
  //constructor
  GeomLine gl(1, 2, 3, 4);
  BOOST_TEST((gl.p1 == 1 && gl.p2 == 2 
    && gl.branchId == 3 && gl.regionTag == 4));
}



BOOST_AUTO_TEST_CASE( geom_point_constructors, 
  *utf::description("constructors")
  *utf::tolerance(eps))
{

  //constructor
  GeomPoint obj2{1, 1};
  BOOST_TEST((obj2.x == 1. && obj2.y == 1. 
    && obj2.branchId == 0 && obj2.regionTag == 0
    && obj2.meshSize == 1.));
  BOOST_TEST(obj2.norm() == sqrt(2.));

  GeomPoint obj3(2, 3, 4, 5, 6);
  BOOST_TEST((obj3.x == 2. && obj3.y == 3. 
    && obj3.branchId == 4 && obj3.regionTag == 5 
    && obj3.meshSize == 6.));
  BOOST_TEST(obj3.norm() == sqrt(13.));

  //copy constructor
  GeomPoint obj4(obj3);
  BOOST_TEST((obj4.x == 2. && obj4.y == 3. 
    && obj4.branchId == 4 && obj4.regionTag == 5 
    && obj4.meshSize == 6.));
  BOOST_TEST(obj4.norm() == sqrt(13.));

  //assign 
  auto obj5 = obj4;
  BOOST_TEST((obj5.x == 2. && obj5.y == 3.));
  BOOST_TEST(obj5.norm() == sqrt(13.));

  //polar coord
  GeomPoint obj6(GeomPolar{sqrt(2.), M_PI/4., 11, 12, 13});
  BOOST_TEST(obj6.x == 1.);
  BOOST_TEST(obj6.y == 1.);
  BOOST_TEST(obj6.branchId == 11);
  BOOST_TEST(obj6.regionTag == 12);
  BOOST_TEST(obj6.meshSize == 13);
}



BOOST_AUTO_TEST_CASE(geom_point_methods, 
  *utf::tolerance(eps)
  *utf::description("methods")
)
{
  //norm and angle
  GeomPoint obj(1, 2);
  BOOST_TEST(obj.norm() == sqrt(5.));
  BOOST_TEST(obj.angle() == atan(2.));
  BOOST_TEST(obj.angle(obj) == 0.);
  GeomPoint obj_angl(1, -2);
  BOOST_TEST(obj.angle(obj_angl) == - 2*atan(2.));
  GeomPoint a(0, 0);
  BOOST_CHECK_THROW(a.angle(), std::invalid_argument);
  BOOST_CHECK_THROW(a.normalize(), std::invalid_argument);
  BOOST_CHECK_THROW(a.getNormalized(), std::invalid_argument);
  BOOST_CHECK_THROW(obj.angle(a), std::invalid_argument);

  //getNormalized
  GeomPoint obj2 = obj.getNormalized();
  BOOST_TEST((obj2.x == 1./sqrt(5.) && obj2.y == 2./sqrt(5.)));

  //normalize
  obj.normalize();
  BOOST_TEST(obj2 == obj);

  //rotate
  GeomPoint p(1, 0);
  
  p.rotate(0);
  BOOST_TEST_INFO("p.x = " << p.x << " p.y = " << p.y );
  BOOST_TEST(p.x == 1.);
  BOOST_TEST(p.y == 0.);
  
  p.rotate(M_PI/2.);
  BOOST_TEST(p.x == 0.);
  BOOST_TEST(p.y == 1.);
  
  p.rotate( -M_PI);
  BOOST_TEST(p.x == 0.);
  BOOST_TEST(p.y == -1.);
  
  p.rotate( -M_PI/2);
  BOOST_TEST(p.x == -1.);
  BOOST_TEST(p.y == 0.);
}



BOOST_AUTO_TEST_CASE(geom_point_operators, 
  *utf::tolerance(eps)
  *utf::description("operators")
)
{
  GeomPoint a(1, 1, 2, 3, 4);

  a += (a + a)/*2*a*/ +( a*(-2) + a*2)/*0*/ 
    + ( a /(-2) + a/2)/*0*/;
  a -= a/3;
  a /= 2;
  BOOST_TEST((a.x == 1. && a.y == 1.));
  BOOST_TEST((a[0] == 1. && a[1] == 1.));
  BOOST_TEST((a.branchId == 2 && a.regionTag == 3 && a.meshSize == 4.));
  BOOST_CHECK_THROW(a[2], std::invalid_argument);
}