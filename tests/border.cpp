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
    tethex::Mesh border_mesh;
    Border border(border_mesh);

    BOOST_TEST(border.GetHolesId().empty());
    BOOST_TEST(border.GetSourcesId().empty());
    
    //now with some initialization
    border.MakeRectangular(
        {1, 4}, 
        {0, 1, 2, 3},
        {0.5},
        {1});
    
    BOOST_TEST(!border.GetSourcesId().empty()); 
    BOOST_TEST(border.GetSourcesId().size() == 1);
    BOOST_TEST((border.GetSourcesId().at(0) == 1));
    BOOST_TEST(border_mesh.get_point(0).get_vertex(0) == 4);
    BOOST_TEST(border_mesh.get_point(1).get_vertex(0) == 5);
    BOOST_TEST(border_mesh.get_vertex(4).get_coord(0) == 0.5 - border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(5).get_coord(0) == 0.5 + border.eps/2);

    BOOST_TEST(border.GetSourcesNormalAngle().at(0) = M_PI/2);
    BOOST_TEST(true);
    


    border_mesh.clean();
    vector<double> coords = {0.5, 0.6, 0.7};
    vector<int> ids = {1, 2, 3};
    border.MakeRectangular(
        {1, 4}, 
        {0, 1, 2, 3},
        coords, ids);
    
    BOOST_TEST(border.GetHolesId().empty());
    //FIXME: we should also read from file and compare to it
    //this case is very common

    //TEST of MAkeRectangular
    BOOST_TEST(border_mesh.get_point(0).get_vertex(0) == 4);
    BOOST_TEST(border_mesh.get_point(1).get_vertex(0) == 5);
    BOOST_TEST(border_mesh.get_point(2).get_vertex(0) == 6);
    BOOST_TEST(border_mesh.get_point(3).get_vertex(0) == 7);
    BOOST_TEST(border_mesh.get_point(4).get_vertex(0) == 8);
    BOOST_TEST(border_mesh.get_point(5).get_vertex(0) == 9);

    auto[p1_i, p2_i] = border.GetSourceVerticesIndexById(1);
    BOOST_TEST(p1_i == 4);
    BOOST_TEST(p2_i == 5);
    BOOST_TEST(border_mesh.get_vertex(4).get_coord(0) == 0.5 - border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(5).get_coord(0) == 0.5 + border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(6).get_coord(0) == 0.6 - border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(7).get_coord(0) == 0.6 + border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(8).get_coord(0) == 0.7 - border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(9).get_coord(0) == 0.7 + border.eps/2);
    BOOST_TEST(border_mesh.get_vertex(4).get_coord(1) == 0);
    BOOST_TEST(border_mesh.get_vertex(5).get_coord(1) == 0);
    BOOST_TEST(border_mesh.get_vertex(6).get_coord(1) == 0);
    BOOST_TEST(border_mesh.get_vertex(7).get_coord(1) == 0);
    BOOST_TEST(border_mesh.get_vertex(8).get_coord(1) == 0);
    BOOST_TEST(border_mesh.get_vertex(9).get_coord(1) == 0);

    BOOST_TEST(border_mesh.get_n_points() == 6);

    auto adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(4));
    BOOST_TEST(adjacent_left.get_coord(0) == 0);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);
    adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(5));
    BOOST_TEST(adjacent_left.get_coord(0) == 0.6 - border.eps/2);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);
    adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(6));
    BOOST_TEST(adjacent_left.get_coord(0) == 0.5 + border.eps/2);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);
    adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(7));
    BOOST_TEST(adjacent_left.get_coord(0) == 0.7 - border.eps/2);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);
    adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(8));
    BOOST_TEST(adjacent_left.get_coord(0) == 0.6 + border.eps/2);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);
    adjacent_left = border_mesh.get_vertex(border.GetAdjacentPointId(9));
    BOOST_TEST(adjacent_left.get_coord(0) == 1);
    BOOST_TEST(adjacent_left.get_coord(1) == 0);

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

BOOST_AUTO_TEST_CASE( close_sources_method, 
    *utf::tolerance(eps))
{
    tet::Mesh border_mesh;
    Border border(border_mesh);
    border.MakeRectangular(
        {2, 2}, 
        {0, 1, 2, 3/*line id*/}, 
        {0.1, 0.2, 0.3, 0.4, 0.5}, 
        {1,2,3,4,5});
    auto num_of_lines = border_mesh.get_n_lines();
    border.CloseSources(3/*line id*/);
    BOOST_TEST(border_mesh.get_n_lines() - num_of_lines == 5 );


}