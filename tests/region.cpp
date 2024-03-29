//Link to Boost
//#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Region class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include "region.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------

BOOST_AUTO_TEST_CASE( Boundaries_NormalAngle, 
    *utf::tolerance(eps))
{   
    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {0, 1e-10}) == M_PI);
    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);

    //_ _
    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {2, 0}) == M_PI/2);
    // _|
    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {1, 1}) == M_PI/4*3);
    // -|
    BOOST_TEST(Region::NormalAngle({1, 0}, {1, 1}, {0, 1}) == M_PI*5/4);
    // |-
    BOOST_TEST(Region::NormalAngle({1, 1}, {0, 1}, {0, 0}) == 7*M_PI/4);
    //|_
    BOOST_TEST(Region::NormalAngle({0, 1}, {0, 0}, {1, 0}) == M_PI/4);

    BOOST_TEST(Region::NormalAngle({0, 0}, {1, 0}, {1, -1}) == M_PI/4);
}

BOOST_AUTO_TEST_CASE( Boundaries_some_methods, 
    *utf::tolerance(eps))
{   
    BOOST_TEST_CHECKPOINT("empty test");
    Region region;

    //checks if boundary is empty
    BOOST_TEST_CHECKPOINT("Check");
    BOOST_CHECK_THROW(region.Check(), Exception);
    BOOST_TEST_CHECKPOINT("GetHolesList");
    BOOST_TEST(region.GetHolesList() == vector<Point>{});
    BOOST_TEST_CHECKPOINT("GetSourcesIdsPointsAndAngles");
    Sources sources;
    BOOST_TEST(region.GetSourcesIdsPointsAndAngles(sources) == t_rivers_interface{});
    
    BOOST_TEST_CHECKPOINT("With one boundary test");
    const auto source_x_position = 0.25, width = 1., height = 1.;
    auto outer_simple_boundary = Boundary
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
        }
    };
    region[1] = outer_simple_boundary;
    sources[1] = {1, 1};

    //non empty boudary checks
    BOOST_CHECK_NO_THROW(region.Check());
    BOOST_TEST(region.GetHolesList() == vector<Point>{});
    auto s = t_rivers_interface{{1,{{source_x_position, 0}, M_PI/2}}};
    BOOST_TEST(region.GetSourcesIdsPointsAndAngles(sources) == s);

    //add inner boudnary
    BOOST_TEST_CHECKPOINT("With two boundaries test");
    auto inner_simple_boundary = Boundary
    {
        {/*vertices(counterclockwise order)*/
            {0.25*width, 0.75*height},
            {0.75*width, 0.75*height}, 
            {0.75*width, 0.25*height}, 
            {0.25*width, 0.25*height}
        }, 
        {/*lines*/
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 0, 4}
        }
    };
    region.holes = {{0.5*width, 0.5*height}};
    region[2] = inner_simple_boundary;
    sources[2] = {2, 1};

    //checks with inner boundaries
    BOOST_CHECK_NO_THROW(region.Check());
    auto holes_list = vector<Point>{{0.5*width, 0.5*height}};
    BOOST_TEST(region.GetHolesList() == holes_list);
    s = t_rivers_interface{
        {1, {{source_x_position, 0}, M_PI/2}},
        {2, {{0.75 * width, 0.75 * height}, M_PI/4}}
    };
    BOOST_TEST(region.GetSourcesIdsPointsAndAngles(sources) == s);

    sources[1] = {1, 0};
    sources[2] = {1, 1};
    sources[3] = {1, 2};
    sources[4] = {1, 3};
    sources[5] = {1, 4};
    sources[6] = {2, 0};
    sources[7] = {2, 1};
    sources[8] = {2, 2};
    sources[9] = {2, 3};
   
    BOOST_TEST(region.GetHolesList() == holes_list);
    s = t_rivers_interface
    {
        {1, {{0, 0}, M_PI/4}},
        {2, {{source_x_position, 0}, M_PI/2}},
        {3, {{width, 0}, 3*M_PI/4}},
        {4, {{width, height}, 5*M_PI/4}},
        {5, {{0, height}, 7*M_PI/4}},
        {6, {{0.25*width, 0.75 * height}, 3*M_PI/4}},
        {7, {{0.75*width, 0.75 * height}, M_PI/4}},
        {8, {{0.75*width, 0.25 * height}, -M_PI/4}},
        {9, {{0.25*width, 0.25 * height}, 5*M_PI/4}},
    };
    auto s_2 = region.GetSourcesIdsPointsAndAngles(sources);
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
    Region region;
    BOOST_CHECK_THROW(region.Check(), Exception);
    auto sources = region.MakeRectangular();
    
    BOOST_TEST(region.GetHolesList() == vector<Point>{});

    auto sources_ids_points_and_angles_map = region.GetSourcesIdsPointsAndAngles(sources);
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

BOOST_AUTO_TEST_CASE( tree_vertices, 
    *utf::tolerance(eps))
{   
    Boundary tree_vertices;
    Rivers tr;
    BOOST_CHECK_THROW(RiversBoundary(tree_vertices, tr, 1, 0.01), Exception);
}


BOOST_AUTO_TEST_CASE( tree_boundary, 
    *utf::tolerance(eps))
{
    //empty tree
    Rivers rivers;
    Boundary boundary;
    BOOST_CHECK_THROW(RiversBoundary(boundary, rivers, 1, 0.01), Exception);

    //initiazlized tree
    Region region;
    auto sources = region.MakeRectangular(1, 1, 0.5);
    auto source_id = 1;

    boundary = Boundary{};
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    RiversBoundary(boundary, rivers, source_id, 0.1);
    BOOST_TEST(boundary.lines.empty());

    //tree with two points
    t_boundary_id boundary_id = 10;
    rivers.at(source_id).AddPoint(Polar{0.01, 0}, boundary_id);
    boundary = Boundary{};
    RiversBoundary(boundary, rivers, source_id, 0.1);

    auto expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}};
    BOOST_TEST(boundary.lines.size() == 2);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(boundary.lines.at(i) == expected_lines.at(i));

    //tree with three points
    rivers.at(source_id).AddPoint(Polar{0.01, 0}, boundary_id);
    boundary = Boundary{};
    RiversBoundary(boundary, rivers, source_id, 0.1);

    expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}, {2, 3, 10}, {3, 4, 10}};
    BOOST_TEST(boundary.lines.size() == 4);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(boundary.lines.at(i) == expected_lines.at(i));
}

