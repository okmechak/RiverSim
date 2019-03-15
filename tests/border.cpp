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
    tethex::Mesh borderMesh;
    Border border(borderMesh);

    BOOST_TEST(border.GetHolesId().empty());
    BOOST_TEST(border.GetSourcesId().empty());

    //now with some initialization
    border.MakeRectangular(
        {1, 4}, 
        {0, 1, 2, 3},
        {0.5},
        {1});

    BOOST_TEST((!border.GetSourcesId().empty())); 
    BOOST_TEST((border.GetSourcesId().at(0) == 1));
    //each border point should be inside of only 2 lines
    auto source_point_id = border.GetSourceById(1).get_vertex(0);
    BOOST_TEST(border.GetPointLines(source_point_id).size() == 2);
    //and 2 adjacent points 
    BOOST_TEST(border.GetAdjacentPointsId(source_point_id).size() == 2);
    BOOST_TEST(border.GetAdjacentPointsId(source_point_id).at(0) == source_point_id - 1);
    BOOST_TEST(border.GetAdjacentPointsId(source_point_id).at(1) == 0);


    borderMesh.clean();
    vector<double> coords = {0.5, 0.6, 0.7, 0.8, 0.9};
    vector<int> ids = {1, 2, 3, 4, 5};
    border.MakeRectangular(
        {1, 4}, 
        {0, 1, 2, 3},
        coords, ids);
    
    for(unsigned i = 0; i < ids.size(); ++i)
    {
        BOOST_TEST(ids[i] == border.GetSourcesId().at(i));
        BOOST_TEST( borderMesh.get_vertex(border.GetSourceById(ids[i]).get_vertex(0)).get_coord(0) == coords[i]);
    }

    BOOST_TEST(border.GetHolesId().empty());
    //FIXME: we should also read from file and compare to it
    //this case is very common

    for(auto id: ids)
    {
        auto angle = border.GetSourceNormalAngle(id);
        BOOST_TEST(angle == M_PI/2.);
    }
}

BOOST_AUTO_TEST_CASE( get_sources_point, 
    *utf::tolerance(eps))
{
    tet::Mesh border_mesh;
    Border border(border_mesh);
    vector<Point> p;
    BOOST_TEST(border.GetSourcesPoint() == p);BOOST_TEST(border.GetSourcesPoint() == p);
    vector<int> ids{1,2,3,4,5};
    border.MakeRectangular({2, 2}, {0, 1, 2, 3}, {0.1, 0.2, 0.3, 0.4, 0.5}, ids);
    p = vector<Point>{{0.1, 0},{0.2, 0},{0.3, 0},{0.4, 0},{0.5, 0}};
    BOOST_TEST(border.GetSourcesPoint() == p);
    BOOST_TEST(border.GetSourcesId() == ids);

}