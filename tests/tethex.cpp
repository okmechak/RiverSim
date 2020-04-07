//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "TETHEX Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "tethex.hpp"
#include <cmath> 

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
    BOOST_TEST(msh.get_n_holes() == 0);
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
    msh.set_holes(vertices);
    msh.set_lines(lines);
    msh.set_triangles(triangles);
    msh.set_quadrangles(quadrangles);

    BOOST_TEST(msh.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh.get_n_vertices() == 1);
    BOOST_TEST(msh.get_n_holes() == 1);
    BOOST_TEST(msh.get_n_points() == 5);
    BOOST_TEST(msh.get_n_lines() == 4);
    BOOST_TEST(msh.get_n_triangles() == 3);
    BOOST_TEST(msh.get_n_quadrangles() == 2);

    //Copy constructor and assign constructor test
    tethex::Mesh msh_copy(msh);
    auto msh_assign = msh;
    msh.clean();

    BOOST_TEST(msh_copy.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh_copy.get_n_vertices() == 1);
    BOOST_TEST(msh_copy.get_n_holes() == 1);
    BOOST_TEST(msh_copy.get_n_points() == 5);
    BOOST_TEST(msh_copy.get_n_lines() == 4);
    BOOST_TEST(msh_copy.get_n_triangles() == 3);
    BOOST_TEST(msh_copy.get_n_quadrangles() == 2);
    BOOST_TEST(msh_copy.get_point(0).get_vertex(0) == 1);

    BOOST_TEST(msh_assign.get_n_converted_quadrangles() == 0);
    BOOST_TEST(msh_assign.get_n_vertices() == 1);
    BOOST_TEST(msh_assign.get_n_holes() == 1);
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
    vector<tethex::Point> holes{tethex::Point{}};
    vector<tethex::MeshElement *> 
        points{new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}, new tethex::PhysPoint{1, 1}}, 
        lines{new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}, new tethex::Line{1, 1, 1}},
        triangles{new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}, new tethex::Triangle{1, 1, 1, 1}},
        quadrangles{new tethex::Quadrangle{1, 1, 1, 1, 1}, new tethex::Quadrangle{1, 1, 1, 1, 1}};

    msh.set_vertexes(vertices);
    msh.set_holes(holes);
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
    BOOST_TEST(msh_move.get_n_holes() == 1);
    BOOST_TEST(msh_move.get_n_points() == 5);
    BOOST_TEST(msh_move.get_n_lines() == 4);
    BOOST_TEST(msh_move.get_n_triangles() == 3);
    BOOST_TEST(msh_move.get_n_quadrangles() == 2);
    BOOST_TEST(msh_move.get_point(0).get_vertex(0) == 1);
}

BOOST_AUTO_TEST_CASE( measure_of_mesh_elements, 
    *utf::tolerance(eps))
{
    //PhysPoint
    vector<tethex::Point> point_vertices{tethex::Point{0, 0}};
    tethex::PhysPoint p(0, 0);
    BOOST_CHECK_THROW(p.min_angle(point_vertices), River::Exception);
    BOOST_CHECK_THROW(p.max_angle(point_vertices), River::Exception);
    BOOST_CHECK_THROW(p.quality(point_vertices), River::Exception);
    BOOST_TEST(p.measure(point_vertices) == 0);
    BOOST_TEST(p.min_edge(point_vertices) == 0);
    BOOST_TEST(p.max_edge(point_vertices) == 0);

    //Line
    vector<tethex::Point> line_vertices{tethex::Point{1, 1}, tethex::Point{2, 0}};
    tethex::Line l(0, 1, 0);
    BOOST_CHECK_THROW(l.min_angle(line_vertices), River::Exception);
    BOOST_CHECK_THROW(l.max_angle(line_vertices), River::Exception);
    BOOST_CHECK_THROW(l.quality(line_vertices), River::Exception);
    BOOST_TEST(l.measure(line_vertices) == sqrt(2.));
    BOOST_TEST(l.min_edge(line_vertices) == sqrt(2.));
    BOOST_TEST(l.max_edge(line_vertices) == sqrt(2.));

    //Triangle 1
    vector<tethex::Point> triangle_vertices{tethex::Point{0, 0}, tethex::Point{1, 0}, tethex::Point{0, 1}};
    tethex::Triangle t1(0, 1, 2, 0);
    BOOST_TEST(t1.min_angle(triangle_vertices) == 45.);
    BOOST_TEST(t1.max_angle(triangle_vertices) == 90.);
    BOOST_TEST(t1.measure(triangle_vertices) == 0.5);
    BOOST_TEST(t1.min_edge(triangle_vertices) == 1);
    BOOST_TEST(t1.max_edge(triangle_vertices) == sqrt(2.));
    BOOST_TEST(t1.quality(triangle_vertices) = sqrt(2.));

    //Triangle 2
    triangle_vertices = vector<tethex::Point>{tethex::Point{0, 0}, tethex::Point{0, 1}, tethex::Point{1, 0}};
    tethex::Triangle t2(0, 1, 2, 0);
    BOOST_TEST(t2.min_angle(triangle_vertices) == 45.);
    BOOST_TEST(t2.max_angle(triangle_vertices) == 90.);
    BOOST_TEST(t2.measure(triangle_vertices) == 0.5);
    BOOST_TEST(t2.min_edge(triangle_vertices) == 1);
    BOOST_TEST(t2.max_edge(triangle_vertices) == sqrt(2.));
    BOOST_TEST(t2.quality(triangle_vertices) = sqrt(2.));

    //quadrangle
    vector<tethex::Point> quadrangle_vertices{tethex::Point{0, 0}, tethex::Point{1, 0}, tethex::Point{1, 1}, tethex::Point{1, 0}};
    tethex::Quadrangle q(0, 1, 2, 3, 10);
    BOOST_CHECK_THROW(q.min_angle(quadrangle_vertices), River::Exception);
    BOOST_CHECK_THROW(q.max_angle(quadrangle_vertices), River::Exception);
    BOOST_CHECK_THROW(q.measure(quadrangle_vertices), River::Exception);
    BOOST_CHECK_THROW(q.min_edge(quadrangle_vertices), River::Exception);
    BOOST_CHECK_THROW(q.max_edge(quadrangle_vertices), River::Exception);
    BOOST_CHECK_THROW(q.quality(quadrangle_vertices), River::Exception);
}