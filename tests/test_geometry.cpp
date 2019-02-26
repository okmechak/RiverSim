//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Geometry Test Module"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_parameters.hpp>

#include <iostream>
#include <cmath>

#include "geometry.hpp"


namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
using namespace River;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_SUITE( geometry_test_suite )
//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( GeomPolarTest)
{

  auto tol = 1e-10;

  //default constructor test
  BOOST_TEST_MESSAGE("Deafault constructor");
  GeomPolar gp;
  BOOST_CHECK_CLOSE(gp.dl, 0., tol);
  BOOST_CHECK_CLOSE(gp.phi, 0., tol);
  BOOST_CHECK_EQUAL(gp.branchId, 0);
  BOOST_CHECK_EQUAL(gp.regionTag, 0);
  BOOST_CHECK_CLOSE(gp.meshSize, 1., tol);

  BOOST_TEST_MESSAGE("Constructor");
  gp = GeomPolar(2, 2, 2, 2, 2);
  BOOST_CHECK_CLOSE(gp.dl, 2., tol);
  BOOST_CHECK_CLOSE(gp.phi, 2., tol);
  BOOST_CHECK_EQUAL(gp.branchId, 2);
  BOOST_CHECK_EQUAL(gp.regionTag, 2);
  BOOST_CHECK_CLOSE(gp.meshSize, 2., tol);
}

BOOST_AUTO_TEST_CASE(GeomLineTest)
{
  BOOST_TEST_MESSAGE("Constructor");
  GeomLine gl(1, 1);
  BOOST_CHECK_EQUAL(gl.branchId, 0);
  BOOST_CHECK_EQUAL(gl.regionTag, 0);
  BOOST_CHECK_EQUAL(gl.p1, 1);
  BOOST_CHECK_EQUAL(gl.p2, 1);

}

BOOST_AUTO_TEST_CASE(GeomPointTest)
{
  auto tol = 1e-10;

  BOOST_TEST_MESSAGE("Default constructor");
  GeomPoint gp;

  BOOST_CHECK_EQUAL(gp.branchId, 0);
  BOOST_CHECK_EQUAL(gp.regionTag, 0);
  BOOST_CHECK_CLOSE(gp.meshSize, 1., tol);

  BOOST_TEST_MESSAGE("Constructor");
  gp = GeomPoint(1., 2., 3, 4, 5.);
  BOOST_CHECK_CLOSE(gp.x, 1., tol);
  BOOST_CHECK_CLOSE(gp.y, 2., tol);
  BOOST_CHECK_EQUAL(gp.branchId, 3);
  BOOST_CHECK_EQUAL(gp.regionTag, 4);
  BOOST_CHECK_CLOSE(gp.meshSize, 5., tol);

  BOOST_TEST_MESSAGE("Methods");
  BOOST_CHECK_CLOSE(gp.norm(), sqrt(5), tol);
  BOOST_CHECK_CLOSE(gp.angle(), M_PI/3, tol);
}

BOOST_AUTO_TEST_SUITE_END()