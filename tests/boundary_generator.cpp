//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Boundary Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include "boundary_generator.hpp"

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

    Boundary boundary;
    boundary.MakeRectangular(1, 1, 0.5);

}

BOOST_AUTO_TEST_CASE( tree_boundary, 
    *utf::tolerance(eps))
{
    //empty tree
    Tree tree;
    BOOST_CHECK_THROW(TreeBoundary(tree, 1, 0, 0.01), Exception);


    //initiazlized tree
    Boundary boundary;
    boundary.MakeRectangular(1, 1, 0.5);


    tree.Initialize(boundary.GetSourcesIdsPointsAndAngles());
    auto tree_boundary = TreeBoundary(tree, tree.SourceBranchesID().at(0), 10, 0.1);
    BOOST_TEST(tree_boundary.lines.empty());

    //tree with two points
    tree.GetBranch(tree.SourceBranchesID().at(0))->AddPoint(Polar{0.01, 0});
    tree_boundary = TreeBoundary(tree, tree.SourceBranchesID().at(0), 10, 0.1);

    auto expected_lines = vector<Line>{{0, 1, 10}, {1, 2, 10}};
    BOOST_TEST(tree_boundary.lines.size() == 2);
    for(size_t i = 0; i < expected_lines.size(); ++i)
        BOOST_TEST(tree_boundary.lines.at(i) == expected_lines.at(i));

    //tree with three points
    tree.GetBranch(tree.SourceBranchesID().at(0))->AddPoint(Polar{0.01, 0});
    tree_boundary = TreeBoundary(tree, tree.SourceBranchesID().at(0), 10, 0.1);

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
    model.boundary.MakeRectangularWithHole();
    model.trees.Initialize(model.boundary.GetSourcesIdsPointsAndAngles());

    auto boundary = SimpleBoundaryGenerator(model);

    BOOST_TEST(boundary.lines.size() == 13);

    model.trees.GetBranch(model.trees.SourceBranchesID().at(0))->AddPoint(Polar{0.01, 0});
    boundary = SimpleBoundaryGenerator(model);
    BOOST_TEST(boundary.lines.size() == 15);

    auto vertices =  vector<Point>{
        {0, 0}, {0.25-0.01/2, 0}, {0.25, 0.01}, {0.25+0.01/2, 0}, {1, 0}, {1, 1}, {0, 1},
        {0.25, 0.25}, {0.75, 0.25}, {0.75, 0.75}, {0.25, 0.75},
        {0.8, 0.8}, {0.9, 0.8}, {0.9, 0.9}, {0.8, 0.9}};
    
    auto holes = vector<Point>{
        {0.5, 0.5}, {0.85, 0.85}
    };

    auto lines =  vector<Line>{
        {0, 1, 1}, {1, 2, 100}, {2, 3, 100}, {3, 4, 2}, {4, 5, 3}, {5, 6, 4}, {6, 0, 5},
        {7, 8, 6}, {8, 9, 7}, {9, 10, 8}, {10, 7, 9},
        {11, 12, 10}, {12, 13, 11}, {13, 14, 12}, {14, 11, 13},
    };

    BOOST_TEST(model.boundary.GetHolesList() == holes);
    for(size_t i = 0; i < vertices.size(); ++i)
    {
        BOOST_TEST_MESSAGE(i);
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
    Boundary boundary;
    boundary.MakeRectangular(1, 1, 0.5);

    Tree tr;
    tr.Initialize(
        boundary.GetSourcesIdsPointsAndAngles());
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    auto br = tr.GetBranch(1);
    br->AddPoint(Polar{0.1, 0});
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
    br->AddPoint(Polar{0.1, 0});
    BranchNew left_branch(br->TipPoint(), br->TipAngle() + M_PI/2);
    left_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    BranchNew right_branch(br->TipPoint(), br->TipAngle() - M_PI/2);
    right_branch.AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0}).AddPoint(Polar{0.1, 0});
    tr.AddSubBranches(1, left_branch, right_branch);

    auto tip_ids = vector<int> {2, 3};
    BOOST_TEST(tr.TipBranchesId() == tip_ids);

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
    Boundary boundary;
    boundary.MakeRectangular(1,1,0.5);

    Tree tr;
    tr.Initialize(
        boundary.GetSourcesIdsPointsAndAngles());
    
    vector<Point> tree_vector;
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);


    BranchNew l(tr.GetBranch(1)->TipPoint(), tr.GetBranch(1)->TipAngle()+M_PI/4),
        r(tr.GetBranch(1)->TipPoint(), tr.GetBranch(1)->TipAngle() - M_PI/4);
    
    auto[i1, i2] = tr.AddSubBranches(1, l, r);
    cout << i2;
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    tr.GetBranch(i1)->AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    TreeVertices(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(tree_vector.at(1), Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(tree_vector.at(2), Point{0.5, 0});    
}