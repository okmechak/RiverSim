//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BaseClassModule"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "geometry.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructor, *utf::tolerance(eps))
{
    Branch b(1, GeomPoint{1., 2., 3, 4, 5.}, 1., 1e-10);

    BOOST_TEST(b.id == 1);
    BOOST_TEST(b.getWidth() == 1e-10);
    BOOST_TEST(b.getTailAngle() == 1.);

    auto p = b.getTail();
    BOOST_TEST(p.x == 1.); 
    BOOST_TEST(p.y == 2.);
    BOOST_TEST(p.branchId == 1);//same as in branch
    BOOST_TEST(p.regionTag == 4);
    BOOST_TEST(p.meshSize == 5.);

    


  
}



BOOST_AUTO_TEST_CASE( assignment )
{

}