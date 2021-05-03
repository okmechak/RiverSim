//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Boundary Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include "boundary_generator.hpp"
#include "tethex.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( tree_vertices, 
    *utf::tolerance(eps))
{   
    vector<Point> tree_vertices;
    Tree tr;
    BOOST_CHECK_THROW(TreeVertices(tree_vertices, 1, tr, 0.01), Exception);
}

BOOST_AUTO_TEST_CASE( tree_boundary, 
    *utf::tolerance(eps))
{
    //empty tree
    Tree tree;
    BOOST_CHECK_THROW(TreeBoundary(tree, 1, 0, 0.01), Exception);

    //initiazlized tree
    Boundaries boundaries;
    auto sources = boundaries.MakeRectangular(1, 1, 0.5);
    auto source_id = 1;


    tree.Initialize(boundaries.GetSourcesIdsPointsAndAngles(sources));
    auto tree_boundary = TreeBoundary(tree, source_id, 10, 0.1);
    BOOST_TEST(tree_boundary.lines.empty());

    //tree with two points
    tree.at(source_id).AddPoint(Polar{0.01, 0});
    tree_boundary = TreeBoundary(tree, source_id, 10, 0.1);

    auto expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}};
    BOOST_TEST(tree_boundary.lines.size() == 2);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(tree_boundary.lines.at(i) == expected_lines.at(i));

    //tree with three points
    tree.at(source_id).AddPoint(Polar{0.01, 0});
    tree_boundary = TreeBoundary(tree, source_id, 10, 0.1);

    expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}, {2, 3, 10}, {3, 4, 10}};
    BOOST_TEST(tree_boundary.lines.size() == 4);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(tree_boundary.lines.at(i) == expected_lines.at(i));
}

BOOST_AUTO_TEST_CASE( Simple_Boundary_Generator, 
    *utf::tolerance(eps))
{
    Model model;
    model.mesh.eps = 0.01;
    model.river_boundary_id = 100;

    //TODO setup source as separate structure!!!
    //щоб вони не були частиною границі і дерева а були окремим обєктом
    model.sources = model.border.MakeRectangularWithHole();
    auto source_id = 1;
    model.tree.Initialize(model.border.GetSourcesIdsPointsAndAngles(model.sources));

    auto boundary = SimpleBoundaryGenerator(model);

    BOOST_TEST(boundary.lines.size() == 13);

    model.tree.at(source_id).AddPoint(Polar{0.01, 0});
    boundary = SimpleBoundaryGenerator(model);
    BOOST_TEST(boundary.lines.size() == 15);

    auto vertices =  vector<Point>
    {
        {0, 0}, {0.25-0.01/2, 0}, {0.25, 0.01}, {0.25+0.01/2, 0}, {1, 0}, {1, 1}, {0, 1},
        {0.25, 0.75}, {0.75, 0.75}, {0.75, 0.25}, {0.25, 0.25},
        {0.8, 0.9}, {0.9, 0.9}, {0.9, 0.8}, {0.8, 0.8}
    };
    
    auto holes = vector<Point>{
        {0.5, 0.5}, {0.85, 0.85}
    };

    auto lines =  vector<Line>{
        {0, 1, 1}, {1, 2, model.river_boundary_id}, {2, 3, model.river_boundary_id}, {3, 4, 2}, {4, 5, 3}, {5, 6, 4}, {6, 0, 5},
        {7, 8, 6}, {8, 9, 7}, {9, 10, 8}, {10, 7, 9},
        {11, 12, 10}, {12, 13, 11}, {13, 14, 12}, {14, 11, 13},
    };

    BOOST_TEST(model.border.GetHolesList() == holes);
    BOOST_TEST(boundary.vertices.size() == 15);
    for(size_t i = 0; i < vertices.size(); ++i)
    {
        BOOST_TEST(boundary.vertices.at(i) == vertices.at(i));
        BOOST_TEST(boundary.lines.at(i) == lines.at(i));
    }
}

inline void TEST_POINT(Point p1, Point p2)
{
    BOOST_TEST(p1 == p2);
}

