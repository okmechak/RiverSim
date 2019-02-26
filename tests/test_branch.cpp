//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BaseClassModule"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "geometry.hpp"
// ------------- Tests Follow --------------
//Name your test cases for what they test
BOOST_AUTO_TEST_CASE( constructors )
{
  River::Branch obj(1, River::GeomPoint{0., 0}, 0);
  BOOST_CHECK(!obj.empty());
  
}

BOOST_AUTO_TEST_CASE( assignment )
{

}