//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Boundary Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include "boundary.hpp"

using namespace River;

const double eps = EPS;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Boundary_Condition, 
    *utf::tolerance(eps)
    *utf::description("line class tests"))
{   
    BoundaryCondition bd;
    BOOST_TEST(bd.type == DIRICHLET);
    BOOST_TEST(bd.value == 0.);
}

BOOST_AUTO_TEST_CASE( Line_test, 
    *utf::tolerance(eps)
    *utf::description("line class tests"))
{   
    auto 
        l1 = Line{1, 2, 3}, 
        l2 = Line{1, 2, 3};
    
    BOOST_TEST(l1.p1 == 1);
    BOOST_TEST(l1.p2 == 2);
    BOOST_TEST(l1.boundary_id == 3);
    BOOST_TEST(l1 == l2);
}

BOOST_AUTO_TEST_CASE( SimpleBoundary_ReplaceElement, 
    *utf::tolerance(eps))
{
    SimpleBoundary b_base;
    b_base.vertices = {{0, 0}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    b_base.lines = {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 4, 4}, {4, 0, 5}};

    SimpleBoundary b_insert;
    b_insert.vertices = {{0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    b_insert.lines = {{0, 1, 0}, {1, 2, 0}, {2, 3, 0}};

    b_base.ReplaceElement(1, b_insert);
    
    auto expected_vertices = vector<Point>{{0, 0}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    auto expected_lines = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};

    BOOST_TEST(b_base.vertices == expected_vertices);
    BOOST_TEST(b_base.lines == expected_lines);

    //now lets insert empty boundary
    SimpleBoundary b_insert_1;
    b_base.ReplaceElement(1, b_insert_1);
    b_base.ReplaceElement(3, b_insert_1);
    BOOST_TEST(b_base.vertices == expected_vertices);
    BOOST_TEST(b_base.lines == expected_lines);

    //insert boundary with only one element
    SimpleBoundary b_insert_2;
    b_insert_2.vertices = {{10, 10}};
    b_base.ReplaceElement(5, b_insert_2);
    auto expected_vertices_2 = vector<Point>{{0, 0}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {10, 10}, {1, 1}, {0, 1}};
    auto expected_lines_2 = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};
    BOOST_TEST(b_base.vertices == expected_vertices_2);
    BOOST_TEST(b_base.lines == expected_lines_2);

    //insertion at the end
    b_base.ReplaceElement(7, b_insert_2);
    auto expected_vertices_3 = vector<Point>{{0, 0}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {10, 10}, {1, 1}, {10, 10}};
    auto expected_lines_3 = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};
    BOOST_TEST(b_base.vertices == expected_vertices_3);
    BOOST_TEST(b_base.lines == expected_lines_3);
    
    //insertion at beginning
    b_base.ReplaceElement(0, b_insert_2);
    auto expected_vertices_4 = vector<Point>{{10, 10}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {10, 10}, {1, 1}, {10, 10}};
    auto expected_lines_4 = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};
    BOOST_TEST(b_base.vertices == expected_vertices_4);
    BOOST_TEST(b_base.lines == expected_lines_4);

    //insertion at middle of a bigger sequence
    b_base.ReplaceElement(4, b_insert);
    auto expected_vertices_5 = vector<Point>{{10, 10}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {10, 10}, {1, 1}, {10, 10}};
    auto expected_lines_5 = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 0}, {5, 6, 0}, {6, 7, 0}, {7, 8, 2}, {8, 9, 3}, {9, 10, 4}, {10, 0, 5}};
    BOOST_TEST(b_base.vertices == expected_vertices_5);
    BOOST_TEST(b_base.lines == expected_lines_5);
}

BOOST_AUTO_TEST_CASE( SimpleBoundary_Append, 
    *utf::tolerance(eps))
{
    SimpleBoundary b_base, b_insert_1, b_insert_2;
    b_base.Append(b_insert_1);
    BOOST_TEST(b_base.vertices.empty());
    BOOST_TEST(b_base.lines.empty());    

    b_insert_1.vertices = {{0, 0}, {0, 1}};
    b_insert_1.lines = {{0, 1, 1}};

    b_base.Append(b_insert_1);
    
    auto expected_vertices_1 = vector<Point>{{0, 0}, {0, 1}};
    for(size_t i = 0; i < expected_vertices_1.size(); ++i)
        BOOST_TEST(b_base.vertices.at(i) == expected_vertices_1.at(i));
    
    auto expected_lines_1 = vector<Line>{{0, 1, 1}};
    for(size_t i = 0; i < expected_lines_1.size(); ++i)
        BOOST_TEST(b_base.lines.at(i) == expected_lines_1.at(i));

    b_insert_2.vertices = {{0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    b_insert_2.lines = {{0, 1, 0}, {1, 2, 0}, {2, 3, 0}};

    b_base.Append(b_insert_2);
    
    auto expected_vertices_2 = vector<Point>{{0, 0}, {0, 1}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    for(size_t i = 0; i < expected_vertices_2.size(); ++i)
        BOOST_TEST(b_base.vertices.at(i) == expected_vertices_2.at(i));
    
    auto expected_lines_2 = vector<Line>{{0, 1, 1}, {2, 3, 0}, {3, 4, 0}, {4, 5, 0}};
    for(size_t i = 0; i < expected_lines_2.size(); ++i)
        BOOST_TEST(b_base.lines.at(i) == expected_lines_2.at(i));
}

BOOST_AUTO_TEST_CASE( Boundaries_NormalAngle, 
    *utf::tolerance(eps))
{   
    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {0, 1e-10}) == M_PI);
    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);

    //_ _
    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {2, 0}) == M_PI/2);
    // _|
    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {1, 1}) == M_PI/4*3);
    // -|
    BOOST_TEST(Boundaries::NormalAngle({1, 0}, {1, 1}, {0, 1}) == M_PI*5/4);
    // |-
    BOOST_TEST(Boundaries::NormalAngle({1, 1}, {0, 1}, {0, 0}) == 7*M_PI/4);
    //|_
    BOOST_TEST(Boundaries::NormalAngle({0, 1}, {0, 0}, {1, 0}) == M_PI/4);

    BOOST_TEST(Boundaries::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
}

