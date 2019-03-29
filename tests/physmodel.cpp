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

    BOOST_TEST(mdl.BaseVector(1, 1) == 1.);
    BOOST_TEST(mdl.BaseVector(1, 2) == sqrt(2.));
    BOOST_TEST(mdl.BaseVector(2, 1) == 0.);
    BOOST_TEST(mdl.BaseVector(2, 1i) == -1.);
    BOOST_TEST(mdl.BaseVector(3, 1) == 1.);
    BOOST_TEST(mdl.BaseVector(3, 2i) == -2.);


    //const values valid only for next Rmax
    BOOST_TEST(mdl.Rmax == 0.01);
    //Values from similar Mathematica formula
    BOOST_TEST(mdl.WeightFunction( Point{0.25, 0.1}.norm()) == 1.369306341561314E-315);
    BOOST_TEST(mdl.WeightFunction( Point{0.01, 0.01}.norm()) == 0.1353352832366125);
}

BOOST_AUTO_TEST_CASE( area_constraint_test, 
    *utf::tolerance(eps))
{
    River::AreaConstraint ac;
    ac.exponant = 2;
    ac.min_area = 0.0001;
    ac.r0 = 0.05;
    ac.tip_points = {{0, 0}, {0, 0}, {0,0}};
    BOOST_TEST(ac(0, 0) == ac.min_area);
    
    ac.tip_points = {{0, 0}, {0.1, 0.1}, {0.1,0}};
    BOOST_TEST(ac(0.05, 0.05) == 0.8647647167633872);
}