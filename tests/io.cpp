//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "IO functions"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "io.hpp"

using namespace River;

const double eps = 1e-12;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( io_methods, 
    *utf::tolerance(eps))
{
    Model mdl;
    mdl.eps = 0.1;
    mdl.ds = 2;
    mdl.dx = 0.5;
    mdl.biffurcation_threshold = 0;
    mdl.width = 2;
    mdl.height = 3;
    mdl.field_value = 4;
    mdl.ac.exponant = 100;
    mdl.ac.min_area = 101;
    mdl.ac.r0 = 102;
    BOOST_TEST_CHECKPOINT("1");
    //Timing Object setup
    Timing time;

    //Border object setup.. Rectangular boundaries
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{mdl.width, mdl.height};
    auto sources_x_coord = vector<double>{mdl.dx};
    auto sources_id = vector<int>{1};
    
    BOOST_TEST_CHECKPOINT("2");
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.river_boundary_id = river_boundary_id;
    
    BOOST_TEST_CHECKPOINT("3");
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    
    BOOST_TEST_CHECKPOINT("4");
    //Tree object setup
    Tree tree;
    BOOST_TEST_CHECKPOINT("4.5");
    tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

    BranchNew br1left({0, 1}, 2), br1right({3, 4}, 5), br2left({6, 7}, 8), br2right({9, 10}, 11);
    br1left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br1right.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2right.AddAbsolutePoint(Polar{1, 0}).AddAbsolutePoint(Polar{1, 0});
    
    tree.AddSubBranches(1, br1left, br1right);
    tree.AddSubBranches(3, br2left, br2right);

    BOOST_TEST_CHECKPOINT("5");
    Save(mdl, time, border, tree, "iotest.json");
    mdl = Model();
    tree = Tree();
    //border = Border(border_mesh);
    BOOST_TEST_CHECKPOINT("6");
    Open(mdl, border, tree, "iotest.json");

    //Model TEST
    BOOST_TEST(mdl.eps == 0.1);
    BOOST_TEST(mdl.ds == 2);
    BOOST_TEST(mdl.dx == 0.5);
    BOOST_TEST(mdl.biffurcation_threshold == 0);
    BOOST_TEST(mdl.width == 2);
    BOOST_TEST(mdl.height == 3);
    BOOST_TEST(mdl.field_value == 4);
    BOOST_TEST(mdl.ac.exponant == 100);
    BOOST_TEST(mdl.ac.min_area == 101);
    BOOST_TEST(mdl.ac.r0 == 102);

    //Tree Test
    BOOST_TEST(tree.source_branches_id == sources_id);
    map<int, pair<int, int>> t = {{1, {2, 3}}, {3, {4, 5}}};
    BOOST_TEST(tree.branches_relation == t);
    BOOST_TEST(tree.GetBranch(5).TipPoint().x == 11);
    BOOST_TEST(tree.GetBranch(5).TipPoint().y == 10);
    //TODO

    //Border Test
    //TODO
}