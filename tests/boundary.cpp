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

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Line_test, 
    *utf::tolerance(eps))
{   
    auto l1 = Line{1, 2, 3}, 
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

BOOST_AUTO_TEST_CASE( Boundary_NormalAngle, 
    *utf::tolerance(eps))
{   
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {0, 1e-10}) == M_PI);
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);

    //_ _
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {2, 0}) == M_PI/2);
    // _|
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {1, 1}) == M_PI/4*3);
    // -|
    BOOST_TEST(Boundary::NormalAngle({1, 0}, {1, 1}, {0, 1}) == M_PI*5/4);
    // |-
    BOOST_TEST(Boundary::NormalAngle({1, 1}, {0, 1}, {0, 0}) == 7*M_PI/4);
    //|_
    BOOST_TEST(Boundary::NormalAngle({0, 1}, {0, 0}, {1, 0}) == M_PI/4);

    BOOST_TEST(Point::angle(1, 0) == 0);
    BOOST_TEST(Point::angle(0, -1) == 6*M_PI/4);
    BOOST_TEST(Point::angle(-1, -1) == 5*M_PI/4);
    BOOST_TEST(Boundary::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
}

BOOST_AUTO_TEST_CASE( Boundary_some_methods, 
    *utf::tolerance(eps))
{   
    const auto source_x_position = 0.25, width = 1., height = 1.;
    const SimpleBoundary::source_id_t source_id = 1;
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
        {{source_id, 1}}/*sources*/, 
        false/*this is not inner boudary*/,
        {}/*hole*/,
        "outer rectangular boudary"
    };

    Boundary boundary;

    //checks if boundary is empty
    BOOST_CHECK_THROW(boundary.Check(), Exception);
    BOOST_CHECK_THROW(boundary.GetOuterBoundary(), Exception);
    BOOST_TEST(boundary.GetSourcesIds() == vector<SimpleBoundary::source_id_t>{});
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles() == Boundary::trees_interface_t{});
    
    boundary.simple_boundaries = {outer_simple_boundary};

    //non empty boudary checks
    BOOST_CHECK_NO_THROW(boundary.Check());
    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
    BOOST_TEST(boundary.GetSourcesIds() == vector<SimpleBoundary::source_id_t>{source_id});
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});
    auto s = Boundary::trees_interface_t{{source_id,{{source_x_position, 0}, M_PI/2}}};
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles() == s);

    //add inner boudnary
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
        {{source_id + 1, 2}}/*sources*/, 
        false/*this is not inner boudary*/,
        {0.5*width, 0.5*height}/*hole*/,
        "outer rectangular boudary"
    };
    boundary.simple_boundaries.push_back(inner_simple_boundary);
    BOOST_CHECK_THROW(boundary.Check(), Exception);

    boundary.simple_boundaries.at(1).inner_boundary = true;
    
    //checks with inner boundaries
    BOOST_CHECK_NO_THROW(boundary.Check());
    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
    auto sources_ids = vector<SimpleBoundary::source_id_t>{source_id, source_id + 1};
    BOOST_TEST(boundary.GetSourcesIds() == sources_ids);
    auto holes_list = vector<Point>{{0.5*width, 0.5*height}};
    BOOST_TEST(boundary.GetHolesList() == holes_list);
    s = Boundary::trees_interface_t{
        {source_id, {{source_x_position, 0}, M_PI/2}},
        {source_id + 1, {{0.75*width, 0.75*height}, M_PI/4}}
    };
    BOOST_TEST(boundary.GetSourcesIdsPointsAndAngles() == s);

    //modify inner boundary with adding a lot of sources points
    inner_simple_boundary = SimpleBoundary
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
        {{source_id + 5, 0}, {source_id + 6, 1}, {source_id + 7, 2}, {source_id + 8, 3}}/*sources*/, 
        true/*this is not inner boudary*/,
        {0.5*width, 0.5*height}/*hole*/,
        "outer rectangular boudary"
    };
    boundary.simple_boundaries.at(1) = inner_simple_boundary;
    boundary.simple_boundaries.at(0).sources = {
        {source_id, 0},
        {source_id + 1, 1},
        {source_id + 2, 2},
        {source_id + 3, 3},
        {source_id + 4, 4}};
    outer_simple_boundary.sources = boundary.simple_boundaries.at(0).sources;

    BOOST_CHECK_NO_THROW(boundary.Check());
    BOOST_TEST(boundary.GetOuterBoundary() == outer_simple_boundary);
    sources_ids = vector<SimpleBoundary::source_id_t>{
        source_id, source_id + 1, source_id + 2, source_id + 3, source_id + 4,
        source_id + 5, source_id + 6, source_id + 7, source_id + 8};

    BOOST_TEST(boundary.GetSourcesIds() == sources_ids);
    holes_list = vector<Point>{{0.5*width, 0.5*height}};
    BOOST_TEST(boundary.GetHolesList() == holes_list);
    s = Boundary::trees_interface_t{
        {source_id, {{0, 0}, M_PI/4}},
        {source_id + 1, {{source_x_position, 0}, M_PI/2}},
        {source_id + 2, {{width, 0}, 3*M_PI/4}},
        {source_id + 3, {{width, height}, 5*M_PI/4}},
        {source_id + 4, {{0, height}, 7*M_PI/4}},
        {source_id + 5, {{0.25*width, 0.25*height}, 5*M_PI/4}},
        {source_id + 6, {{0.75*width, 0.25*height}, 7*M_PI/4}},
        {source_id + 7, {{0.75*width, 0.75*height}, M_PI/4}},
        {source_id + 8, {{0.25*width, 0.75*height}, 3*M_PI/4}}
    };
    auto s_2 = boundary.GetSourcesIdsPointsAndAngles();
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
    Boundary boundary;
    BOOST_CHECK_THROW(boundary.Check(), Exception);
    boundary.MakeRectangular();

    auto& outer_boundary = boundary.GetOuterBoundary();
    BOOST_TEST(outer_boundary.inner_boundary == false );
    
    BOOST_TEST(boundary.GetHolesList() == vector<Point>{});

    auto sources_ids_points_and_angles_map = boundary.GetSourcesIdsPointsAndAngles();
    BOOST_TEST(sources_ids_points_and_angles_map.count(1));
    BOOST_TEST((sources_ids_points_and_angles_map.at(1).first == Point{0.25, 0}));
    BOOST_TEST(sources_ids_points_and_angles_map.at(1).second == M_PI/2);

    auto sources_ids = boundary.GetSourcesIds();
    BOOST_TEST(sources_ids == vector<SimpleBoundary::source_id_t>{1});
}