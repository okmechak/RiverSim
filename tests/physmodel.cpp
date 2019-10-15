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
    BOOST_TEST(mdl.integr.BaseVector(3, complex<double>(0., 2.)) == -2.);
}


BOOST_AUTO_TEST_CASE( next_point_method, 
    *utf::tolerance(eps))
{
    Model mdl;
    mdl.ds = 0.003;
    auto p = mdl.next_point({0.0233587, 0.0117224, 0.621602}, 0, 1/*maximal a series param*/);
    BOOST_TEST(p.r == pow(mdl.ds, mdl.eta));
    BOOST_TEST(p.phi ==-0.054918865933649114);

}