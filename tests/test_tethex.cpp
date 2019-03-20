//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "TETHEX Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "tethex.hpp"

using namespace std;

const double eps = 1e-13;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructor_copy_and_assignment, 
    *utf::tolerance(eps))
{   
    tethex::Mesh msh;
    
    BOOST_TEST(msh.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh.get_n_vertices() == 0);
    BOOST_TEST(msh.get_n_points() == 0);
    BOOST_TEST(msh.get_n_lines() == 0);
    BOOST_TEST(msh.get_n_triangles() == 0);
    BOOST_TEST(msh.get_n_quadrangles() == 0);


    vector<tethex::Point> vertices{tethex::Point{}};
    vector<tethex::MeshElement *> 
        points{new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}}, 
        lines{new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}},
        triangles{new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}},
        quadrangles{new tethex::Quadrangle{1, 1, 1, 1, 1}, new tethex::Quadrangle{1, 1, 1, 1, 1}};

    msh.set_vertexes(vertices);
    msh.set_points(points);
    msh.set_lines(lines);
    msh.set_triangles(triangles);
    msh.set_quadrangles(quadrangles);

    BOOST_TEST(msh.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh.get_n_vertices() == 1);
    BOOST_TEST(msh.get_n_points() == 5);
    BOOST_TEST(msh.get_n_lines() == 4);
    BOOST_TEST(msh.get_n_triangles() == 3);
    BOOST_TEST(msh.get_n_quadrangles() == 2);

    //Copy constructor and assign constructor test
    auto msh_assign = msh;
    tethex::Mesh msh_copy(msh);
    msh.clean();

    BOOST_TEST(msh_copy.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh_copy.get_n_vertices() == 1);
    BOOST_TEST(msh_copy.get_n_points() == 5);
    BOOST_TEST(msh_copy.get_n_lines() == 4);
    BOOST_TEST(msh_copy.get_n_triangles() == 3);
    BOOST_TEST(msh_copy.get_n_quadrangles() == 2);
    BOOST_TEST(msh_copy.get_point(0).get_vertex(0) == 1);

    BOOST_TEST(msh_assign.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh_assign.get_n_vertices() == 1);
    BOOST_TEST(msh_assign.get_n_points() == 5);
    BOOST_TEST(msh_assign.get_n_lines() == 4);
    BOOST_TEST(msh_assign.get_n_triangles() == 3);
    BOOST_TEST(msh_assign.get_n_quadrangles() == 2);
    BOOST_TEST(msh_assign.get_point(0).get_vertex(0) == 1);
}


tethex::Mesh ReturnSomeMeshObj()
{
    tethex::Mesh msh;
    vector<tethex::Point> vertices{tethex::Point{}};
    vector<tethex::MeshElement *> 
        points{new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}}, 
        lines{new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}},
        triangles{new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}},
        quadrangles{new tethex::Quadrangle{1, 1, 1, 1, 1}, new tethex::Quadrangle{1, 1, 1, 1, 1}};

    msh.set_vertexes(vertices);
    msh.set_points(points);
    msh.set_lines(lines);
    msh.set_triangles(triangles);
    msh.set_quadrangles(quadrangles);

    return msh;
}

BOOST_AUTO_TEST_CASE( move_constructor, 
    *utf::tolerance(eps))
{
    auto msh_move = ReturnSomeMeshObj();
    BOOST_TEST(msh_move.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh_move.get_n_vertices() == 1);
    BOOST_TEST(msh_move.get_n_points() == 5);
    BOOST_TEST(msh_move.get_n_lines() == 4);
    BOOST_TEST(msh_move.get_n_triangles() == 3);
    BOOST_TEST(msh_move.get_n_quadrangles() == 2);
    BOOST_TEST(msh_move.get_point(0).get_vertex(0) == 1);
    
}