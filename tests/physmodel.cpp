//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Physical Model Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "physmodel.hpp"

using namespace River;

const double eps = 1e-12;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( phys_model_methods, 
    *utf::tolerance(eps))
{
    River::Model mdl;

    BOOST_TEST(mdl.integr.BaseVector(1, 1) == 1.);
    BOOST_TEST(mdl.integr.BaseVector(1, 2) == sqrt(2.));
    BOOST_TEST(mdl.integr.BaseVector(2, 1) == 0.);
    BOOST_TEST(mdl.integr.BaseVector(2, complex<double>(0., 1.)) == -1.);
    BOOST_TEST(mdl.integr.BaseVector(3, 1) == 1.);
    BOOST_TEST(mdl.integr.BaseVector(3, 2i) == -2.);
}


BOOST_AUTO_TEST_CASE( next_point_method, 
    *utf::tolerance(eps))
{
    Model mdl;
    auto p = mdl.next_point({0.0233587, 0.0117224, 0.621602}, 0);
    BOOST_TEST(p.r == mdl.ds);
    BOOST_TEST(p.phi == -0.1000335889387177);
    BOOST_TEST(atan(1) == M_PI/4);
    BOOST_TEST(atan(sqrt(3)) == M_PI/3);

}