BOOST_AUTO_TEST_CASE( Simple_Boundary_Generator, 
    *utf::tolerance(eps))
{
    Region region;
    auto sources = region.MakeRectangularWithHole();
    auto source_id = 1;
    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));

    auto boundary = BoundaryGenerator(sources, region, rivers, 0.01);

    BOOST_TEST(boundary.lines.size() == 13);

    rivers.at(source_id).AddPoint(Polar{0.01, 0}, 100);
    boundary = BoundaryGenerator(sources, region, rivers, 0.01);
    BOOST_TEST(boundary.lines.size() == 15);

    auto vertices = t_PointList
    {
        {0, 0}, {0.25-0.01/2, 0}, {0.25, 0.01}, {0.25+0.01/2, 0}, {1, 0}, {1, 1}, {0, 1},
        {0.25, 0.75}, {0.75, 0.75}, {0.75, 0.25}, {0.25, 0.25},
        {0.8, 0.9}, {0.9, 0.9}, {0.9, 0.8}, {0.8, 0.8}
    };
    
    auto holes = t_PointList{
        {0.5, 0.5}, {0.85, 0.85}
    };

    auto lines =  vector<Line>{
        {0, 1, 1}, {1, 2, 100}, {2, 3, 100}, {3, 4, 2}, {4, 5, 3}, {5, 6, 4}, {6, 0, 5},
        {7, 8, 6}, {8, 9, 7}, {9, 10, 8}, {10, 7, 9},
        {11, 12, 10}, {12, 13, 11}, {13, 14, 12}, {14, 11, 13},
    };

    BOOST_TEST(region.GetHolesList() == holes);
    BOOST_TEST(boundary.vertices.size() == 15);
    for(size_t i = 0; i < vertices.size(); ++i)
    {
        BOOST_TEST(boundary.vertices.at(i) == vertices.at(i));
        BOOST_TEST(boundary.lines.at(i) == lines.at(i));
    }
}

inline void TEST_POINT(River::Point p1, River::Point p2)
{
    BOOST_TEST(p1 == p2);
}

