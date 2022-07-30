//Link to Boost
//#define BOOST_TEST_DYN_LINK

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
    *utf::description("Boundary_Condition class tests"))
{   
    BoundaryCondition bd;
    BOOST_TEST(bd.type == DIRICHLET);
    BOOST_TEST(bd.value == 0.);
}

BOOST_AUTO_TEST_CASE( Boundary_Condition_get_Method, 
    *utf::tolerance(eps)
    *utf::description("Boundary_Condition class Get method tests"))
{
    BoundaryConditions bd, db_neuman, bd_dirirchlet, bd_empty;

    BOOST_TEST(bd.Get(DIRICHLET) == bd_empty);
    BOOST_TEST(bd.Get(NEUMAN) == bd_empty);

    bd[1] = {DIRICHLET, 10};
    bd[2] = {NEUMAN, 11};
    bd[3] = {DIRICHLET, 12};
    bd[4] = {NEUMAN, 13};

    db_neuman[2]={NEUMAN, 11};
    db_neuman[4] = {NEUMAN, 13};
    BOOST_TEST(bd.Get(NEUMAN) == db_neuman);
    
    bd_dirirchlet[1] = {DIRICHLET, 10};
    bd_dirirchlet[3] = {DIRICHLET, 12};
    BOOST_TEST(bd.Get(DIRICHLET) == bd_dirirchlet);

    BOOST_CHECK_THROW(bd.Get((t_boundary)2), Exception);
    BOOST_CHECK_THROW(bd.Get((t_boundary)3), Exception);
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
    BOOST_TEST((l1 == l2));
}

BOOST_AUTO_TEST_CASE( SimpleBoundary_ReplaceElement, 
    *utf::tolerance(eps))
{
    Boundary b_base;
    b_base.vertices = {{0, 0}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    b_base.lines = {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 4, 4}, {4, 0, 5}};

    Boundary b_insert;
    b_insert.vertices = {{0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    b_insert.lines = {{0, 1, 0}, {1, 2, 0}, {2, 3, 0}};

    b_base.ReplaceElement(1, b_insert);
    
    auto expected_vertices = vector<Point>{{0, 0}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    auto expected_lines = vector<Line>{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};

    BOOST_TEST(b_base.vertices == expected_vertices);
    BOOST_TEST(b_base.lines == expected_lines);

    //now lets insert empty boundary
    Boundary b_insert_1;
    b_base.ReplaceElement(1, b_insert_1);
    b_base.ReplaceElement(3, b_insert_1);
    BOOST_TEST(b_base.vertices == expected_vertices);
    BOOST_TEST(b_base.lines == expected_lines);

    //insert boundary with only one element
    Boundary b_insert_2;
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
    Boundary b_base, b_insert_1, b_insert_2;
    b_base.Append(b_insert_1);
    BOOST_TEST(b_base.vertices.empty());
    BOOST_TEST(b_base.lines.empty());    

    b_insert_1.vertices = {{0, 0}, {0, 1}};
    b_insert_1.lines = {{0, 1, 1}};

    b_base.Append(b_insert_1);
    
    auto expected_vertices_1 = vector<Point>{{0, 0}, {0, 1}};
    for(size_t i = 0; i < expected_vertices_1.size(); ++i)
        BOOST_TEST((b_base.vertices.at(i) == expected_vertices_1.at(i)));
    
    auto expected_lines_1 = vector<Line>{{0, 1, 1}};
    for(size_t i = 0; i < expected_lines_1.size(); ++i)
        BOOST_TEST((b_base.lines.at(i) == expected_lines_1.at(i)));

    b_insert_2.vertices = {{0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    b_insert_2.lines = {{0, 1, 0}, {1, 2, 0}, {2, 3, 0}};

    b_base.Append(b_insert_2);
    
    auto expected_vertices_2 = vector<Point>{{0, 0}, {0, 1}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    for(size_t i = 0; i < expected_vertices_2.size(); ++i)
        BOOST_TEST((b_base.vertices.at(i) == expected_vertices_2.at(i)));
    
    auto expected_lines_2 = vector<Line>{{0, 1, 1}, {2, 3, 0}, {3, 4, 0}, {4, 5, 0}};
    for(size_t i = 0; i < expected_lines_2.size(); ++i)
        BOOST_TEST((b_base.lines.at(i) == expected_lines_2.at(i)));
}