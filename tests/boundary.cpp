//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Boundary Class and Branch"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <sstream>      // std::stringstream

#include "boundary.hpp"

namespace utf = boost::unit_test;
namespace bdata = boost::unit_test::data;
using namespace River;

//-----------------------------------------------------
// t_boundary
//-----------------------------------------------------
BOOST_AUTO_TEST_CASE( t_boundary_enum, 
    *utf::description("t_boundary_enum") *utf::tolerance(EPS) ){
        
    BOOST_TEST(t_boundary::DIRICHLET == 0);
    BOOST_TEST(t_boundary::NEUMAN == 1);
    BOOST_TEST(t_boundary::ROBIN == 2);

}

//-----------------------------------------------------
//BoundaryCondition
//-----------------------------------------------------
BOOST_TEST_DECORATOR( *utf::description("BoundaryCondition class") *utf::tolerance(EPS) )
BOOST_DATA_TEST_CASE( boundary_condition, bdata::xrange<int>(0, 2, 1) * bdata::xrange<double>(-5., 5., 0.5), type, value) {

    BoundaryCondition 
        boundary_cond_1{(t_boundary)type, value},
        boundary_cond_2;
    
    BOOST_TEST((boundary_cond_1 == boundary_cond_2) == (type == t_boundary::DIRICHLET && value == 0.));
    
}


BOOST_AUTO_TEST_CASE( t_boundary_print, 
    *utf::description("t_boundary_print") *utf::tolerance(EPS) ){

    BoundaryCondition 
        boundary_cond{DIRICHLET, 1},
        a;

    stringstream ss;
    ss << boundary_cond;
    auto result = ss.str();
    BOOST_TEST(result == "boudary type: Dirichlet,  value: 1");
    
}

//-----------------------------------------------------
//BoundaryConditions
//-----------------------------------------------------
BOOST_AUTO_TEST_CASE( BoundaryConditionsGet, 
    *utf::description("BoundaryConditions.Get(type)") *utf::tolerance(EPS) ){
    
    BoundaryConditions boundary_conditions;
    boundary_conditions[1] = {DIRICHLET, 0.};
    boundary_conditions[2] = {NEUMAN, 1.};
    boundary_conditions[3] = {ROBIN, 2.};
    boundary_conditions[4] = {DIRICHLET, 3.};
    boundary_conditions[5] = {NEUMAN, 4.};
    boundary_conditions[6] = {DIRICHLET, 5.};

    t_BoundaryConditions 
        boundary_conditions_expected_dirichlet = {
            {1, {DIRICHLET, 0.}}, 
            {4, {DIRICHLET, 3.}}, 
            {6, {DIRICHLET, 5.}}, 
        },
        boundary_conditions_expected_neuman = {
            {2, {NEUMAN, 1.}}, 
            {5, {NEUMAN, 4.}}
        },
        boundary_conditions_expected_robin = {
            {3, {ROBIN, 2.}}
        };

    BOOST_TEST(boundary_conditions(DIRICHLET) == boundary_conditions_expected_dirichlet);
    BOOST_TEST(boundary_conditions(NEUMAN) == boundary_conditions_expected_neuman);
    BOOST_TEST(boundary_conditions(ROBIN) == boundary_conditions_expected_robin);
        
}

BOOST_AUTO_TEST_CASE( t_boundary_conditions_print, 
    *utf::description("boundary_conditions_print") *utf::tolerance(EPS) ){

    BoundaryConditions boundary_cond;

    boundary_cond[1] = {DIRICHLET, 1.};

    stringstream ss;
    ss << boundary_cond;
    auto result = ss.str();
    BOOST_TEST(result == "id = 1,  boudary type: Dirichlet,  value: 1\n");
    
}

//-----------------------------------------------------
//Sources
//-----------------------------------------------------
BOOST_AUTO_TEST_CASE( sources, 
    *utf::description("sources") *utf::tolerance(EPS) ){

    Sources sources;

    sources[1] = {0, 0};
    sources[2] = {1, 5};
    sources[3] = {2, 6};
    sources[4] = {3, 7};
    sources[5] = {4, 8}; 

    BOOST_TEST(sources.at(1).first == 0);
    BOOST_TEST(sources.at(1).second == 0);
    BOOST_TEST(sources.at(5).first == 4);
    BOOST_TEST(sources.at(5).second == 8);

    //GetSourcesIds
    BOOST_TEST((sources.getSourcesIds() == t_sources_ids{1, 2, 3, 4, 5}));
    
}