BOOST_AUTO_TEST_CASE( boundary_generator_new, 
    *utf::tolerance(eps))
{   
    Region region;
    auto sources = region.MakeRectangular(1, 1, 0.5);

    Rivers rivers;
    rivers.Initialize(
        region.GetSourcesIdsPointsAndAngles(sources));
    
    Boundary boundary;
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 1);
    auto p = River::Point{0.5, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);

    t_boundary_id boundary_id = 0;
    auto& br = rivers.at(1);
    br.AddPoint(River::Polar{0.1, 0}, boundary_id);
    boundary = Boundary{};
    
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 3);
    p = River::Point{0.5 - 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);
    p = River::Point{0.5, 0.1};
    BOOST_TEST(boundary.vertices.at(1) == p);
    p = River::Point{0.5 + 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(2) == p);

    boundary = Boundary{};
    br.AddPoint(River::Polar{0.1, 0}, boundary_id);
    Branch left_branch(br.TipPoint(), br.TipAngle() + M_PI/2);
    left_branch.AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    Branch right_branch(br.TipPoint(), br.TipAngle() - M_PI/2);
    right_branch.AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    rivers.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<t_branch_id> {2, 3};
    BOOST_TEST(rivers.TipBranchesIds() == tip_ids);

    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 17);

    //left source branch
    TEST_POINT(boundary.vertices.at(0), River::Point{0.5 - 1e-3/2, 0});
    TEST_POINT(boundary.vertices.at(1), River::Point{0.5 - 1e-3/2, 0.1});

    //left left branch
    TEST_POINT(boundary.vertices.at(2), River::Point{0.5, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(3), River::Point{0.4, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(4), River::Point{0.3, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(5), River::Point{0.2, 0.2});
    //right left branch
    TEST_POINT(boundary.vertices.at(6), River::Point{0.3, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(7), River::Point{0.4, 0.2 + 1e-3/2});
    //left right branch
    TEST_POINT(boundary.vertices.at(8), River::Point{0.5, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(9), River::Point{0.6, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(10), River::Point{0.7, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(11), River::Point{0.8, 0.2});
    //right right branch
    TEST_POINT(boundary.vertices.at(12), River::Point{0.7, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(13), River::Point{0.6, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(14), River::Point{0.5, 0.2 - 1e-3/2});
    //right source branch
    TEST_POINT(boundary.vertices.at(15), River::Point{0.5 + 1e-3/2, 0.1 });
    TEST_POINT(boundary.vertices.at(16), River::Point{0.5 + 1e-3/2, 0.0 });
    TEST_POINT(boundary.vertices.back(), River::Point{0.5 + 1e-3/2, 0});

}

BOOST_AUTO_TEST_CASE( boundary_generator_new_2, 
    *utf::tolerance(eps))
{   
    Region region;
    auto sources = region.MakeRectangular(1,1,0.5);

    Rivers rivers;
    rivers.Initialize(
        region.GetSourcesIdsPointsAndAngles(sources));
    
    Boundary boundary;
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 1);
    

    Branch l(rivers.at(1).TipPoint(), rivers.at(1).TipAngle()+M_PI/4),
        r(rivers.at(1).TipPoint(), rivers.at(1).TipAngle() - M_PI/4);
    
    auto[i1, i2] = rivers.AddSubBranches(1, l, r);
    cout << i2;
    BOOST_TEST(boundary.vertices.size() == 1);
    auto p = River::Point{0.5, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);

    t_boundary_id boundary_id = 0;
    rivers.at(i1).AddPoint(Polar{0.1, 0}, boundary_id);
    boundary.vertices.clear();
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 3);
    TEST_POINT(boundary.vertices.at(0), River::Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(boundary.vertices.at(1), River::Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(boundary.vertices.at(2), River::Point{0.5, 0});    
}

BOOST_AUTO_TEST_CASE( boundary_generator_new_new, 
    *utf::tolerance(eps))
{   
    Region region;
    auto sources = region.MakeRectangular(1, 1, 0.5);

    Rivers rivers;
    rivers.Initialize(
        region.GetSourcesIdsPointsAndAngles(sources));
    
    Boundary boundary;
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 1);
    auto p = River::Point{0.5, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);

    t_boundary_id boundary_id = 0;
    auto& br = rivers.at(1);
    br.AddPoint(Polar{0.1, 0}, boundary_id);
    boundary.vertices.clear();
    
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 3);
    p = River::Point{0.5 - 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);
    p = River::Point{0.5, 0.1};
    BOOST_TEST(boundary.vertices.at(1) == p);
    p = River::Point{0.5 + 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(2) == p);


    boundary = Boundary{};
    br.AddPoint(Polar{0.1, 0}, boundary_id);
    Branch left_branch(br.TipPoint(), br.TipAngle() + M_PI/2);
    left_branch.AddPoint(Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    Branch right_branch(br.TipPoint(), br.TipAngle() - M_PI/2);
    right_branch.AddPoint(Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    rivers.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<t_branch_id> {2, 3};
    BOOST_TEST(rivers.TipBranchesIds() == tip_ids);

    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 17);

    //left source branch
    TEST_POINT(boundary.vertices.at(0), River::Point{0.5 - 1e-3/2, 0});
    TEST_POINT(boundary.vertices.at(1), River::Point{0.5 - 1e-3/2, 0.1});

    //left left branch
    TEST_POINT(boundary.vertices.at(2), River::Point{0.5, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(3), River::Point{0.4, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(4), River::Point{0.3, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(5), River::Point{0.2, 0.2});
    //right left branch
    TEST_POINT(boundary.vertices.at(6), River::Point{0.3, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(7), River::Point{0.4, 0.2 + 1e-3/2});
    //left right branch
    TEST_POINT(boundary.vertices.at(8), River::Point{0.5, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(9), River::Point{0.6, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(10), River::Point{0.7, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(11), River::Point{0.8, 0.2});
    //right right branch
    TEST_POINT(boundary.vertices.at(12), River::Point{0.7, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(13), River::Point{0.6, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(14), River::Point{0.5, 0.2 - 1e-3/2});
    //right source branch
    TEST_POINT(boundary.vertices.at(15), River::Point{0.5 + 1e-3/2, 0.1 });
    TEST_POINT(boundary.vertices.at(16), River::Point{0.5 + 1e-3/2, 0.0 });
    TEST_POINT(boundary.vertices.back(), River::Point{0.5 + 1e-3/2, 0});

}

BOOST_AUTO_TEST_CASE( boundary_generator_new_2_lala, 
    *utf::tolerance(eps))
{   
    Region region;
    auto sources = region.MakeRectangular(1,1, 0.5);

    Rivers rivers;
    rivers.Initialize(
        region.GetSourcesIdsPointsAndAngles(sources));
    
    Boundary boundary;
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 1);


    Branch l(rivers.at(1).TipPoint(), rivers.at(1).TipAngle()+M_PI/4),
        r(rivers.at(1).TipPoint(), rivers.at(1).TipAngle() - M_PI/4);
    
    auto[i1, i2] = rivers.AddSubBranches(1, l, r);
    cout << i2;
    BOOST_TEST(boundary.vertices.size() == 1);
    auto p = River::Point{0.5, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);
    
    t_boundary_id boundary_id = 0;
    rivers.at(i1).AddPoint(Polar{0.1, 0}, boundary_id);
    boundary = Boundary{};
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 3);
    TEST_POINT(boundary.vertices.at(0), River::Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(boundary.vertices.at(1), River::Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(boundary.vertices.at(2), River::Point{0.5, 0});    
}

BOOST_AUTO_TEST_CASE( BoundaryGenerator_test, 
    *utf::tolerance(eps))
{   
    auto river_width = 0.01;
    //auto river_boundary_id = t_boundary_id{4};
    Region region;
    region[1] = 
    {
        {
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {0.5, 0},
            {0.6, 0},
            {0.7, 0},
            {0.8, 0}
        },
        {
            {0, 1, 2},
            {1, 2, 2},
            {2, 3, 2},
            {3, 4, 2},
            {4, 5, 2},
            {5, 6, 2},
            {6, 7, 2},
            {7, 0, 2},
        }
    };
    Sources sources;
    sources[1] = {1, 4};
    sources[2] = {1, 5};
    sources[3] = {1, 6};
    sources[4] = {1, 7};
    
    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    
    BOOST_TEST_MESSAGE( "Boundary Generator start" );
    auto border = BoundaryGenerator(sources, region, rivers, river_width);

    BOOST_TEST(border.vertices.size() == 8);
    //Vertices test
    //1
    BOOST_TEST(border.vertices[0].x == 1.);
    BOOST_TEST(border.vertices[0].y == 0.);
    //2
    BOOST_TEST(border.vertices[1].x == 1.);
    BOOST_TEST(border.vertices[1].y == 1.);
    //3
    BOOST_TEST(border.vertices[2].x == 0.);
    BOOST_TEST(border.vertices[2].y == 1.);
    //4
    BOOST_TEST(border.vertices[3].x == 0.);
    BOOST_TEST(border.vertices[3].y == 0.);
    //5
    BOOST_TEST(border.vertices[4].x == 0.5);
    BOOST_TEST(border.vertices[4].y == 0.);
    //6
    BOOST_TEST(border.vertices[5].x == 0.6);
    BOOST_TEST(border.vertices[5].y == 0.);
    //7
    BOOST_TEST(border.vertices[6].x == 0.7);
    BOOST_TEST(border.vertices[6].y == 0.);
    //8
    BOOST_TEST(border.vertices[7].x == 0.8);
    BOOST_TEST(border.vertices[7].y == 0.);

    //line lements
    BOOST_TEST(border.lines.size() == 8);
    //1
    BOOST_TEST(border.lines[0].p1 == 0);
    BOOST_TEST(border.lines[0].p2 == 1);
    //2
    BOOST_TEST(border.lines[1].p1 == 1);
    BOOST_TEST(border.lines[1].p2 == 2);
    //3
    BOOST_TEST(border.lines[2].p1 == 2);
    BOOST_TEST(border.lines[2].p2 == 3);
    //4
    BOOST_TEST(border.lines[3].p1 == 3);
    BOOST_TEST(border.lines[3].p2 == 4);
    //5
    BOOST_TEST(border.lines[4].p1 == 4);
    BOOST_TEST(border.lines[4].p2 == 5);
    //6
    BOOST_TEST(border.lines[5].p1 == 5);
    BOOST_TEST(border.lines[5].p2 == 6);
    //7
    BOOST_TEST(border.lines[6].p1 == 6);
    BOOST_TEST(border.lines[6].p2 == 7);
    //8
    BOOST_TEST(border.lines[7].p1 == 7);
    BOOST_TEST(border.lines[7].p2 == 0);
}



BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new, 
    *utf::tolerance(eps))
{   
    auto river_width = 0.02;
    auto river_boundary_id = 5;
    
    Region region;
    region[1] = 
    {
        {
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {0.5, 0},
            {0.6, 0},
            {0.7, 0},
            {0.8, 0}
        },
        {
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, (t_boundary_id)river_boundary_id},
            {4, 5, (t_boundary_id)river_boundary_id},
            {5, 6, (t_boundary_id)river_boundary_id},
            {6, 7, (t_boundary_id)river_boundary_id},
            {7, 0, (t_boundary_id)river_boundary_id},
        }
    };
    
    Sources sources;
    sources[1] = {1, 4};
    sources[2] = {1, 5};
    sources[3] = {1, 6};
    sources[4] = {1, 7};

    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    rivers.AddPolars(sources.GetSourcesIds(), {{1, 0}, {2, 0}, {3, 0}, {4, 0}}, 
        {(t_boundary_id)river_boundary_id, (t_boundary_id)river_boundary_id, (t_boundary_id)river_boundary_id, (t_boundary_id)river_boundary_id});
    
    auto border = BoundaryGenerator(sources, region, rivers, river_width);

    BOOST_TEST(border.vertices.size() == 16);
    //Vertices test
    //1
    BOOST_TEST(border.vertices[0].x == 1.);
    BOOST_TEST(border.vertices[0].y == 0.);
    //2
    BOOST_TEST(border.vertices[1].x == 1.);
    BOOST_TEST(border.vertices[1].y == 1.);
    //3
    BOOST_TEST(border.vertices[2].x == 0.);
    BOOST_TEST(border.vertices[2].y == 1.);
    //4
    BOOST_TEST(border.vertices[3].x == 0.);
    BOOST_TEST(border.vertices[3].y == 0.);
    //5
    BOOST_TEST(border.vertices[4].x == 0.49);
    BOOST_TEST(border.vertices[4].y == 0.);
    BOOST_TEST(border.vertices[5].x == 0.5);
    BOOST_TEST(border.vertices[5].y == 1.);
    BOOST_TEST(border.vertices[6].x == 0.51);
    BOOST_TEST(border.vertices[6].y == 0.);
    //7
    BOOST_TEST(border.vertices[7].x == 0.59);
    BOOST_TEST(border.vertices[7].y == 0.);
    BOOST_TEST(border.vertices[8].x == 0.6);
    BOOST_TEST(border.vertices[8].y == 2.);
    BOOST_TEST(border.vertices[9].x == 0.61);
    BOOST_TEST(border.vertices[9].y == 0.);
    //8
    BOOST_TEST(border.vertices[10].x == 0.69);
    BOOST_TEST(border.vertices[10].y == 0.);
    BOOST_TEST(border.vertices[11].x == 0.7);
    BOOST_TEST(border.vertices[11].y == 3.);
    BOOST_TEST(border.vertices[12].x == 0.71);
    BOOST_TEST(border.vertices[12].y == 0.);
    //9
    BOOST_TEST(border.vertices[13].x == 0.79);
    BOOST_TEST(border.vertices[13].y == 0.);
    BOOST_TEST(border.vertices[14].x == 0.8);
    BOOST_TEST(border.vertices[14].y == 4.);
    BOOST_TEST(border.vertices[15].x == 0.81);
    BOOST_TEST(border.vertices[15].y == 0.);

    //line lements
    BOOST_TEST(border.lines.size() == 16);
    //1
    BOOST_TEST(border.lines[0].p1 == 0);
    BOOST_TEST(border.lines[0].p2 == 1);
    BOOST_TEST(border.lines[0].boundary_id == 1);
    //2
    BOOST_TEST(border.lines[1].p1 == 1);
    BOOST_TEST(border.lines[1].p2 == 2);
    BOOST_TEST(border.lines[1].boundary_id == 2);
    //3
    BOOST_TEST(border.lines[2].p1 == 2);
    BOOST_TEST(border.lines[2].p2 == 3);
    BOOST_TEST(border.lines[2].boundary_id == 3);
    //4
    BOOST_TEST(border.lines[3].p1 == 3);
    BOOST_TEST(border.lines[3].p2 == 4);
    BOOST_TEST(border.lines[3].boundary_id == river_boundary_id);
    //5
    BOOST_TEST(border.lines[4].p1 == 4);
    BOOST_TEST(border.lines[4].p2 == 5);
    BOOST_TEST(border.lines[4].boundary_id == river_boundary_id);
    //6
    BOOST_TEST(border.lines[5].p1 == 5);
    BOOST_TEST(border.lines[5].p2 == 6);
    BOOST_TEST(border.lines[5].boundary_id == river_boundary_id);
    //7
    BOOST_TEST(border.lines[6].p1 == 6);
    BOOST_TEST(border.lines[6].p2 == 7);
    BOOST_TEST(border.lines[6].boundary_id == river_boundary_id);
    //8
    BOOST_TEST(border.lines[7].p1 == 7);
    BOOST_TEST(border.lines[7].p2 == 8);
    BOOST_TEST(border.lines[7].boundary_id == river_boundary_id);
    //9
    BOOST_TEST(border.lines[8].p1 == 8);
    BOOST_TEST(border.lines[8].p2 == 9);
    BOOST_TEST(border.lines[8].boundary_id == river_boundary_id);
    //10
    BOOST_TEST(border.lines[9].p1 == 9);
    BOOST_TEST(border.lines[9].p2 == 10);
    BOOST_TEST(border.lines[9].boundary_id == river_boundary_id);
    //11
    BOOST_TEST(border.lines[10].p1 == 10);
    BOOST_TEST(border.lines[10].p2 == 11);
    BOOST_TEST(border.lines[10].boundary_id == river_boundary_id);
    //12
    BOOST_TEST(border.lines[11].p1 == 11);
    BOOST_TEST(border.lines[11].p2 == 12);
    BOOST_TEST(border.lines[11].boundary_id == river_boundary_id);
    //13
    BOOST_TEST(border.lines[12].p1 == 12);
    BOOST_TEST(border.lines[12].p2 == 13);
    BOOST_TEST(border.lines[12].boundary_id == river_boundary_id);
    //14
    BOOST_TEST(border.lines[13].p1 == 13);
    BOOST_TEST(border.lines[13].p2 == 14);
    BOOST_TEST(border.lines[13].boundary_id == river_boundary_id);
    //15
    BOOST_TEST(border.lines[14].p1 == 14);
    BOOST_TEST(border.lines[14].p2 == 15);
    BOOST_TEST(border.lines[14].boundary_id == river_boundary_id);
    //16
    BOOST_TEST(border.lines[15].p1 == 15);
    BOOST_TEST(border.lines[15].p2 == 0);
    BOOST_TEST(border.lines[15].boundary_id == river_boundary_id);
}


BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new_new, 
    *utf::tolerance(eps))
{   
    auto river_width = 0.02;
    auto river_boundary_id = 4;
    Region region;
    region[1] = 
    {
        {
            {1, 0},
            {1, 1},
            {0, 1},
            {0, 0},
            {0.5, 0},
            {0.8, 0}
        },
        {
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, (t_boundary_id)river_boundary_id},
            {4, 5, (t_boundary_id)river_boundary_id},
            {5, 0, (t_boundary_id)river_boundary_id}
        }
    };
    
    Sources sources;
    sources[1] = {1, 4};
    sources[2] = {1, 5};

    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    rivers.AddPolars(sources.GetSourcesIds(), {{1, 0}, {2, 0}}, {(t_boundary_id)river_boundary_id, (t_boundary_id)river_boundary_id});
    rivers.AddPolars(sources.GetSourcesIds(), {{1, 0}, {2, 0}}, {(t_boundary_id)river_boundary_id, (t_boundary_id)river_boundary_id});
    
    auto boundary = BoundaryGenerator(sources, region, rivers, river_width);

    BOOST_TEST(boundary.lines.size() == 14);
    BOOST_TEST(boundary.vertices.size() == 14);

    BOOST_TEST(boundary.vertices[4].x == 0.49);
    BOOST_TEST(boundary.vertices[4].y == 0.);
    BOOST_TEST(boundary.vertices[5].x == 0.49);
    BOOST_TEST(boundary.vertices[5].y == 1.);
    BOOST_TEST(boundary.vertices[6].x == 0.5);
    BOOST_TEST(boundary.vertices[6].y == 2.);
    BOOST_TEST(boundary.vertices[7].x == 0.51);
    BOOST_TEST(boundary.vertices[7].y == 1.);
    BOOST_TEST(boundary.vertices[8].x == 0.51);
    BOOST_TEST(boundary.vertices[8].y == 0.);

    BOOST_TEST(boundary.vertices[5 + 4].x == 0.79);
    BOOST_TEST(boundary.vertices[5 + 4].y == 0.);
    BOOST_TEST(boundary.vertices[5 + 5].x == 0.79);
    BOOST_TEST(boundary.vertices[5 + 5].y == 2.);
    BOOST_TEST(boundary.vertices[5 + 6].x == 0.8);
    BOOST_TEST(boundary.vertices[5 + 6].y == 4.);
    BOOST_TEST(boundary.vertices[5 + 7].x == 0.81);
    BOOST_TEST(boundary.vertices[5 + 7].y == 2.);
    BOOST_TEST(boundary.vertices[5 + 8].x == 0.81);
    BOOST_TEST(boundary.vertices[5 + 8].y == 0.);


    BOOST_TEST(boundary.lines[4].p1 == 4);
    BOOST_TEST(boundary.lines[4].p2 == 5);
    BOOST_TEST(boundary.lines[4].boundary_id == river_boundary_id);
    //6
    BOOST_TEST(boundary.lines[5].p1 == 5);
    BOOST_TEST(boundary.lines[5].p2 == 6);
    BOOST_TEST(boundary.lines[5].boundary_id == river_boundary_id);
    //7
    BOOST_TEST(boundary.lines[6].p1 == 6);
    BOOST_TEST(boundary.lines[6].p2 == 7);
    BOOST_TEST(boundary.lines[6].boundary_id == river_boundary_id);
    //8
    BOOST_TEST(boundary.lines[7].p1 == 7);
    BOOST_TEST(boundary.lines[7].p2 == 8);
    BOOST_TEST(boundary.lines[7].boundary_id == river_boundary_id);
    //9
    BOOST_TEST(boundary.lines[8].p1 == 8);
    BOOST_TEST(boundary.lines[8].p2 == 9);
    BOOST_TEST(boundary.lines[8].boundary_id == river_boundary_id);
    //10
    BOOST_TEST(boundary.lines[9].p1 == 9);
    BOOST_TEST(boundary.lines[9].p2 == 10);
    BOOST_TEST(boundary.lines[9].boundary_id == river_boundary_id);
    //11
    BOOST_TEST(boundary.lines[10].p1 == 10);
    BOOST_TEST(boundary.lines[10].p2 == 11);
    BOOST_TEST(boundary.lines[10].boundary_id == river_boundary_id);
    //12
    BOOST_TEST(boundary.lines[11].p1 == 11);
    BOOST_TEST(boundary.lines[11].p2 == 12);
    BOOST_TEST(boundary.lines[11].boundary_id == river_boundary_id);
    //13
    BOOST_TEST(boundary.lines[12].p1 == 12);
    BOOST_TEST(boundary.lines[12].p2 == 13);
    BOOST_TEST(boundary.lines[12].boundary_id == river_boundary_id);
    //13
    BOOST_TEST(boundary.lines[13].p1 == 13);
    BOOST_TEST(boundary.lines[13].p2 == 0);
    BOOST_TEST(boundary.lines[13].boundary_id == river_boundary_id);
}


BOOST_AUTO_TEST_CASE( full_test_of_boundary_generator_most_complicated, 
    *utf::tolerance(eps))
{
    //initial condition setup
    auto river_width = 0.02;
    auto ds = 0.01;
    auto dx = 0.25;
    auto w = 1.;
    auto h = 1.;
    t_boundary_id r = 100;

    //every vertex is a source
    Region region;
    region[1] = 
    {/*Outer Boundary*/
        {/*vertices(counterclockwise order)*/
            {0, 0},
            {dx, 0}, 
            {w, 0}, 
            {w, h}, 
            {0, h}
        }, 
        {/*lines*/
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, 4},
            {4, 0, 5} 
        }
    };

    Sources sources;
    sources[1] = {1, 0};
    sources[2] = {1, 1};
    sources[3] = {1, 2};
    sources[4] = {1, 3};
    sources[5] = {1, 4};

    region[2] =
    {/*Hole Boundary*/
        {/*vertices*/
            {0.25 * w, 0.75 * h},
            {0.75 * w, 0.75 * h}, 
            {0.75 * w, 0.25 * h}, 
            {0.25 * w, 0.25 * h}
        }, 
        {/*lines*/
            {0, 1, 6},
            {1, 2, 7},
            {2, 3, 8},
            {3, 0, 9} 
        }
    };

    sources[6] = {2, 0};
    sources[7] = {2, 1};
    sources[8] = {2, 2};
    sources[9] = {2, 3};

    region[3] =
    {/*Hole Boundary*/
        {/*vertices*/
            {0.8 * w, 0.9 * h},
            {0.9 * w, 0.9 * h}, 
            {0.9 * w, 0.8 * h}, 
            {0.8 * w, 0.8 * h}
        }, 
        {/*lines*/
            {0, 1, 10},
            {1, 2, 11},
            {2, 3, 12},
            {3, 0, 13} 
        }
    };
    sources[10] = {3, 0};
    sources[11] = {3, 1};
    sources[12] = {3, 2};
    sources[13] = {3, 3};

    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    
    //grow each source point by one step
    Polar p{ds, 0};
    rivers.AddPolars(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
        {p, p, p, p, p, p, p, p, p, p,  p,  p,  p},
        {r, r, r, r, r, r, r, r, r, r,  r,  r,  r}); 

    //expected values
    auto vertices = t_PointList{
        //outer boundary
        //0
        {0 - river_width/sqrt(2)/2, 0 + river_width/sqrt(2)/2},
        {0 +  ds/sqrt(2),   0 +  ds/sqrt(2)},
        {0 + river_width/sqrt(2)/2, 0 - river_width/sqrt(2)/2},
        //1
        {dx - river_width/2, 0},
        {dx,        ds},
        {dx + river_width/2, 0},
        //2
        {w - river_width/sqrt(2)/2, - river_width/sqrt(2)/2},
        {w  - ds/sqrt(2),      ds/sqrt(2)},
        {w + river_width/sqrt(2)/2, + river_width/sqrt(2)/2},
        //3
        {w + river_width/sqrt(2)/2, h - river_width/sqrt(2)/2},
        {w -  ds/sqrt(2),   h -  ds/sqrt(2)},
        {w - river_width/sqrt(2)/2, h + river_width/sqrt(2)/2},
        //4
        {+ river_width/sqrt(2)/2, h + river_width/sqrt(2)/2},
        {+  ds/sqrt(2),   h -  ds/sqrt(2)},
        {- river_width/sqrt(2)/2, h - river_width/sqrt(2)/2},
        
        //first inner boundary
        //5
        {0.25*w - river_width/sqrt(2)/2, 0.75*h - river_width/sqrt(2)/2},
        {0.25*w -  ds/sqrt(2),   0.75*h +  ds/sqrt(2)},
        {0.25*w + river_width/sqrt(2)/2, 0.75*h + river_width/sqrt(2)/2},
        //6
        {0.75*w - river_width/sqrt(2)/2, 0.75*h + river_width/sqrt(2)/2},
        {0.75*w  + ds/sqrt(2),   0.75*h  + ds/sqrt(2)},
        {0.75*w + river_width/sqrt(2)/2, 0.75*h - river_width/sqrt(2)/2},
        //7
        {0.75*w + river_width/sqrt(2)/2, 0.25*h + river_width/sqrt(2)/2},
        {0.75*w +  ds/sqrt(2),   0.25*h -  ds/sqrt(2)},
        {0.75*w - river_width/sqrt(2)/2, 0.25*h - river_width/sqrt(2)/2},
        //8
        {0.25*w + river_width/sqrt(2)/2, 0.25*h - river_width/sqrt(2)/2},
        {0.25*w -  ds/sqrt(2),   0.25*h -  ds/sqrt(2)},
        {0.25*w - river_width/sqrt(2)/2, 0.25*h + river_width/sqrt(2)/2},

        //second inner boundary
        //19
        {0.8*w - river_width/sqrt(2)/2, 0.9*h - river_width/sqrt(2)/2},
        {0.8*w -  ds/sqrt(2),   0.9*h +  ds/sqrt(2)},
        {0.8*w + river_width/sqrt(2)/2, 0.9*h + river_width/sqrt(2)/2},
        //10
        {0.9*w - river_width/sqrt(2)/2, 0.9*h + river_width/sqrt(2)/2},
        {0.9*w  + ds/sqrt(2),   0.9*h + ds/sqrt(2)},
        {0.9*w + river_width/sqrt(2)/2, 0.9*h - river_width/sqrt(2)/2},
        //11
        {0.9*w + river_width/sqrt(2)/2, 0.8*h + river_width/sqrt(2)/2},
        {0.9*w +  ds/sqrt(2),   0.8*h -  ds/sqrt(2)},
        {0.9*w - river_width/sqrt(2)/2, 0.8*h - river_width/sqrt(2)/2},
        //12
        {0.8*w + river_width/sqrt(2)/2, 0.8*h - river_width/sqrt(2)/2},
        {0.8*w -  ds/sqrt(2),   0.8*h -  ds/sqrt(2)},
        {0.8*w - river_width/sqrt(2)/2, 0.8*h + river_width/sqrt(2)/2},
    };

    auto lines = t_LineList{
        {0,   1,  r},
        {1,   2,  r},
        {2,   3,  1},
        {3,   4,  r},
        {4,   5,  r},
        {5,   6,  2},
        {6,   7,  r},
        {7,   8,  r},
        {8,   9,  3},
        {9,  10,  r},
        {10, 11,  r},
        {11, 12,  4},
        {12, 13,  r},
        {13, 14,  r},
        {14,  0,  5},

        {15,  16,  r},
        {16,  17,  r},
        {17,  18,  6},
        {18,  19,  r},
        {19,  20,  r},
        {20,  21,  7},
        {21,  22,  r},
        {22,  23,  r},
        {23,  24,  8},
        {24,  25,  r},
        {25,  26,  r},
        {26,  15,  9},

        {27,  28,  r},
        {28,  29,  r},
        {29,  30,  10},
        {30,  31,  r},
        {31,  32,  r},
        {32,  33,  11},
        {33,  34,  r},
        {34,  35,  r},
        {35,  36,  12},
        {36,  37,  r},
        {37,  38,  r},
        {38,  27,  13},
        
    };
    auto holes = t_PointList
    {
        { 0.5*w,  0.5*w},
        {0.85*w, 0.85*w}
    };

    region.holes = holes;

    //generatre boundary
    auto boundary = BoundaryGenerator(sources, region, rivers, river_width);
    BOOST_TEST(boundary.vertices.size() == vertices.size());
    BOOST_TEST(boundary.lines.size() == lines.size());
    for(size_t i = 0; i < boundary.vertices.size(); ++i)
    {
        BOOST_TEST_MESSAGE(to_string(i));
        BOOST_TEST(boundary.vertices.at(i) == vertices.at(i));
        BOOST_TEST(   boundary.lines.at(i) == lines.at(i));
    }      

    BOOST_TEST(region.holes == holes);
}


BOOST_AUTO_TEST_CASE( tree_vertices__, 
    *utf::tolerance(eps))
{   
    Boundary boundary;
    Rivers tr;
    BOOST_CHECK_THROW(RiversBoundary(boundary, tr, 1, 0.01), Exception);
}

BOOST_AUTO_TEST_CASE( tree_boundary__, 
    *utf::tolerance(eps))
{
    //empty tree
    Boundary boundary;
    Rivers tree;
    BOOST_CHECK_THROW(RiversBoundary(boundary, tree, 1, 0.01), Exception);

    //initiazlized tree
    Region region;
    auto sources = region.MakeRectangular(1, 1, 0.5);
    auto source_id = 1;


    tree.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    boundary = Boundary{};
    RiversBoundary(boundary, tree, source_id, 0.1);
    BOOST_TEST(boundary.lines.empty());

    //tree with two points
    t_boundary_id boundary_id = 10;
    tree.at(source_id).AddPoint(Polar{0.01, 0}, boundary_id);
    boundary = Boundary{};
    RiversBoundary(boundary, tree, source_id, 0.1);

    auto expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}};
    BOOST_TEST(boundary.lines.size() == 2);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(boundary.lines.at(i) == expected_lines.at(i));

    //tree with three points
    tree.at(source_id).AddPoint(Polar{0.01, 0}, boundary_id);
    boundary = Boundary{};
    RiversBoundary(boundary, tree, source_id, 0.1);

    expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}, {2, 3, 10}, {3, 4, 10}};
    BOOST_TEST(boundary.lines.size() == 4);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(boundary.lines.at(i) == expected_lines.at(i));
}


BOOST_AUTO_TEST_CASE( Simple_Boundary_Generato_r, 
    *utf::tolerance(eps))
{
    auto river_width = 0.01;
    t_boundary_id river_boundary_id = 100;

    //TODO setup source as separate structure!!!
    //щоб вони не були частиною границі і дерева а були окремим обєктом
    Region region;
    auto sources = region.MakeRectangularWithHole();
    auto source_id = 1;
    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));

    auto boundary = BoundaryGenerator(sources, region, rivers, river_width);

    BOOST_TEST(boundary.lines.size() == 13);

    rivers.at(source_id).AddPoint(Polar{0.01, 0}, river_boundary_id);
    boundary = BoundaryGenerator(sources, region, rivers, river_width);
    BOOST_TEST(boundary.lines.size() == 15);

    auto vertices = t_PointList
    {
        {0, 0}, {0.25-0.01/2, 0}, {0.25, 0.01}, {0.25+0.01/2, 0}, {1, 0}, {1, 1}, {0, 1},
        {0.25, 0.75}, {0.75, 0.75}, {0.75, 0.25}, {0.25, 0.25},
        {0.8, 0.9}, {0.9, 0.9}, {0.9, 0.8}, {0.8, 0.8}
    };
    
    auto holes = t_PointList{
        {0.5, 0.5}, {0.85, 0.85}
    };

    auto lines =  vector<Line>{
        {0, 1, 1}, {1, 2, river_boundary_id}, {2, 3, river_boundary_id}, {3, 4, 2}, {4, 5, 3}, {5, 6, 4}, {6, 0, 5},
        {7, 8, 6}, {8, 9, 7}, {9, 10, 8}, {10, 7, 9},
        {11, 12, 10}, {12, 13, 11}, {13, 14, 12}, {14, 11, 13},
    };

    BOOST_TEST(region.GetHolesList() == holes);
    BOOST_TEST(boundary.vertices.size() == 15);
    for(size_t i = 0; i < vertices.size(); ++i)
    {
        BOOST_TEST(boundary.vertices.at(i) == vertices.at(i));
        BOOST_TEST(boundary.lines.at(i) == lines.at(i));
    }
}



BOOST_AUTO_TEST_CASE( boundary_generator_new_sdf, 
    *utf::tolerance(eps))
{   
    Region region;
    auto sources = region.MakeRectangular(1, 1, 0.5);

    Rivers rivers;
    rivers.Initialize(
        region.GetSourcesIdsPointsAndAngles(sources));
    
    Boundary boundary;
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 1);
    auto p = River::Point{0.5, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);

    t_boundary_id boundary_id = 0;
    auto& br = rivers.at(1);
    br.AddPoint(River::Polar{0.1, 0}, boundary_id);
    boundary = Boundary{};
    
    RiversBoundary(boundary, rivers, 1, 1e-3);
    BOOST_TEST(boundary.vertices.size() == 3);
    p = River::Point{0.5 - 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(0) == p);
    p = River::Point{0.5, 0.1};
    BOOST_TEST(boundary.vertices.at(1) == p);
    p = River::Point{0.5 + 1e-3/2, 0};
    BOOST_TEST(boundary.vertices.at(2) == p);


    boundary = Boundary{};
    br.AddPoint(River::Polar{0.1, 0}, boundary_id);
    Branch left_branch(br.TipPoint(), br.TipAngle() + M_PI/2);
    left_branch.AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    Branch right_branch(br.TipPoint(), br.TipAngle() - M_PI/2);
    right_branch.AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(River::Polar{0.1, 0}, boundary_id).AddPoint(Polar{0.1, 0}, boundary_id);
    rivers.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<t_branch_id> {2, 3};
    BOOST_TEST(rivers.TipBranchesIds() == tip_ids);

    RiversBoundary(boundary, rivers, 1,  1e-3);
    BOOST_TEST(boundary.vertices.size() == 17);

    //left source branch
    TEST_POINT(boundary.vertices.at(0), River::Point{0.5 - 1e-3/2, 0});
    TEST_POINT(boundary.vertices.at(1), River::Point{0.5 - 1e-3/2, 0.1});

    //left left branch
    TEST_POINT(boundary.vertices.at(2), River::Point{0.5, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(3), River::Point{0.4, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(4), River::Point{0.3, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(5), River::Point{0.2, 0.2});
    //right left branch
    TEST_POINT(boundary.vertices.at(6), River::Point{0.3, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(7), River::Point{0.4, 0.2 + 1e-3/2});
    //left right branch
    TEST_POINT(boundary.vertices.at(8), River::Point{0.5, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(9), River::Point{0.6, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(10), River::Point{0.7, 0.2 + 1e-3/2});
    TEST_POINT(boundary.vertices.at(11), River::Point{0.8, 0.2});
    //right right branch
    TEST_POINT(boundary.vertices.at(12), River::Point{0.7, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(13), River::Point{0.6, 0.2 - 1e-3/2});
    TEST_POINT(boundary.vertices.at(14), River::Point{0.5, 0.2 - 1e-3/2});
    //right source branch
    TEST_POINT(boundary.vertices.at(15), River::Point{0.5 + 1e-3/2, 0.1 });
    TEST_POINT(boundary.vertices.at(16), River::Point{0.5 + 1e-3/2, 0.0 });
    TEST_POINT(boundary.vertices.back(), River::Point{0.5 + 1e-3/2, 0});
}


BOOST_AUTO_TEST_CASE( debug_python_code_test, 
    *utf::tolerance(eps))
{
    //initial condition setup
    auto river_width = 0.02;
    auto ds = 0.2;
    t_boundary_id r = 100;

    //every vertex is a source
    Region region;
    region[1] = 
    {
        {
            {0., 0.}, 
            {1., 0.}, 
            {1., 1.}, 
            {0., 1.}
        }, 
        {
            {0, 1, 1},
            {1, 2, 1},
            {2, 3, 1},
            {3, 0, 1}
        }
    };

    Sources sources;
    sources[1] = {1, 0};
    sources[2] = {1, 2};

    Rivers rivers;
    rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    
    //grow each source point by one step
    Polar p{ds, 0.};
    rivers.AddPolars(
        {1, 2},
        {p, p},
        {r, r}); 
    rivers.AddPolars(
        {1, 2},
        {p, p},
        {r, r}); 

    //expected values
    t_PointList vertices;
    Point left_shift{river_width/2., 0.}, rigth_shift{river_width/2., 0.};
    left_shift.rotate(3./4. * M_PI);
    rigth_shift.rotate(- 1./4. * M_PI);

    vertices.push_back(Point{0.,    0.} + left_shift);
    vertices.push_back(Point{ds,    0.}.rotate(M_PI / 4.) + left_shift);
    vertices.push_back(Point{2.*ds, 0.}.rotate(M_PI / 4.)); 
    vertices.push_back(Point{ds,    0.}.rotate(M_PI / 4.) + rigth_shift);
    vertices.push_back(Point{0.,    0.} + rigth_shift);
    vertices.push_back(Point{1.,    0.});
    vertices.push_back(Point{1.,    1.} + rigth_shift);
    vertices.push_back(Point{1.,    1.} + Point{ds,    0.}.rotate(5. * M_PI / 4.) + rigth_shift);
    vertices.push_back(Point{1.,    1.} + Point{2.*ds, 0.}.rotate(5. * M_PI / 4.)); 
    vertices.push_back(Point{1.,    1.} + Point{ds,    0.}.rotate(5. * M_PI / 4.) + left_shift);
    vertices.push_back(Point{1.,    1.} + left_shift);
    vertices.push_back(Point{0.,    1.});

    auto lines = t_LineList{
        {0,   1,  r},
        {1,   2,  r},
        {2,   3,  r},
        {3,   4,  r},
        {4,   5,  1},
        {5,   6,  1},
        {6,   7,  r},
        {7,   8,  r},
        {8,   9,  r},
        {9,  10,  r},
        {10, 11,  1},
        {11, 0,  1}
    };

    //generatre boundary
    auto boundary = BoundaryGenerator(sources, region, rivers, river_width);

    BOOST_TEST(boundary.vertices.at(0) == vertices.at(0));
    BOOST_TEST(   boundary.lines.at(0) ==    lines.at(0));      
    BOOST_TEST(boundary.vertices.at(1) == vertices.at(1));
    BOOST_TEST(   boundary.lines.at(1) ==    lines.at(1));      
    BOOST_TEST(boundary.vertices.at(2) == vertices.at(2));
    BOOST_TEST(   boundary.lines.at(2) ==    lines.at(2));      
    BOOST_TEST(boundary.vertices.at(3) == vertices.at(3));
    BOOST_TEST(   boundary.lines.at(3) ==    lines.at(3));      
    BOOST_TEST(boundary.vertices.at(4) == vertices.at(4));
    BOOST_TEST(   boundary.lines.at(4) ==    lines.at(4));      
    BOOST_TEST(boundary.vertices.at(5) == vertices.at(5));
    BOOST_TEST(   boundary.lines.at(5) ==    lines.at(5));      
    BOOST_TEST(boundary.vertices.at(6) == vertices.at(6));
    BOOST_TEST(   boundary.lines.at(6) ==    lines.at(6));      
    BOOST_TEST(boundary.vertices.at(7) == vertices.at(7));
    BOOST_TEST(   boundary.lines.at(7) ==    lines.at(7));      
    BOOST_TEST(boundary.vertices.at(8) == vertices.at(8));
    BOOST_TEST(   boundary.lines.at(8) ==    lines.at(8));      
    BOOST_TEST(boundary.vertices.at(9) == vertices.at(9));
    BOOST_TEST(   boundary.lines.at(9) ==    lines.at(9));      
    BOOST_TEST(boundary.vertices.at(10) == vertices.at(10));
    BOOST_TEST(   boundary.lines.at(10) ==    lines.at(10));      
    BOOST_TEST(boundary.vertices.at(11) == vertices.at(11));
    BOOST_TEST(   boundary.lines.at(11) ==    lines.at(11));      
}