BOOST_AUTO_TEST_CASE( boundary_generator_new, 
    *utf::tolerance(eps))
{   
    Boundaries boundary;
    auto sources = boundary.MakeRectangular(1, 1, 0.5);

    Tree tr;
    tr.Initialize(
        boundary.GetSourcesIdsPointsAndAngles(sources));
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    auto& br = tr.at(1);
    br.AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    p = Point{0.5 - 1e-3/2, 0};
    BOOST_TEST(tree_vector.at(0) == p);
    p = Point{0.5, 0.1};
    BOOST_TEST(tree_vector.at(1) == p);
    p = Point{0.5 + 1e-3/2, 0};
    BOOST_TEST(tree_vector.at(2) == p);


    tree_vector.clear();
    br.AddPoint(Polar{0.1, 0});
    Branch left_branch(br.TipPoint(), br.TipAngle() + M_PI/2);
    left_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    Branch right_branch(br.TipPoint(), br.TipAngle() - M_PI/2);
    right_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    tr.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<t_branch_id> {2, 3};
    BOOST_TEST(tr.TipBranchesIds() == tip_ids);

    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 17);

    //left source branch
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2, 0});
    TEST_POINT(tree_vector.at(1), Point{0.5 - 1e-3/2, 0.1});

    //left left branch
    TEST_POINT(tree_vector.at(2), Point{0.5, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(3), Point{0.4, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(4), Point{0.3, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(5), Point{0.2, 0.2});
    //right left branch
    TEST_POINT(tree_vector.at(6), Point{0.3, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(7), Point{0.4, 0.2 + 1e-3/2});
    //left right branch
    TEST_POINT(tree_vector.at(8), Point{0.5, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(9), Point{0.6, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(10), Point{0.7, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(11), Point{0.8, 0.2});
    //right right branch
    TEST_POINT(tree_vector.at(12), Point{0.7, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(13), Point{0.6, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(14), Point{0.5, 0.2 - 1e-3/2});
    //right source branch
    TEST_POINT(tree_vector.at(15), Point{0.5 + 1e-3/2, 0.1 });
    TEST_POINT(tree_vector.at(16), Point{0.5 + 1e-3/2, 0.0 });
    TEST_POINT(tree_vector.back(), Point{0.5 + 1e-3/2, 0});

}

BOOST_AUTO_TEST_CASE( boundary_generator_new_2, 
    *utf::tolerance(eps))
{   
    Boundaries boundary;
    auto sources = boundary.MakeRectangular(1,1,0.5);

    Tree tr;
    tr.Initialize(
        boundary.GetSourcesIdsPointsAndAngles(sources));
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);
    

    Branch l(tr.at(1).TipPoint(), tr.at(1).TipAngle()+M_PI/4),
        r(tr.at(1).TipPoint(), tr.at(1).TipAngle() - M_PI/4);
    
    auto[i1, i2] = tr.AddSubBranches(1, l, r);
    cout << i2;
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    tr.at(i1).AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(tree_vector.at(1), Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(tree_vector.at(2), Point{0.5, 0});    
}

BOOST_AUTO_TEST_CASE( boundary_generator_new_new, 
    *utf::tolerance(eps))
{   
    Boundaries border;
    auto sources = border.MakeRectangular(1, 1, 0.5);

    Tree tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    auto& br = tr.at(1);
    br.AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    p = Point{0.5 - 1e-3/2, 0};
    BOOST_TEST(tree_vector.at(0) == p);
    p = Point{0.5, 0.1};
    BOOST_TEST(tree_vector.at(1) == p);
    p = Point{0.5 + 1e-3/2, 0};
    BOOST_TEST(tree_vector.at(2) == p);


    tree_vector.clear();
    br.AddPoint(Polar{0.1, 0});
    Branch left_branch(br.TipPoint(), br.TipAngle() + M_PI/2);
    left_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    Branch right_branch(br.TipPoint(), br.TipAngle() - M_PI/2);
    right_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    tr.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<t_branch_id> {2, 3};
    BOOST_TEST(tr.TipBranchesIds() == tip_ids);

    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 17);

    //left source branch
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2, 0});
    TEST_POINT(tree_vector.at(1), Point{0.5 - 1e-3/2, 0.1});

    //left left branch
    TEST_POINT(tree_vector.at(2), Point{0.5, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(3), Point{0.4, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(4), Point{0.3, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(5), Point{0.2, 0.2});
    //right left branch
    TEST_POINT(tree_vector.at(6), Point{0.3, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(7), Point{0.4, 0.2 + 1e-3/2});
    //left right branch
    TEST_POINT(tree_vector.at(8), Point{0.5, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(9), Point{0.6, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(10), Point{0.7, 0.2 + 1e-3/2});
    TEST_POINT(tree_vector.at(11), Point{0.8, 0.2});
    //right right branch
    TEST_POINT(tree_vector.at(12), Point{0.7, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(13), Point{0.6, 0.2 - 1e-3/2});
    TEST_POINT(tree_vector.at(14), Point{0.5, 0.2 - 1e-3/2});
    //right source branch
    TEST_POINT(tree_vector.at(15), Point{0.5 + 1e-3/2, 0.1 });
    TEST_POINT(tree_vector.at(16), Point{0.5 + 1e-3/2, 0.0 });
    TEST_POINT(tree_vector.back(), Point{0.5 + 1e-3/2, 0});

}

BOOST_AUTO_TEST_CASE( boundary_generator_new_2_lala, 
    *utf::tolerance(eps))
{   
    Boundaries border;
    auto sources = border.MakeRectangular(1,1, 0.5);

    Tree tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);


    Branch l(tr.at(1).TipPoint(), tr.at(1).TipAngle()+M_PI/4),
        r(tr.at(1).TipPoint(), tr.at(1).TipAngle() - M_PI/4);
    
    auto[i1, i2] = tr.AddSubBranches(1, l, r);
    cout << i2;
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    tr.at(i1).AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(tree_vector.at(1), Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(tree_vector.at(2), Point{0.5, 0});    
}

BOOST_AUTO_TEST_CASE( BoundaryGenerator_test, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.01;
    mdl.river_boundary_id = 4;
    mdl.border[1] = 
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
        },
        false,
        {},
        "test"
    };
    mdl.sources[1] = {1, 4};
    mdl.sources[2] = {1, 5};
    mdl.sources[3] = {1, 6};
    mdl.sources[4] = {1, 7};
    
    mdl.tree.Initialize(mdl.border.GetSourcesIdsPointsAndAngles(mdl.sources));
    
    BOOST_TEST_MESSAGE( "Boundary Generator start" );
    auto border = SimpleBoundaryGenerator(mdl);
    tethex::Mesh mesh(border);
    BOOST_TEST_MESSAGE( "Boundary Generator done" );


    BOOST_TEST(mesh.get_n_vertices() == 8);
    //Vertices test
    //1
    BOOST_TEST(mesh.get_vertex(0).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(0).get_coord(1) == 0.);
    //2
    BOOST_TEST(mesh.get_vertex(1).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(1).get_coord(1) == 1.);
    //3
    BOOST_TEST(mesh.get_vertex(2).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(2).get_coord(1) == 1.);
    //4
    BOOST_TEST(mesh.get_vertex(3).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(3).get_coord(1) == 0.);
    //5
    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    //6
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.6);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 0.);
    //7
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.7);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 0.);
    //8
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 0.);

    //line lements
    BOOST_TEST(mesh.get_n_lines() == 8);
    //1
    BOOST_TEST(mesh.get_line(0).get_vertex(0) == 0);
    BOOST_TEST(mesh.get_line(0).get_vertex(1) == 1);
    //2
    BOOST_TEST(mesh.get_line(1).get_vertex(0) == 1);
    BOOST_TEST(mesh.get_line(1).get_vertex(1) == 2);
    //3
    BOOST_TEST(mesh.get_line(2).get_vertex(0) == 2);
    BOOST_TEST(mesh.get_line(2).get_vertex(1) == 3);
    //4
    BOOST_TEST(mesh.get_line(3).get_vertex(0) == 3);
    BOOST_TEST(mesh.get_line(3).get_vertex(1) == 4);
    //5
    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 0);
}



BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.02;
    mdl.river_boundary_id = 5;
    
    mdl.border[1] = 
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
            {3, 4, mdl.river_boundary_id},
            {4, 5, mdl.river_boundary_id},
            {5, 6, mdl.river_boundary_id},
            {6, 7, mdl.river_boundary_id},
            {7, 0, mdl.river_boundary_id},
        },
        false,
        {},
        "test"
    };
    
    mdl.sources[1] = {1, 4};
    mdl.sources[2] = {1, 5};
    mdl.sources[3] = {1, 6};
    mdl.sources[4] = {1, 7};

    mdl.tree.Initialize(mdl.border.GetSourcesIdsPointsAndAngles(mdl.sources));
    mdl.tree.AddPolars(mdl.sources.GetSourcesIds(), {{1, 0}, {2, 0}, {3, 0}, {4, 0}});
    
    auto border = SimpleBoundaryGenerator(mdl);
    tethex::Mesh mesh(border);

    BOOST_TEST(mesh.get_n_vertices() == 16);
    //Vertices test
    //1
    BOOST_TEST(mesh.get_vertex(0).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(0).get_coord(1) == 0.);
    //2
    BOOST_TEST(mesh.get_vertex(1).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(1).get_coord(1) == 1.);
    //3
    BOOST_TEST(mesh.get_vertex(2).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(2).get_coord(1) == 1.);
    //4
    BOOST_TEST(mesh.get_vertex(3).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(3).get_coord(1) == 0.);
    //5
    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 0.);
    //7
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.59);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(8).get_coord(0) == 0.6);
    BOOST_TEST(mesh.get_vertex(8).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(9).get_coord(0) == 0.61);
    BOOST_TEST(mesh.get_vertex(9).get_coord(1) == 0.);
    //8
    BOOST_TEST(mesh.get_vertex(10).get_coord(0) == 0.69);
    BOOST_TEST(mesh.get_vertex(10).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(11).get_coord(0) == 0.7);
    BOOST_TEST(mesh.get_vertex(11).get_coord(1) == 3.);
    BOOST_TEST(mesh.get_vertex(12).get_coord(0) == 0.71);
    BOOST_TEST(mesh.get_vertex(12).get_coord(1) == 0.);
    //9
    BOOST_TEST(mesh.get_vertex(13).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(13).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(14).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(14).get_coord(1) == 4.);
    BOOST_TEST(mesh.get_vertex(15).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(15).get_coord(1) == 0.);

    //line lements
    BOOST_TEST(mesh.get_n_lines() == 16);
    //1
    BOOST_TEST(mesh.get_line(0).get_vertex(0) == 0);
    BOOST_TEST(mesh.get_line(0).get_vertex(1) == 1);
    BOOST_TEST(mesh.get_line(0).get_material_id() == 1);
    //2
    BOOST_TEST(mesh.get_line(1).get_vertex(0) == 1);
    BOOST_TEST(mesh.get_line(1).get_vertex(1) == 2);
    BOOST_TEST(mesh.get_line(1).get_material_id() == 2);
    //3
    BOOST_TEST(mesh.get_line(2).get_vertex(0) == 2);
    BOOST_TEST(mesh.get_line(2).get_vertex(1) == 3);
    BOOST_TEST(mesh.get_line(2).get_material_id() == 3);
    //4
    BOOST_TEST(mesh.get_line(3).get_vertex(0) == 3);
    BOOST_TEST(mesh.get_line(3).get_vertex(1) == 4);
    BOOST_TEST(mesh.get_line(3).get_material_id() == mdl.river_boundary_id);
    //5
    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    BOOST_TEST(mesh.get_line(4).get_material_id() == mdl.river_boundary_id);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    BOOST_TEST(mesh.get_line(5).get_material_id() == mdl.river_boundary_id);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    BOOST_TEST(mesh.get_line(6).get_material_id() == mdl.river_boundary_id);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 8);
    BOOST_TEST(mesh.get_line(7).get_material_id() == mdl.river_boundary_id);
    //9
    BOOST_TEST(mesh.get_line(8).get_vertex(0) == 8);
    BOOST_TEST(mesh.get_line(8).get_vertex(1) == 9);
    BOOST_TEST(mesh.get_line(8).get_material_id() == mdl.river_boundary_id);
    //10
    BOOST_TEST(mesh.get_line(9).get_vertex(0) == 9);
    BOOST_TEST(mesh.get_line(9).get_vertex(1) == 10);
    BOOST_TEST(mesh.get_line(9).get_material_id() == mdl.river_boundary_id);
    //11
    BOOST_TEST(mesh.get_line(10).get_vertex(0) == 10);
    BOOST_TEST(mesh.get_line(10).get_vertex(1) == 11);
    BOOST_TEST(mesh.get_line(10).get_material_id() == mdl.river_boundary_id);
    //12
    BOOST_TEST(mesh.get_line(11).get_vertex(0) == 11);
    BOOST_TEST(mesh.get_line(11).get_vertex(1) == 12);
    BOOST_TEST(mesh.get_line(11).get_material_id() == mdl.river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(12).get_vertex(0) == 12);
    BOOST_TEST(mesh.get_line(12).get_vertex(1) == 13);
    BOOST_TEST(mesh.get_line(12).get_material_id() == mdl.river_boundary_id);
    //14
    BOOST_TEST(mesh.get_line(13).get_vertex(0) == 13);
    BOOST_TEST(mesh.get_line(13).get_vertex(1) == 14);
    BOOST_TEST(mesh.get_line(13).get_material_id() == mdl.river_boundary_id);
    //15
    BOOST_TEST(mesh.get_line(14).get_vertex(0) == 14);
    BOOST_TEST(mesh.get_line(14).get_vertex(1) == 15);
    BOOST_TEST(mesh.get_line(14).get_material_id() == mdl.river_boundary_id);
    //16
    BOOST_TEST(mesh.get_line(15).get_vertex(0) == 15);
    BOOST_TEST(mesh.get_line(15).get_vertex(1) == 0);
    BOOST_TEST(mesh.get_line(15).get_material_id() == mdl.river_boundary_id);
}

BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new_new, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.02;
    mdl.river_boundary_id = 4;
    mdl.border[1] = 
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
            {3, 4, mdl.river_boundary_id},
            {4, 5, mdl.river_boundary_id},
            {5, 0, mdl.river_boundary_id}
        },
        false,
        {},
        "test"
    };
    
    mdl.sources[1] = {1, 4};
    mdl.sources[2] = {1, 5};

    mdl.tree.Initialize(mdl.border.GetSourcesIdsPointsAndAngles(mdl.sources));
    mdl.tree.AddPolars(mdl.sources.GetSourcesIds(), {{1, 0}, {2, 0}});
    mdl.tree.AddPolars(mdl.sources.GetSourcesIds(), {{1, 0}, {2, 0}});
    
    auto boundary = SimpleBoundaryGenerator(mdl);
    tethex::Mesh mesh(boundary);

    BOOST_TEST(mesh.get_n_lines() == 14);
    BOOST_TEST(mesh.get_n_vertices() == 14);

    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(8).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(8).get_coord(1) == 0.);

    BOOST_TEST(mesh.get_vertex(5 + 4).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(5 + 4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5 + 5).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(5 + 5).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(5 + 6).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(5 + 6).get_coord(1) == 4.);
    BOOST_TEST(mesh.get_vertex(5 + 7).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(5 + 7).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(5 + 8).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(5 + 8).get_coord(1) == 0.);


    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    BOOST_TEST(mesh.get_line(4).get_material_id() == mdl.river_boundary_id);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    BOOST_TEST(mesh.get_line(5).get_material_id() == mdl.river_boundary_id);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    BOOST_TEST(mesh.get_line(6).get_material_id() == mdl.river_boundary_id);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 8);
    BOOST_TEST(mesh.get_line(7).get_material_id() == mdl.river_boundary_id);
    //9
    BOOST_TEST(mesh.get_line(8).get_vertex(0) == 8);
    BOOST_TEST(mesh.get_line(8).get_vertex(1) == 9);
    BOOST_TEST(mesh.get_line(8).get_material_id() == mdl.river_boundary_id);
    //10
    BOOST_TEST(mesh.get_line(9).get_vertex(0) == 9);
    BOOST_TEST(mesh.get_line(9).get_vertex(1) == 10);
    BOOST_TEST(mesh.get_line(9).get_material_id() == mdl.river_boundary_id);
    //11
    BOOST_TEST(mesh.get_line(10).get_vertex(0) == 10);
    BOOST_TEST(mesh.get_line(10).get_vertex(1) == 11);
    BOOST_TEST(mesh.get_line(10).get_material_id() == mdl.river_boundary_id);
    //12
    BOOST_TEST(mesh.get_line(11).get_vertex(0) == 11);
    BOOST_TEST(mesh.get_line(11).get_vertex(1) == 12);
    BOOST_TEST(mesh.get_line(11).get_material_id() == mdl.river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(12).get_vertex(0) == 12);
    BOOST_TEST(mesh.get_line(12).get_vertex(1) == 13);
    BOOST_TEST(mesh.get_line(12).get_material_id() == mdl.river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(13).get_vertex(0) == 13);
    BOOST_TEST(mesh.get_line(13).get_vertex(1) == 0);
    BOOST_TEST(mesh.get_line(13).get_material_id() == mdl.river_boundary_id);
}