BOOST_AUTO_TEST_CASE( Boundaries_some_methods, 
    *utf::tolerance(eps))
{   
    BOOST_TEST_CHECKPOINT("empty test");
    Boundaries boundary;

    //checks if boundary is empty
    BOOST_TEST_CHECKPOINT("Check");
    BOOST_CHECK_THROW(boundary.Check(), Exception);
    BOOST_TEST_CHECKPOINT("GetOuterBoundary");
    BOOST_CHECK_THROW(boundary.GetOuterBoundary(), Exception);
    BOOST_TEST_CHECKPOINT("GetHolesList");
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});
    BOOST_TEST_CHECKPOINT("GetSourcesIdsPointsAndAngles");
    Sources sources;
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles(sources) == Boundaries::trees_interface_t{});
    
    BOOST_TEST_CHECKPOINT("With one boundary test");
    const auto source_x_position = 0.25, width = 1., height = 1.;
    auto outer_simple_boundary = SimpleBoundary
    {
        {/*vertices(counterclockwise order)*/
            {0, 0},
            {source_x_position, 0}, 
            {width, 0}, 
            {width, height}, 
            {0, height}
        }, 
        {/*lines*/
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, 4},
            {4, 0, 5} 
        }, 
        false/*this is not inner boudary*/,
        {}/*holes*/,
        "outer rectangular boudary"
    };
    boundary[1] = outer_simple_boundary;
    sources[1] = {1, 1};

    //non empty boudary checks
    BOOST_CHECK_NO_THROW(boundary.Check());
    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});
    auto s = Boundaries::trees_interface_t{{1,{{source_x_position, 0}, M_PI/2}}};
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles(sources) == s);

    //add inner boudnary
    BOOST_TEST_CHECKPOINT("With two boundaries test");
    auto inner_simple_boundary = SimpleBoundary
    {
        {/*vertices(counterclockwise order)*/
            {0.25*width, 0.25*height},
            {0.75*width, 0.25*height}, 
            {0.75*width, 0.75*height}, 
            {0.25*width, 0.75*height}
        }, 
        {/*lines*/
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 0, 4}
        }, 
        false/*this is not inner boudary*/,
        {{0.5*width, 0.5*height}}/*holes*/,
        "outer rectangular boudary"
    };
    boundary[2] = inner_simple_boundary;
    sources[2] = {2, 2};
    BOOST_CHECK_THROW(boundary.Check(), Exception);

    boundary.at(2).inner_boundary = true;
    
    //checks with inner boundaries
    BOOST_CHECK_NO_THROW(boundary.Check());
    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
    auto holes_list = vector<Point>{{0.5*width, 0.5*height}};
    BOOST_TEST(boundary.GetHolesList() == holes_list);
    s = Boundaries::trees_interface_t{
        {1, {{source_x_position, 0}, M_PI/2}},
        {2, {{0.75*width, 0.75*height}, M_PI/4}}
    };
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles(sources) == s);

    sources[1] = {1, 0};
    sources[2] = {1, 1};
    sources[3] = {1, 2};
    sources[4] = {1, 3};
    sources[5] = {1, 4};
    sources[6] = {2, 0};
    sources[7] = {2, 1};
    sources[8] = {2, 2};
    sources[9] = {2, 3};

    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
   
    BOOST_TEST(boundary.GetHolesList() == holes_list);
    s = Boundaries::trees_interface_t{
        {1, {{0, 0}, M_PI/4}},
        {2, {{source_x_position, 0}, M_PI/2}},
        {3, {{width, 0}, 3*M_PI/4}},
        {4, {{width, height}, 5*M_PI/4}},
        {5, {{0, height}, 7*M_PI/4}},
        {6, {{0.25*width, 0.25*height}, 5*M_PI/4}},
        {7, {{0.75*width, 0.25*height}, 7*M_PI/4}},
        {8, {{0.75*width, 0.75*height}, M_PI/4}},
        {9, {{0.25*width, 0.75*height}, 3*M_PI/4}}
    };
    auto s_2 = boundary.GetSourcesIdsPointsAndAngles(sources);
    for(auto& [id, p]: s)
    {
        BOOST_TEST_MESSAGE("source id: " + to_string(id));
        BOOST_TEST(s_2.count(id));
        BOOST_TEST(s_2.at(id).first == p.first);
        BOOST_TEST(s_2.at(id).second == p.second);
    }
}

BOOST_AUTO_TEST_CASE( MakeRectangular, 
    *utf::tolerance(eps))
{   
    Boundaries boundary;
    BOOST_CHECK_THROW(boundary.Check(), Exception);
    auto sources = boundary.MakeRectangular();

    auto& outer_boundary = boundary.GetOuterBoundary();
    BOOST_TEST(outer_boundary.inner_boundary == false );
    
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});

    auto sources_ids_points_and_angles_map = boundary.GetSourcesIdsPointsAndAngles(sources);
    BOOST_TEST(sources_ids_points_and_angles_map.count(1));
    BOOST_TEST((sources_ids_points_and_angles_map.at(1).first == Point{0.25, 0}));
    BOOST_TEST(sources_ids_points_and_angles_map.at(1).second == M_PI/2);
}