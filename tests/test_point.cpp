//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BaseClassModule"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <math.h>
#include "common.hpp"


namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
using namespace River;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructors)
{

  Point obj;
  Point obj1();
  Point obj2{};
  Point obj3{1, 1};
  
  auto tol = 1e-10;

  BOOST_CHECK_CLOSE(obj3.x, 1., tol);
  BOOST_CHECK_CLOSE(obj3.y, 1., tol);
  
}

//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( methods, *utf::tolerance(1e-4))
{
    utf::unit_test_log_t::instance().set_threshold_level(utf::log_level::log_test_units);
    Point p1{1, 1};

    auto tol = 1e-10;
    BOOST_CHECK_CLOSE(p1.norm(), sqrt(2), tol);
    BOOST_CHECK_CLOSE(p1.angle(), M_PI/4, tol);
    //p1 = Point{-1, 1};
    //BOOST_CHECK(p1.angle() == M_PI*3/4);
    //p1 = Point{-1, -1};
    //BOOST_CHECK(p1.angle() == - M_PI*3/4);
    //p1 = Point{1, -1};
    //BOOST_CHECK(p1.angle() == - M_PI*4);

}