BOOST_AUTO_TEST_CASE( full_test_of_boundary_generator_most_complicated, 
    *utf::tolerance(eps))
{
    //initial condition setup
    Model model;
    model.mesh.eps = 0.02;
    auto& eps = model.mesh.eps;
    model.ds = 0.01;
    auto& ds = model.ds;
    auto& dx = model.dx;
    auto& w = model.width;
    auto& h = model.height;
    auto& r = model.river_boundary_id;

    //every vertex is a source
    model.border[1] = 
    {/*Outer Boundary*/
        {/*vertices(counterclockwise order)*/
            {0, 0},
            {model.dx, 0}, 
            {model.width, 0}, 
            {model.width, model.height}, 
            {0, model.height}
        }, 
        {/*lines*/
            {0, 1, 1},
            {1, 2, 2},
            {2, 3, 3},
            {3, 4, 4},
            {4, 0, 5} 
        }, 
        false/*this is not inner boudary*/,
        {}/*hole*/,
        "outer boudary"/*just name*/
    };

    
    model.sources[1] = {1, 0};
    model.sources[2] = {1, 1};
    model.sources[3] = {1, 2};
    model.sources[4] = {1, 3};
    model.sources[5] = {1, 4};

    model.border[2] =
    {/*Hole Boundary*/
        {/*vertices*/
            {0.25 * model.width, 0.75 * model.height},
            {0.75 * model.width, 0.75 * model.height}, 
            {0.75 * model.width, 0.25 * model.height}, 
            {0.25 * model.width, 0.25 * model.height}
        }, 
        {/*lines*/
            {0, 1, 6},
            {1, 2, 7},
            {2, 3, 8},
            {3, 0, 9} 
        }, 
        true/*this is inner boudary*/,
        {{0.5*model.width, 0.5*model.height}}/*holes*/,
        "hole"/*just name*/
    };

    model.sources[6] = {2, 0};
    model.sources[7] = {2, 1};
    model.sources[8] = {2, 2};
    model.sources[9] = {2, 3};

    model.border[3] =
    {/*Hole Boundary*/
        {/*vertices*/
            {0.8 * model.width, 0.9 * model.height},
            {0.9 * model.width, 0.9 * model.height}, 
            {0.9 * model.width, 0.8 * model.height}, 
            {0.8 * model.width, 0.8 * model.height}
        }, 
        {/*lines*/
            {0, 1, 10},
            {1, 2, 11},
            {2, 3, 12},
            {3, 0, 13} 
        }, 
        true/*this is inner boudary*/,
        {{0.85*model.width, 0.85*model.height}}/*holes*/,
        "hole"/*just name*/
    };
    model.sources[10] = {3, 0};
    model.sources[11] = {3, 1};
    model.sources[12] = {3, 2};
    model.sources[13] = {3, 3};

    model.tree.Initialize(model.border.GetSourcesIdsPointsAndAngles(model.sources));
    
    //grow each source point by one step
    Polar p{ds, 0};
    model.tree.AddPolars(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
        {p, p, p, p, p, p, p, p, p, p,  p,  p,  p}); 

    //expected values
    auto vertices = vector<Point>{
        //outer boundary
        //0
        {0 - eps/sqrt(2)/2, 0 + eps/sqrt(2)/2},
        {0 +  ds/sqrt(2),   0 +  ds/sqrt(2)},
        {0 + eps/sqrt(2)/2, 0 - eps/sqrt(2)/2},
        //1
        {dx - eps/2, 0},
        {dx,        ds},
        {dx + eps/2, 0},
        //2
        {w - eps/sqrt(2)/2, - eps/sqrt(2)/2},
        {w  - ds/sqrt(2),      ds/sqrt(2)},
        {w + eps/sqrt(2)/2, + eps/sqrt(2)/2},
        //3
        {w + eps/sqrt(2)/2, h - eps/sqrt(2)/2},
        {w -  ds/sqrt(2),   h -  ds/sqrt(2)},
        {w - eps/sqrt(2)/2, h + eps/sqrt(2)/2},
        //4
        {+ eps/sqrt(2)/2, h + eps/sqrt(2)/2},
        {+  ds/sqrt(2),   h -  ds/sqrt(2)},
        {- eps/sqrt(2)/2, h - eps/sqrt(2)/2},
        
        //first inner boundary
        //5
        {0.25*w - eps/sqrt(2)/2, 0.75*h - eps/sqrt(2)/2},
        {0.25*w -  ds/sqrt(2),   0.75*h +  ds/sqrt(2)},
        {0.25*w + eps/sqrt(2)/2, 0.75*h + eps/sqrt(2)/2},
        //6
        {0.75*w - eps/sqrt(2)/2, 0.75*h + eps/sqrt(2)/2},
        {0.75*w  + ds/sqrt(2),   0.75*h  + ds/sqrt(2)},
        {0.75*w + eps/sqrt(2)/2, 0.75*h - eps/sqrt(2)/2},
        //7
        {0.75*w + eps/sqrt(2)/2, 0.25*h + eps/sqrt(2)/2},
        {0.75*w +  ds/sqrt(2),   0.25*h -  ds/sqrt(2)},
        {0.75*w - eps/sqrt(2)/2, 0.25*h - eps/sqrt(2)/2},
        //8
        {0.25*w + eps/sqrt(2)/2, 0.25*h - eps/sqrt(2)/2},
        {0.25*w -  ds/sqrt(2),   0.25*h -  ds/sqrt(2)},
        {0.25*w - eps/sqrt(2)/2, 0.25*h + eps/sqrt(2)/2},

        //second inner boundary
        //19
        {0.8*w - eps/sqrt(2)/2, 0.9*h - eps/sqrt(2)/2},
        {0.8*w -  ds/sqrt(2),   0.9*h +  ds/sqrt(2)},
        {0.8*w + eps/sqrt(2)/2, 0.9*h + eps/sqrt(2)/2},
        //10
        {0.9*w - eps/sqrt(2)/2, 0.9*h + eps/sqrt(2)/2},
        {0.9*w  + ds/sqrt(2),   0.9*h + ds/sqrt(2)},
        {0.9*w + eps/sqrt(2)/2, 0.9*h - eps/sqrt(2)/2},
        //11
        {0.9*w + eps/sqrt(2)/2, 0.8*h + eps/sqrt(2)/2},
        {0.9*w +  ds/sqrt(2),   0.8*h -  ds/sqrt(2)},
        {0.9*w - eps/sqrt(2)/2, 0.8*h - eps/sqrt(2)/2},
        //12
        {0.8*w + eps/sqrt(2)/2, 0.8*h - eps/sqrt(2)/2},
        {0.8*w -  ds/sqrt(2),   0.8*h -  ds/sqrt(2)},
        {0.8*w - eps/sqrt(2)/2, 0.8*h + eps/sqrt(2)/2},

    };

    auto lines = vector<Line>{
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
    auto holes = vector<Point>
    {
        { 0.5*w,  0.5*w},
        {0.85*w, 0.85*w}
    };

    //generatre boundary
    auto final_boundary = SimpleBoundaryGenerator(model);

    for(size_t i = 0; i < final_boundary.vertices.size(); ++i)
    {
        BOOST_TEST_MESSAGE(to_string(i));
        BOOST_TEST(final_boundary.vertices.at(i) == vertices.at(i));
        BOOST_TEST(   final_boundary.lines.at(i) == lines.at(i));
    }      

    BOOST_TEST(final_boundary.holes == holes);
}