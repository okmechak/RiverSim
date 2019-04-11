//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Border Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "border.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructor_and_methods, 
    *utf::tolerance(eps))
{   
    //empty borderMesh
    Border border;

    BOOST_TEST(border.GetSourcesId().empty());
    BOOST_TEST(border.GetSourcesPoint().empty());
    BOOST_TEST(border.GetSourcesNormalAngle().empty());
    
    //now with some initialization
    border.MakeRectangular(
        {1, 1}, 
        {1, 2, 3, 4},
        {0.5},
        {7});

    BOOST_TEST(border.vertices.size() == border.lines.size());

    BOOST_TEST((border.vertices.at(0) == Point{1, 0}));
    BOOST_TEST((border.vertices.at(1) == Point{1, 1}));
    BOOST_TEST((border.vertices.at(2) == Point{0, 1}));
    BOOST_TEST((border.vertices.at(3) == Point{0, 0}));
    BOOST_TEST((border.vertices.at(4) == Point{0.5, 0}));

    BOOST_TEST(border.lines.at(0).p1 == 0);
    BOOST_TEST(border.lines.at(0).p2 == 1);
    BOOST_TEST(border.lines.at(1).p1 == 1);
    BOOST_TEST(border.lines.at(1).p2 == 2);
    BOOST_TEST(border.lines.at(2).p1 == 2);
    BOOST_TEST(border.lines.at(2).p2 == 3);
    BOOST_TEST(border.lines.at(3).p1 == 3);
    BOOST_TEST(border.lines.at(3).p2 == 4);
    BOOST_TEST(border.lines.at(4).p1 == 4);
    BOOST_TEST(border.lines.at(4).p2 == 0);

    BOOST_TEST((border.GetSourcesId().at(0) == 7));
    BOOST_TEST(border.vertices.at(4).x == 0.5);
    BOOST_TEST(border.vertices.at(4).y == 0.);
    BOOST_TEST(border.GetSourceVerticeIndex(7) == 4);

    BOOST_TEST(border.GetSourcesNormalAngle().at(0) = M_PI/2);
    
    vector<double> coords = {0.5, 0.6, 0.7};
    vector<int> ids = {1, 2, 3};
    border.MakeRectangular(
        {1, 4}, 
        {0, 1, 2, 3},
        coords, ids);

    BOOST_TEST(border.vertices.size() == border.lines.size());
    BOOST_TEST(border.vertices.size() == 7);

    BOOST_TEST(border.GetSourcesId() == ids);
    BOOST_TEST(border.GetSourcesPoint().back() == Point(0.7, 0.));
    BOOST_TEST(border.GetSourceVerticeIndex(1) == 4);
    BOOST_TEST(border.GetSourceVerticeIndex(2) == 5);
    BOOST_TEST(border.GetSourceVerticeIndex(3) == 6);
    
    for(auto id: ids)
        BOOST_TEST(border.GetSourceNormalAngle(id) == M_PI/2.);
}

BOOST_AUTO_TEST_CASE( get_sources_point, 
    *utf::tolerance(eps))
{
    Border border;
    vector<Point> p;
    BOOST_TEST(border.GetSourcesPoint() == p);
    
    vector<int> ids{1,2,3,4,5};
    border.MakeRectangular({2, 2}, {0, 1, 2, 3}, {0.1, 0.2, 0.3, 0.4, 0.5}, ids);
    p = vector<Point>{
        {0.1, 0},
        {0.2, 0},
        {0.3, 0},
        {0.4, 0},
        {0.5, 0}};
    
    BOOST_TEST(border.GetSourcesPoint() == p);
    BOOST_TEST(border.GetSourcesId() == ids);
}