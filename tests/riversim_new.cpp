//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "riversim.hpp"
   
using namespace River;

const double eps = 1e-10;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( first_step, 
    *utf::tolerance(eps)
    *utf::description("when model is empty"))
{
    Model model;
    Triangle triangle(&model.mesh);
    BOOST_CHECK_THROW(TriangulateBoundaries(model, triangle, "test"), Exception);
}