//-----------------------------------------------------
//Line
//-----------------------------------------------------
BOOST_AUTO_TEST_CASE( line, 
    *utf::description("line") *utf::tolerance(EPS) ){

    Line 
        l1, 
        l2{1, 2, 3},
        l3{l2};

    BOOST_TEST(l1.p1 == 0);
    BOOST_TEST(l1.p2 == 0);
    BOOST_TEST(l1.boundary_id == 0);

    BOOST_TEST(l2.p1 == 1);
    BOOST_TEST(l2.p2 == 2);
    BOOST_TEST(l2.boundary_id == 3);

    BOOST_TEST(l3.p1 == 1);
    BOOST_TEST(l3.p2 == 2);
    BOOST_TEST(l3.boundary_id == 3);

    BOOST_TEST(l2 == l3);
    
}

//-----------------------------------------------------
//Boundary
//-----------------------------------------------------
BOOST_AUTO_TEST_CASE( boundary_replace_element, 
    *utf::description("class: boundary,  method: ReplaceElement") *utf::tolerance(EPS))
{   
    // first case
    // definition of boundary
    Boundary empty_boundary;

    // tests for empty boundary
    BOOST_TEST(empty_boundary.vertices.empty());
    BOOST_TEST(empty_boundary.lines.empty());
    BOOST_TEST(empty_boundary.holes.empty());
    BOOST_TEST(empty_boundary.name == "");

    // second case
    // replaccement in empty boundary should give error
    BOOST_CHECK_THROW(empty_boundary.replaceElement(0, empty_boundary), Exception);

    // third case
    // replacment of existing vertice by empty boundary should act like deletion of the vertice
    Boundary boundary;
    boundary.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    boundary.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};

    boundary.replaceElement(0, empty_boundary);
    auto vertices_expected = t_PointList{{1, 0}, {1, 1}, {0, 1}};
    auto lines_expected = t_LineList{{1, 2, 3}, {2, 3, 4}};
    BOOST_TEST(boundary.vertices == vertices_expected);
    BOOST_TEST(boundary.lines == lines_expected);

    boundary.replaceElement(0, empty_boundary);
    boundary.replaceElement(0, empty_boundary);
    boundary.replaceElement(0, empty_boundary);

    BOOST_TEST(boundary.vertices.empty());
    BOOST_TEST(boundary.lines.empty());

    // 4-th case
    // replacement by same element.. boundary should stay same
    boundary = Boundary();
    Boundary one_element_bound;
    boundary.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    boundary.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};

    // first
    one_element_bound.vertices = {{0, 0}};
    one_element_bound.lines = {{0, 1, 1}};
    boundary.replaceElement(0, one_element_bound);
    vertices_expected = t_PointList{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    lines_expected = t_LineList{{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};
    BOOST_TEST(boundary.vertices == vertices_expected);
    BOOST_TEST(boundary.lines == lines_expected);

    // second
    one_element_bound.vertices = {{1, 0}};
    one_element_bound.lines = {{1, 2, 3}};
    boundary.replaceElement(1, one_element_bound);
    vertices_expected = t_PointList{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    lines_expected = t_LineList{{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};
    BOOST_TEST(boundary.vertices == vertices_expected);
    BOOST_TEST(boundary.lines == lines_expected);

    // last
    one_element_bound.vertices = {{0, 1}};
    one_element_bound.lines = {{3, 0, 5}};
    boundary.replaceElement(3, one_element_bound);
    vertices_expected = t_PointList{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    lines_expected = t_LineList{{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};
    BOOST_TEST(boundary.vertices == vertices_expected);
    BOOST_TEST(boundary.lines == lines_expected);

    // fift case normal replacement by nonempty boundary
    Boundary wystup_boundary;
    wystup_boundary.vertices = {{0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}};
    wystup_boundary.lines = {{0, 1, 0}, {1, 2, 0}, {2, 3, 0}};
    boundary.vertices = {{0, 0}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    boundary.lines = {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 4, 4}, {4, 0, 5}};

    boundary.replaceElement(1, wystup_boundary);

    vertices_expected = t_PointList{{0, 0}, {0.3, 0}, {0.3, 0.1}, {0.5, 0.1}, {0.5, 0}, {1, 0}, {1, 1}, {0, 1}};
    lines_expected = t_LineList{{0, 1, 1}, {1, 2, 0}, {2, 3, 0}, {3, 4, 0}, {4, 5, 2},{5, 6, 3}, {6, 7, 4}, {7, 0, 5}};
    BOOST_TEST(boundary.vertices == vertices_expected);
    BOOST_TEST(boundary.lines == lines_expected);
}

BOOST_AUTO_TEST_CASE( boundary_append_element, 
    *utf::description("class: boundary,  method: append") *utf::tolerance(EPS))
{

    //1st case: append empty to empty
    Boundary empty_boundary;

    //2nd case: append nonempty to empty
    Boundary some_boundary;
    some_boundary.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    some_boundary.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};

    empty_boundary.append(some_boundary);

    BOOST_TEST(empty_boundary.vertices == some_boundary.vertices);
    BOOST_TEST(empty_boundary.lines == some_boundary.lines);

    //3d case: several appends in a row  
    auto vertices_expected = t_PointList{
        {0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, 
        {1, 1}, {0, 1}, {0, 0}, {1, 0}, {1, 1}, {0, 1}};
    auto lines_expected = t_LineList{
        {0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}, {4, 5, 1}, {5, 6, 3}, 
        {6, 7, 4}, {7, 4, 5}, {8, 9, 1}, {9, 10, 3}, {10, 11, 4}, {11, 8, 5}};
    empty_boundary.append(some_boundary);
    empty_boundary.append(some_boundary);

    BOOST_TEST(empty_boundary.vertices == vertices_expected);
    BOOST_TEST(empty_boundary.lines == lines_expected);

}

BOOST_AUTO_TEST_CASE( boundary_equal, 
    *utf::description("class: boundary,  method: equal") *utf::tolerance(EPS))
{
    Boundary 
        some_boundary_1,
        some_boundary_2,
        some_boundary_3,
        empty_boundary;

    some_boundary_1.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    some_boundary_1.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};

    some_boundary_2.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    some_boundary_2.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 0, 5}};

    some_boundary_3.vertices = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {2, 2}};
    some_boundary_3.lines = {{0, 1, 1}, {1, 2, 3}, {2, 3, 4}, {3, 4, 5}, {4, 0, 6}};
    

    //1st case positive
    BOOST_TEST(empty_boundary == empty_boundary);
    BOOST_TEST(some_boundary_1 == some_boundary_1);
    BOOST_TEST(some_boundary_1 == some_boundary_2);

    //2nd case negatic
    BOOST_TEST(!(empty_boundary == some_boundary_1));
    BOOST_TEST(!(some_boundary_1 == some_boundary_3));
}

BOOST_AUTO_TEST_CASE( boundary_print, 
    *utf::description("class: boundary,  method: print") *utf::tolerance(EPS))
{
    Boundary boundary;

    boundary.vertices = {{0, 0}, {1, 0}};
    boundary.lines = {{0, 1, 1}, {1, 2, 3}};
    boundary.name = "typical boundary";
    boundary.holes = {{0.5, 0.5}, {0.6, 0.6}};
    boundary.inner_boundary = true;

    stringstream ss;
    ss << boundary;
    auto result = ss.str();

    BOOST_TEST(result == "Name: typical boundary\ninner boudary: 1\npoint: 0.5, 0.5\npoint: 0.6, 0.6\nVertices:\npoint: 0, 0\npoint: 1, 0\nLines:\nline [ 0, 1], boundary_id = 1\nline [ 1, 2], boundary_id = 3\n");
}

BOOST_AUTO_TEST_CASE( boundary_append_point, 
    *utf::description("class: boundary,  method: append cartesian point") *utf::tolerance(EPS))
{
    Boundary boundary;

    boundary.append(Point{0, 0}, 1);

    BOOST_TEST(boundary.vertices == t_PointList{{0, 0}});
    BOOST_TEST(boundary.lines.empty());
    BOOST_TEST(boundary.holes.empty());

    boundary.append(Point{1, 1}, 2);

    BOOST_TEST(boundary.vertices == t_PointList{{0, 0}, {1, 1}});
    BOOST_TEST(boundary.lines == t_LinesList{{0, 1, 2}});
    BOOST_TEST(boundary.holes.empty());
}

BOOST_AUTO_TEST_CASE( boundary_append_polar, 
    *utf::description("class: boundary,  method: append cartesian polar") *utf::tolerance(EPS))
{
    Boundary boundary;

    boundary.append(Polar{0, 0}, 1);

    BOOST_TEST(boundary.vertices == t_PointList{{0, 0}});
    BOOST_TEST(boundary.lines.empty());
    BOOST_TEST(boundary.holes.empty());

    boundary.append(Polar{1, 0}, 2);

    BOOST_TEST(boundary.vertices == t_PointList{{0, 0}, {1, 0}});
    BOOST_TEST(boundary.lines == t_LinesList{{0, 1, 2}});
    BOOST_TEST(boundary.holes.empty());

    boundary.append(Polar{1, M_PI/2}, 3);

    BOOST_TEST(boundary.vertices == t_PointList{{0, 0}, {1, 0}, {1, 1}});
    BOOST_TEST(boundary.lines == t_LinesList{{0, 1, 2}, {1, 2, 3}});
    BOOST_TEST(boundary.holes.empty());

}

BOOST_AUTO_TEST_CASE( boundary_shrink, 
    *utf::description("class: boundary,  method: shrink") *utf::tolerance(EPS))
{
    // 1st case shrink of empty boundary
    Boundary empty;
    empty.shrink();
}

BOOST_AUTO_TEST_CASE( boundary_remove_tip_point, 
    *utf::description("class: boundary,  method: removeTipPoint") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_tip_point, 
    *utf::description("class: boundary,  method: tipPoint") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_tip_vector, 
    *utf::description("class: boundary,  method: tipVector") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_vector, 
    *utf::description("class: boundary,  method: vector") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_tip_angle, 
    *utf::description("class: boundary,  method: tipAngle") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_source_point, 
    *utf::description("class: boundary,  method: sourcePoint") *utf::tolerance(EPS))
{

}

BOOST_AUTO_TEST_CASE( boundary_length, 
    *utf::description("class: boundary,  method: length") *utf::tolerance(EPS))
{

}

/*

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
        {//vertices(counterclockwise order)
            {0, 0},
            {source_x_position, 0}, 
            {width, 0}, 
            {width, height}, 
            {0, height}
        }, 
        {//lines
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, 4},
            {4, 0, 5} 
        }, 
        false,//this is not inner boudary,
        {},//holes
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
        {//vertices(counterclockwise order)
            {0.25*width, 0.75*height},
            {0.75*width, 0.75*height}, 
            {0.75*width, 0.25*height}, 
            {0.25*width, 0.25*height}
        }, 
        {//lines*
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 0, 4}
        }, 
        false,//this is not inner boudary,
        {{0.5*width, 0.5*height}},//holes,
        "outer rectangular boudary"
    };
    boundary[2] = inner_simple_boundary;
    sources[2] = {2, 1};
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
    s = Boundaries::trees_interface_t
    {
        {1, {{0, 0}, M_PI/4}},
        {2, {{source_x_position, 0}, M_PI/2}},
        {3, {{width, 0}, 3*M_PI/4}},
        {4, {{width, height}, 5*M_PI/4}},
        {5, {{0, height}, 7*M_PI/4}},
        {6, {{0.25*width, 0.75*height}, 3*M_PI/4}},
        {7, {{0.75*width, 0.75*height}, M_PI/4}},
        {8, {{0.75*width, 0.25*height}, -M_PI/4}},
        {9, {{0.25*width, 0.25*height}, 5*M_PI/4}},
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


BOOST_AUTO_TEST_CASE( Sources_test, 
    *utf::tolerance(eps))
{   
    Sources sources;
    sources[1] = {1, 1};

    BOOST_TEST((sources.GetSourcesIds() == vector<t_source_id>{1}));

    sources.clear();
    BOOST_TEST((sources.GetSourcesIds() == vector<t_source_id>{}));

    sources[1] = {1,1};
    sources[2] = {1,1};
    sources[3] = {1,1};
    BOOST_TEST((sources.GetSourcesIds() == vector<t_source_id>{1, 2, 3}));
}

BOOST_AUTO_TEST_CASE( Branch_Class, 
    *utf::tolerance(eps))
{   
    //constructor: empty branch
    auto source_point = Point{0, 0};
    auto source_angle = M_PI/2;
    auto br = Branch(source_point, source_angle);

    BOOST_TEST(br.size() == 1);
    BOOST_TEST(br.Lenght() == 0);
    BOOST_TEST(br.TipPoint() == source_point);
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);
    BOOST_CHECK_THROW(br.TipVector(), Exception);
    BOOST_CHECK_THROW(br.Vector(0), Exception);
    BOOST_CHECK_THROW(br.Vector(1), Exception);
    BOOST_TEST(br.SourcePoint() == source_point );
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST((br == vector<Point>{source_point}));

    //now lets modify a little this branch
    BOOST_CHECK_NO_THROW(br.AddPoint(Point{0, 1}));
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Point{0, 2}));
    BOOST_CHECK_NO_THROW(br.AddPoint(Polar{1, 0}));
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Polar{1, M_PI/2}));

    BOOST_TEST(br.size() == 5);
    BOOST_TEST(br.Lenght() == 4);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 4}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point);
    BOOST_TEST((br == vector<Point>{{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}}));

    //now let remove two tip point
    BOOST_CHECK_NO_THROW(br.RemoveTipPoint().RemoveTipPoint());
    BOOST_TEST(br.size() == 3);
    BOOST_TEST(br.Lenght() == 2);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 2}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point );
    BOOST_TEST((br == vector<Point>{{0, 0}, {0, 1}, {0, 2}}));
    
    BOOST_CHECK_NO_THROW(br.Shrink(2));
    BOOST_TEST(br.Lenght() == 0);
    BOOST_TEST(br.size() == 1);
    BOOST_TEST(br.TipPoint() == source_point);
    BOOST_TEST(br.SourcePoint() == source_point);
    BOOST_TEST(br.TipAngle() == br.SourceAngle());
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);

    //now let add points with different angles
    source_point = Point{0, 0};
    br = Branch(source_point, M_PI/6);
    BOOST_CHECK_NO_THROW(br.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0}));
    Point test_p_1{sqrt(3), 1};
    BOOST_TEST(br.TipPoint() == test_p_1);
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Polar{2, 5./6.* M_PI}));
    Point test_p_2{0, 2};
    BOOST_TEST(br.TipPoint() == test_p_2);

    //now let make circle in clockwise direction:
    source_point = Point{0, 0};
    br = Branch(source_point, M_PI/6);
    //clockwise
    for(size_t i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, 2*M_PI/10});
    //counterclockwise
    for(size_t i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, -2*M_PI/10});

    BOOST_TEST(br.TipPoint() == br.SourcePoint());
    BOOST_TEST(source_point == br.SourcePoint());

    br.Shrink(20);
    BOOST_TEST(br.TipPoint() == br.SourcePoint());
    BOOST_TEST(source_point == br.SourcePoint());

    br = Branch(source_point, 0);
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.Shrink(0.5);
    BOOST_TEST(br.size() == 3);
    auto test_p_4 = Point{1.5, 0};
    BOOST_TEST(br.TipPoint() == test_p_4);
    br.Shrink(0.5);
    BOOST_TEST(br.size() == 2);
    BOOST_TEST(br.Lenght() == 1);
    auto test_p_3 = Point{1, 0};
    BOOST_TEST(br.TipPoint() == test_p_3);
}

BOOST_AUTO_TEST_CASE( Branch_vector, 
    *utf::tolerance(eps))
{   
    Branch br{Point{0, 0}, 0};
    BOOST_CHECK_THROW(br.Vector(0), Exception);

    br.AddPoint(Polar{1, 0});
    auto test_p = Point{1, 0};
    BOOST_TEST(br.Vector(1) == test_p);
    BOOST_TEST(br.size() == 2);
    BOOST_CHECK_THROW(br.Vector(2), Exception);

    br.AddPoint(Polar{1, M_PI/2});
    auto test_p_2 = Point{0, 1};
    BOOST_TEST(br.Vector(2) == test_p_2);
    BOOST_CHECK_THROW(br.Vector(3), Exception);
}

BOOST_AUTO_TEST_CASE( Branch_equal, 
    *utf::tolerance(eps))
{  
    Branch 
        br1{Point{0, 0}, 0}, 
        br2{Point{0, 0}, 0};
    
    BOOST_TEST(br1 == br2);

    br1.AddPoint(Point{1, 2});

    BOOST_TEST(!(br1 == br2));

    br2.AddPoint(Point{1, 2});

    BOOST_TEST(br1 == br2);
}
*/