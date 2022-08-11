//Link to Boost
//#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Rivers and River branch"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "rivers.hpp"

using namespace River;

const double eps = EPS;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE( tip_boundary, 
    *utf::tolerance(eps))
{
    Rivers rivers;

    auto boundary = rivers.TipBoundary();

    BOOST_TEST(boundary.lines.empty());
    BOOST_TEST(boundary.vertices.empty());

    t_boundary_id boundary_id = 1;
    t_branch_id branch_id = 1;
    double ds = 1.,
        dalpha = 0;
    unsigned n = 1;
    
    rivers.GrowTestTree(boundary_id, branch_id, ds, n, dalpha);

    boundary = rivers.TipBoundary();

    auto expected_lines = t_LineList{{0, 1, boundary_id}, {2, 3, boundary_id}};
    auto expected_vertices = t_PointList{{0, 1}, {-sqrt(2)/2, 1 + sqrt(2)/2}, {0, 1}, {sqrt(2)/2, 1 + sqrt(2)/2}};
    BOOST_TEST((boundary.lines == expected_lines));
    BOOST_TEST((boundary.vertices == expected_vertices));
}

/*
BOOST_AUTO_TEST_CASE( smoothness_debug, 
    *utf::tolerance(eps))
{
    auto source_point = Point{0, 0};
    auto source_angle = M_PI/2;

    auto branch = Branch(source_point, source_angle);

    for(int i = 0; i < 20; ++i)
        branch.AddPoint(Polar{0.1, 0.03}, 0);
    
    for(const auto & el: branch.vertices);
        cout << "x = " << el.x << ", y = " << el.y << endl;
    
    auto branch_smooth = branch.generateSmoothBoundary(2, 0);
    for(const auto & el: branch_smooth.vertices);
        cout << "x = " << el.x << ", y = " << el.y << endl;
}
*/

BOOST_AUTO_TEST_CASE( BranchNew_Class, 
    *utf::tolerance(eps))
{   
    //constructor: empty branch
    auto source_point = Point{0, 0};
    auto source_angle = M_PI/2;
    auto br = Branch(source_point, source_angle);

    BOOST_TEST(br.vertices.size() == 1);
    BOOST_TEST(br.Lenght() == 0);
    BOOST_TEST((br.TipPoint() == source_point));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);
    BOOST_CHECK_THROW(br.TipVector(), Exception);
    BOOST_CHECK_THROW(br.Vector(0), Exception);
    BOOST_CHECK_THROW(br.Vector(1), Exception);
    BOOST_TEST((br.SourcePoint() == source_point ));
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST((br.vertices == vector<Point>{source_point}));

    //now lets modify a little this branch
    t_boundary_id boundary_id = 0;
    BOOST_CHECK_NO_THROW(br.AddPoint(Point{0, 1},boundary_id));
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Point{0, 2}, boundary_id));
    BOOST_CHECK_NO_THROW(br.AddPoint(Polar{1, 0}, boundary_id));
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Polar{1, M_PI/2}, boundary_id));
    BOOST_TEST(br.vertices.size() == 5);
    BOOST_TEST(br.Lenght() == 4);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 4}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST((br.SourcePoint() == source_point));
    BOOST_TEST((br.vertices == vector<Point>{{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}}));

    //now let remove two tip point
    BOOST_CHECK_NO_THROW(br.RemoveTipPoint().RemoveTipPoint());
    BOOST_TEST(br.vertices.size() == 3);
    BOOST_TEST(br.Lenght() == 2);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 2}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST((br.SourcePoint() == source_point));
    BOOST_TEST((br.vertices == vector<Point>{{0, 0}, {0, 1}, {0, 2}}));
    BOOST_CHECK_NO_THROW(br.Shrink(2));
    BOOST_TEST(br.Lenght() == 0);
    BOOST_TEST(br.vertices.size() == 1);
    BOOST_TEST((br.TipPoint() == source_point));
    BOOST_TEST((br.SourcePoint() == source_point));
    BOOST_TEST(br.TipAngle() == br.SourceAngle());
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);
    //now let add points with different angles
    source_point = Point{0, 0};
    br = Branch(source_point, M_PI/6);
    BOOST_CHECK_NO_THROW(br.AddPoint(Polar{1, 0}, boundary_id).AddPoint(Polar{1, 0}, boundary_id));
    Point test_p_1{sqrt(3), 1};
    BOOST_TEST((br.TipPoint() == test_p_1));
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Polar{2, 5./6.* M_PI}, boundary_id));
    Point test_p_2{0, 2};
    BOOST_TEST((br.TipPoint() == test_p_2));

    //now let make circle in clockwise direction:
    source_point = Point{0, 0};
    br = Branch(source_point, M_PI/6);
    //clockwise
    for(size_t i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, 2*M_PI/10}, boundary_id);
    //counterclockwise
    for(size_t i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, -2*M_PI/10}, boundary_id);
    BOOST_TEST((br.TipPoint() == br.SourcePoint()));
    BOOST_TEST((source_point == br.SourcePoint()));

    br.Shrink(20);
    BOOST_TEST((br.TipPoint() == br.SourcePoint()));
    BOOST_TEST((source_point == br.SourcePoint()));
    br = Branch(source_point, 0);
    br.AddPoint(Polar{1, 0}, boundary_id);
    br.AddPoint(Polar{1, 0}, boundary_id);
    br.Shrink(0.5);
    BOOST_TEST(br.vertices.size() == 3);
    auto test_p_4 = Point{1.5, 0};
    BOOST_TEST((br.TipPoint() == test_p_4));
    br.Shrink(0.5);
    BOOST_TEST(br.vertices.size() == 2);
    BOOST_TEST(br.Lenght() == 1);
    auto test_p_3 = Point{1, 0};
    BOOST_TEST((br.TipPoint() == test_p_3));
}

BOOST_AUTO_TEST_CASE( BranchNew_vector, 
    *utf::tolerance(eps))
{   
    Branch br{Point{0, 0}, 0};
    BOOST_CHECK_THROW(br.Vector(0), Exception);

    t_boundary_id boundary_id = 0;
    br.AddPoint(Polar{1, 0}, boundary_id);
    auto test_p = Point{1, 0};
    BOOST_TEST((br.Vector(0) == test_p));
    BOOST_TEST(br.vertices.size() == 2);
    BOOST_CHECK_THROW(br.Vector(1), Exception);
    
    br.AddPoint(Polar{1, M_PI/2}, boundary_id);
    auto test_p_2 = Point{0, 1};
    BOOST_TEST((br.Vector(1) == test_p_2));
    BOOST_CHECK_THROW(br.Vector(2), Exception);
}

BOOST_AUTO_TEST_CASE( BranchNew_equal, 
    *utf::tolerance(eps))
{  
    Branch 
        br1{Point{0, 0}, 0}, 
        br2{Point{0, 0}, 0};
    
    BOOST_TEST((br1 == br2));

    t_boundary_id boundary_id = 0;
    br1.AddPoint(Point{1, 2}, boundary_id);

    BOOST_TEST(!(br1 == br2));

    br2.AddPoint(Point{1, 2}, boundary_id);

    BOOST_TEST((br1 == br2));
}

BOOST_AUTO_TEST_CASE( Tree_Class, 
    *utf::tolerance(eps))
{   
    vector<t_source_id> ids{3, 4, 5};
    Rivers tr;
    BOOST_CHECK_NO_THROW(
        tr.Initialize({
        {3, {{0.0, 0}, 0}},
        {4, {{0.1, 0}, 0}},
        {5, {{0.2, 0}, 0}}})
    );

    for(auto id: ids)
    {
        BOOST_TEST(tr.count(id));
        BOOST_TEST(!tr.HasSubBranches(id));
        BOOST_TEST(!tr.HasParentBranch(id));
    }
    auto tips = tr.TipBranchesIds();
    BOOST_TEST(tips == ids);

    t_boundary_id boundary_id = 0;
    auto& br = tr.at(3);
    Point p{1, 1};
    br.AddPoint(p, boundary_id);
    auto& br_new = tr.at(3);
    BOOST_TEST(br_new.vertices.size() == 2);

    //AddSources
    tr.AddBranch(Branch{{0.3, 0}, 0}, 6);
    ids.push_back(6);
    BOOST_TEST(tr.TipBranchesIds() == ids);
    BOOST_CHECK_THROW(tr.AddBranch(Branch{{0.3, 0}, 0}, 6), Exception);
    BOOST_CHECK_THROW(tr.HasParentBranch(1), Exception);


    //addPoints
    //different size
    BOOST_CHECK_THROW(tr.AddPoints({1, 2, 3}, {{0, 0}, {0.1, 0}}, {0, 0, 0}), Exception);
    
    auto test_point = Point{1, 1};
    auto tip_point = tr.at(3).TipPoint();
    
    tr.AddPoints({3}, {test_point}, {0});
    BOOST_TEST((tr.at(3).TipPoint() == (tip_point + test_point)));
}

BOOST_AUTO_TEST_CASE( Tree_Class_methods, 
    *utf::tolerance(eps))
{   
    vector<t_source_id> ids{5, 4, 9};
    Rivers tr;
    tr.Initialize({
        {5, {{0.0, 0}, 0}},
        {4, {{0.1, 0}, 0.1}},
        {9, {{0.2, 0}, 0.2}}
    });

    Branch 
        left_branch{tr.at(5).TipPoint(), 0.1},
        right_branch{tr.at(5).TipPoint(), -0.1};
    
    //ADD SUBBRANCHES TEST
    BOOST_TEST(!tr.HasSubBranches(5));         
    auto[b1, b2] = tr.AddSubBranches(5, left_branch, right_branch);
    BOOST_CHECK_THROW(tr.AddSubBranches(5, left_branch, right_branch), Exception);
    BOOST_TEST(tr.HasSubBranches(5));
    BOOST_TEST(tr.GetAdjacentBranchId(b1)    == b2);
    BOOST_TEST(tr.GetAdjacentBranchId(b2)    == b1);
    BOOST_TEST(tr.GetSubBranchesIds(5).first  == b1);
    BOOST_TEST(tr.GetSubBranchesIds(5).second == b2);
    BOOST_TEST(tr.HasParentBranch(b1));
    BOOST_TEST(tr.HasParentBranch(b2));

    //ADDBRANCH TEST
    BOOST_CHECK_THROW(tr.AddBranch(left_branch,  b1), Exception);
    BOOST_CHECK_THROW(tr.AddBranch(right_branch, b2), Exception);

    ids = vector<t_source_id>{10};
    tr.Clear();
    tr = Rivers();
    tr.Initialize({{10, {{0.0, 0}, 0.}}});
    
    BOOST_TEST( tr.TipBranchesIds() == ids);

    auto [c1, c2] = tr.AddSubBranches(10, left_branch, right_branch);
    auto new_ids = vector{c1, c2};
    auto new_ids_r = vector{c2, c1};
    BOOST_TEST(((tr.TipBranchesIds() == new_ids) || (tr.TipBranchesIds() == new_ids_r)));
    BOOST_CHECK_THROW(tr.AddSubBranches(79, left_branch, right_branch), Exception);
    BOOST_TEST(tr.HasParentBranch(c2));
    BOOST_TEST(tr.HasParentBranch(c1));

    //GETSOURCEBRANCH TEST
    BOOST_TEST(tr.GetParentBranchId(c1) == 10);
    BOOST_TEST(tr.GetParentBranchId(c2) == 10);

    BOOST_TEST(!tr.HasParentBranch(10));
    BOOST_TEST(tr.HasParentBranch(c1));
    BOOST_TEST(tr.HasParentBranch(c2));
    BOOST_CHECK_THROW(tr.GetParentBranchId(10), Exception);
    BOOST_CHECK_THROW(tr.GetParentBranchId(79), Exception);


    //GENERATE NEW ID
    tr = Rivers();
    tr.Initialize({{1, {{0.0, 0}, 0}}});

    BOOST_TEST(!tr.IsValidBranchId(0));
    BOOST_TEST(tr.IsValidBranchId(1));
    BOOST_TEST(tr.GenerateNewID() == 2);
    tr.AddBranch(Branch(Point{0, 0}, 0), 2);
    BOOST_TEST(tr.GenerateNewID() == 3);
    tr.AddBranch(Branch(Point{0, 0}, 0), 3);
    BOOST_TEST(tr.GenerateNewID() == 4);
    tr.AddBranch(Branch(Point{0, 0}, 0), 4);
    BOOST_TEST(tr.GenerateNewID() == 5);

    BOOST_CHECK_THROW(tr.AddBranch(left_branch, 0), Exception);
}

/*
BOOST_AUTO_TEST_CASE( tree_tips_point_method, 
    *utf::tolerance(eps))
{
    Region border;
    border[1] =
    {
        {//vertices(counterclockwise order)
            {0, 0},
            {0.5, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        }, 
        {//lines
            {0, 1, 0},
            {1, 2, 0},
            {2, 3, 1},
            {3, 4, 2},
            {4, 0, 3} 
        }
    };

    Sources sources;
    sources[1] = {1, 1};

    Rivers tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));

    BOOST_TEST(tr.TipPoints().size() == 1);
    BOOST_TEST((tr.TipPoints().at(0) == Point{0.5, 0}));
}

BOOST_AUTO_TEST_CASE( add_points_tests, 
    *utf::tolerance(eps))
{
    Region border;
    border[1] = 
    {
        {//vertices(counterclockwise order)
            {0, 0},
            {0.5, 0},
            {0.6, 0},
            {0.7, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        }, 
        {//lines
            {0, 1, 0},
            {1, 2, 0},
            {2, 3, 0},
            {3, 4, 0},
            {4, 5, 1}, 
            {6, 7, 2}, 
            {7, 0, 3} 
        }
    };//Outer Boundary
        
    Sources sources;
    sources[1] = {1, 1};
    sources[2] = {1, 2};
    sources[3] = {1, 3};

    Rivers tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));

    tr.AddPoints({1, 2, 3}, {Point{0, 0.1}, Point{0, 0.1}, Point{0, 0.1}}, {0, 0, 0});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.1)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.1)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.1)));

    tr.AddPolars({1, 2, 3}, {Polar{0.1, 0}, Polar{0.1, 0}, Polar{0.1, 0}}, {0, 0, 0});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.2)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.2)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.2)));

    tr.AddAbsolutePolars({1, 2, 3}, {Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}}, {0, 0, 0});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.3)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.3)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.3)));

    tr.AddAbsolutePolars({1, 2, 3}, {Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}}, {0, 0, 0});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.4)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.4)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.4)));
}
*/

BOOST_AUTO_TEST_CASE(shrink_test, *utf::tolerance(eps))
{
    t_boundary_id boundary_id = 0;
    Branch br{Point{0, 0}, 0};
    br.AddPoint(Polar{1, 0}, boundary_id);
    br.AddPoint(Polar{1, 0}, boundary_id);
    br.AddPoint(Polar{1, 0}, boundary_id);
    br.AddPoint(Polar{1, 0}, boundary_id);

    BOOST_TEST(br.Lenght() == 4.);
    BOOST_TEST(br.vertices.size() == 5);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 3.999);
    BOOST_TEST(br.vertices.size() == 5);

    br.Shrink(0.009);
    BOOST_TEST(br.Lenght() == 3.990);
    BOOST_TEST(br.vertices.size() == 5);

    br.Shrink(0.09);
    BOOST_TEST(br.Lenght() == 3.9);
    BOOST_TEST(br.vertices.size() == 5);

    br.Shrink(0.9);
    BOOST_TEST(br.Lenght() == 3.);
    BOOST_TEST(br.vertices.size() == 4);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 2.);
    BOOST_TEST(br.vertices.size() == 3);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 1.);
    BOOST_TEST(br.vertices.size() == 2);

    br.Shrink(0.999);
    BOOST_TEST(br.Lenght() == 0.001);
    BOOST_TEST(br.vertices.size() == 2);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 0.);
    BOOST_TEST(br.vertices.size() == 1);
}


// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Tree_UPD, 
    *utf::tolerance(eps))
{   
    Rivers tree;
    Branch brTest({0, 0}, 0);

    BOOST_TEST(tree.TipBranchesIds().size() == 0);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 0);
    BOOST_TEST(tree.TipPoints().size() == 0);
    BOOST_TEST(tree.GenerateNewID() == 1);
    
    BOOST_CHECK_THROW(tree.AddBranch(brTest, 0), Exception);
    BOOST_CHECK_THROW(tree.AddSubBranches(0, brTest, brTest), Exception);

    BOOST_CHECK_THROW(tree.GetParentBranchId(0), Exception);
    BOOST_CHECK_THROW(tree.GetParentBranchId(2), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranchesIds(2), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranchId(2), Exception);

    BOOST_CHECK_THROW(tree.at(2), std::exception);
    BOOST_CHECK_THROW(tree.GetParentBranch(2), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranch(2), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranches(2), Exception);

    BOOST_CHECK_THROW(tree.DeleteSubBranches(5), Exception);
    BOOST_CHECK_THROW(tree.DeleteBranch(5), std::exception);

    BOOST_CHECK_THROW(tree.AddPoints({1, 2}, {Point{0, 0}, Point{1, 1}}, {0, 0}), Exception);
    BOOST_CHECK_THROW(tree.AddAbsolutePolars({1, 2}, {Polar{0, 0}, Polar{1, 1}}, {0, 0}), Exception);

    BOOST_TEST(tree.count(1) == false);
    BOOST_CHECK_THROW(tree.IsSourceBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasSubBranches(1), Exception);

    BOOST_TEST(tree.IsValidBranchId(0) == false);
    BOOST_TEST(tree.IsValidBranchId(1) == true);

    t_boundary_id boundary_id = 0;
    Branch br1({0, 0}, 1.4);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);

    Branch br2({0, 0}, 1.7);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);

    Branch br3({0, 0}, 1.9);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);

    Branch br4({0, 0}, 2.4);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);

    Branch br5({0, 0}, 4.4);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);

    Branch br6({0, 0}, 0.4);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);

    Branch br7({0, 0}, 0.6);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);

    //building tree
    auto id = tree.AddBranch(br1);
    BOOST_TEST(id == 1);
    BOOST_TEST(tree.TipBranchesIds().size() == 1);
    BOOST_TEST(tree.TipBranchesIds().at(0) == id);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 1);
    BOOST_TEST(tree.TipPoints().size() == 1);
    BOOST_TEST((tree.TipPoints().at(0) == br1.TipPoint()));
    BOOST_TEST(tree.GenerateNewID() == 2);

    BOOST_TEST(tree.count(id) == true);
    BOOST_TEST(tree.IsSourceBranch(id) == true);
    BOOST_TEST(tree.HasParentBranch(id) == false);
    BOOST_TEST(tree.HasSubBranches(id) == false);

    auto[id2, id3] = tree.AddSubBranches(id, br2, br3);
    BOOST_TEST(tree.IsSourceBranch(id) == true);
    BOOST_TEST(tree.HasParentBranch(id) == false);
    BOOST_TEST(tree.HasSubBranches(id) == true);

    BOOST_TEST(tree.IsSourceBranch(id2) == false);
    BOOST_TEST(tree.HasParentBranch(id2) == true);
    BOOST_TEST(tree.HasSubBranches(id2) == false);

    BOOST_TEST(tree.IsSourceBranch(id3) == false);
    BOOST_TEST(tree.HasParentBranch(id3) == true);
    BOOST_TEST(tree.HasSubBranches(id3) == false);

    BOOST_TEST(tree.TipBranchesIds().size() == 2);
    BOOST_TEST((tree.TipBranchesIds().at(0) == id2 ||
        tree.TipBranchesIds().at(0) == id3));
    BOOST_TEST((tree.TipBranchesIds().at(1) == id2 ||
        tree.TipBranchesIds().at(1) == id3));

    BOOST_TEST(tree.GetParentBranchId(id2) == id);
    BOOST_TEST(tree.GetParentBranchId(id3) == id);
    BOOST_TEST(tree.GetAdjacentBranchId(id2) == id3);
    BOOST_TEST(tree.GetAdjacentBranchId(id3) == id2);

    auto[id4, id5] = tree.AddSubBranches(id2, br4, br5);
    auto[id6, id7] = tree.AddSubBranches(id3, br6, br7);

    BOOST_TEST(tree.IsSourceBranch(id2) == false);
    BOOST_TEST(tree.HasParentBranch(id2) == true);
    BOOST_TEST(tree.HasSubBranches(id2) == true);

    BOOST_TEST(tree.IsSourceBranch(id3) == false);
    BOOST_TEST(tree.HasParentBranch(id3) == true);
    BOOST_TEST(tree.HasSubBranches(id3) == true);

    BOOST_TEST((tree.GetSubBranchesIds(id2) == pair<t_source_id, t_source_id>{id4, id5}));
    BOOST_TEST((tree.GetSubBranchesIds(id3) == pair<t_source_id, t_source_id>{id6, id7}));
    BOOST_TEST(tree.GetAdjacentBranchId(id4) == id5);
    BOOST_TEST(tree.GetAdjacentBranchId(id5) == id4);
    BOOST_TEST(tree.GetAdjacentBranchId(id6) == id7);
    BOOST_TEST(tree.GetAdjacentBranchId(id7) == id6);
    BOOST_TEST(tree.GetParentBranchId(id6) == id3);
    BOOST_TEST(tree.GetParentBranchId(id7) == id3);
    BOOST_TEST(tree.GetParentBranchId(id4) == id2);
    BOOST_TEST(tree.GetParentBranchId(id5) == id2);

    BOOST_TEST(tree.TipBranchesIds().size() == 4);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 4);
    BOOST_TEST(tree.TipPoints().size() == 4);
    BOOST_TEST((tree.GenerateNewID() == (t_branch_id)(id7 + 1)));

    //delete
    tree.DeleteSubBranches(id3);
    BOOST_TEST(tree.count(id6) == false);
    BOOST_TEST(tree.count(id7) == false);
    BOOST_TEST(tree.IsSourceBranch(id3) == false);
    BOOST_TEST(tree.HasParentBranch(id3) == true);
    BOOST_TEST(tree.HasSubBranches(id3) == false);

    tree.DeleteSubBranches(id);
    BOOST_TEST(tree.count(id4) == false);
    BOOST_TEST(tree.count(id5) == false);
    BOOST_TEST(tree.count(id3) == false);
    BOOST_TEST(tree.count(id2) == false);
    BOOST_TEST(tree.HasSubBranches(id) == false);
}

BOOST_AUTO_TEST_CASE( Tree_UPD_delete, 
    *utf::tolerance(eps))
{   
    t_boundary_id boundary_id = 0;
    Rivers tree;
    Branch br1({0, 0}, 1.4);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);
    br1.AddPoint(Polar{1, 0}, boundary_id);

    Branch br2({0, 0}, 1.7);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);
    br2.AddPoint(Polar{1, 0}, boundary_id);

    Branch br3({0, 0}, 1.9);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);
    br3.AddPoint(Polar{1, 0}, boundary_id);

    Branch br4({0, 0}, 2.4);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);
    br4.AddPoint(Polar{1, 0}, boundary_id);

    Branch br5({0, 0}, 4.4);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);
    br5.AddPoint(Polar{1, 0}, boundary_id);

    Branch br6({0, 0}, 0.4);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);
    br6.AddPoint(Polar{1, 0}, boundary_id);

    Branch br7({0, 0}, 0.6);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);
    br7.AddPoint(Polar{1, 0}, boundary_id);

    auto id1 = tree.AddBranch(br1);
    auto[id2, id3] = tree.AddSubBranches(id1, br2, br3);
    auto[id4, id5] = tree.AddSubBranches(id2, br4, br5);
    auto[id6, id7] = tree.AddSubBranches(id3, br6, br7);

    BOOST_TEST(tree.count(id2));
    BOOST_TEST(tree.count(id3));
    BOOST_TEST(tree.count(id4));
    BOOST_TEST(tree.count(id5));
    BOOST_TEST(tree.count(id6));
    BOOST_TEST(tree.count(id7));

    tree.DeleteSubBranches(id1);
    BOOST_TEST(!tree.count(id2));
    BOOST_TEST(!tree.count(id3));
    BOOST_TEST(!tree.count(id4));
    BOOST_TEST(!tree.count(id5));
    BOOST_TEST(!tree.count(id6));
    BOOST_TEST(!tree.count(id7));
}

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Tree_UPD_tips, 
    *utf::tolerance(eps))
{
    //TODO test tips: points, ids etc.. 
}

BOOST_AUTO_TEST_CASE( construct_empty_tree, 
    *utf::tolerance(eps))
{   
    Rivers tree;
    BOOST_CHECK_THROW(tree.GetParentBranchId(1), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranchesIds(1), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranchId(1), Exception);
    BOOST_CHECK_THROW(tree.at(1), std::exception);
    BOOST_CHECK_THROW(tree.GetParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranches(1), Exception);
    BOOST_CHECK_THROW(tree.DeleteSubBranches(1), Exception);
    BOOST_CHECK_THROW(tree.DeleteBranch(1), Exception);
    BOOST_CHECK_THROW(tree.AddPoints({1, 2}, {{1, 1}, {2, 2}}, {0, 0}), Exception);
    BOOST_CHECK_THROW(tree.AddPolars( {1, 2}, {{1, 1}, {2, 2}}, {0, 0}), Exception);
    BOOST_CHECK_THROW(tree.AddAbsolutePolars({1, 2}, {{1, 1}, {2, 2}}, {0, 0}), Exception);
    BOOST_TEST(!tree.count(1));
    BOOST_CHECK_THROW(tree.IsSourceBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasSubBranches(1), Exception);
    BOOST_TEST(tree.empty());
    BOOST_TEST((tree.TipBranchesIds() == vector<t_branch_id>{}));
    BOOST_TEST((tree.TipPoints() == vector<Point>{}));
    BOOST_TEST((tree.TipIdsAndPoints() == map<t_branch_id, Point>{}));
}

BOOST_AUTO_TEST_CASE( tree_copy_constructor, 
    *utf::tolerance(eps))
{   
    Rivers tree_original;
    tree_original.Initialize({{1, {{0., 0.}, 10}}});

    Rivers tree_copy{tree_original}, tree_not_copy;

    BOOST_TEST((tree_original == tree_copy));
    BOOST_TEST(!(tree_original == tree_not_copy));
    BOOST_TEST(tree_original.TipBranchesIds() == tree_copy.TipBranchesIds());
    BOOST_TEST((tree_original.TipIdsAndPoints() == tree_copy.TipIdsAndPoints()));
}

/*
BOOST_AUTO_TEST_CASE( tree_initializtion_and_clear, 
    *utf::tolerance(eps))
{
    Region::trees_interface_t ids_points_angles = 
    {
        {1, {{2, 3}, 4}},
        {3, {{3, 4}, 5}},
        {5, {{4, 5}, 6}},
        {7, {{5, 6}, 7}}};
    auto source_branches_id = vector<t_source_id>{1, 3, 5, 7};
    auto tip_points = vector<Point>{{2, 3}, {3, 4}, {4, 5}, {5, 6}};
    auto tip_ids_and_points = 
        map<t_source_id, Point>{{1, {2, 3}}, {3, {3, 4}}, {5, {4, 5}}, {7, {5, 6}}};

    Rivers tree;
    tree.Initialize(ids_points_angles);

    BOOST_TEST(tree.TipBranchesIds() == source_branches_id);
    BOOST_TEST(tree.TipPoints() == tip_points);
    BOOST_TEST(tree.TipIdsAndPoints() == tip_ids_and_points);

    //clear test
    tree.Clear();
    source_branches_id = {};
    tip_points = {};
    tip_ids_and_points = {};
    BOOST_TEST(tree.TipBranchesIds() == source_branches_id);
    BOOST_TEST(tree.TipPoints() == tip_points);
    BOOST_TEST(tree.TipIdsAndPoints() == tip_ids_and_points);
}
*/

BOOST_AUTO_TEST_CASE( tree_add_and_delete_branch, 
    *utf::tolerance(eps))
{
    Rivers tree;
    Branch 
        branch_1{Point{1, 2}, 3}, 
        branch_2{Point{2, 3}, 4},
        branch_3{Point{3, 4}, 5},
        branch_1_duplicate{Point{1, 2}, 3};

    BOOST_CHECK_THROW(tree.AddBranch(branch_1, 0), Exception);
    t_branch_id id = -1;
    BOOST_CHECK_NO_THROW((id = tree.AddBranch(branch_1)));
    BOOST_CHECK_THROW((tree.AddBranch(branch_1, id)), Exception);

    //.....
}

/*
BOOST_AUTO_TEST_CASE( test_branch_growth, 
    *utf::tolerance(eps))
{
    Region::trees_interface_t ids_points_angles = {
        {1, {{2, 3}, 0}},
        {3, {{3, 4},   M_PI/4.}},
        {5, {{4, 5},   M_PI/2.}},
        {7, {{5, 6}, 3*M_PI/4.}}
    };

    Rivers tree;
    
    tree.Initialize(ids_points_angles);
    {
        t_boundary_id boundary_id = 1;
        auto ds = 0.05,
            dalpha = 0.0;
        auto n = 3;
        auto branch_id = 1;
        auto[left_id, right_id] = tree.GrowTestTree(boundary_id, branch_id, ds, n, dalpha);

        auto& br = tree.at(branch_id), 
            &br_left = tree.at(left_id),
            &br_right = tree.at(right_id);

        auto br_vetices = vector<Point>{
            {2,        3}, 
            {2 +   ds, 3}, 
            {2 + 2*ds, 3}, 
            {2 + 3*ds, 3}};
        BOOST_TEST(br.vertices.at(0) == br_vetices.at(0));
        BOOST_TEST(br.vertices.at(1) == br_vetices.at(1));
        BOOST_TEST(br.vertices.at(2) == br_vetices.at(2));
        BOOST_TEST(br.vertices.at(3) == br_vetices.at(3));

        auto br_vetices_left = vector<Point>{
            {2 + 3*ds,                 3}, 
            {2 + 3*ds +   ds/sqrt(2.), 3 + ds/sqrt(2.)}, 
            {2 + 3*ds + 2*ds/sqrt(2.), 3 + 2*ds/sqrt(2.)}, 
            {2 + 3*ds + 3*ds/sqrt(2.), 3 + 3*ds/sqrt(2.)}};
        BOOST_TEST(br_left.vertices.at(0) == br_vetices_left.at(0));
        BOOST_TEST(br_left.vertices.at(1) == br_vetices_left.at(1));
        BOOST_TEST(br_left.vertices.at(2) == br_vetices_left.at(2));
        BOOST_TEST(br_left.vertices.at(3) == br_vetices_left.at(3));

        auto br_vetices_right = vector<Point>{
            {2 + 3*ds,                 3}, 
            {2 + 3*ds +   ds/sqrt(2.), 3 - ds/sqrt(2.)}, 
            {2 + 3*ds + 2*ds/sqrt(2.), 3 - 2*ds/sqrt(2.)}, 
            {2 + 3*ds + 3*ds/sqrt(2.), 3 - 3*ds/sqrt(2.)}};
        BOOST_TEST(br_right.vertices.at(0) == br_vetices_right.at(0));
        BOOST_TEST(br_right.vertices.at(1) == br_vetices_right.at(1));
        BOOST_TEST(br_right.vertices.at(2) == br_vetices_right.at(2));
        BOOST_TEST(br_right.vertices.at(3) == br_vetices_right.at(3));
    }

    //lets try different branch with different id
    {
        t_boundary_id boundary_id = 1;
        auto branch_id = 5;
        auto ds = 0.1,
            dalpha = 0.0;
        auto n = 2;
        auto[left_id, right_id] = tree.GrowTestTree(boundary_id, branch_id, ds, n, dalpha);
        
        auto& br = tree.at(branch_id), 
            &br_left = tree.at(left_id),
            &br_right = tree.at(right_id);
        auto br_vetices = vector<Point>{
            {4, 5}, 
            {4, 5 +   ds}, 
            {4, 5 + 2*ds}};
        BOOST_TEST(br.vertices.at(0) == br_vetices.at(0));
        BOOST_TEST(br.vertices.at(1) == br_vetices.at(1));
        BOOST_TEST(br.vertices.at(2) == br_vetices.at(2));
    
        auto br_vetices_left = vector<Point>{
            {4,                 5 + 2*ds}, 
            {4 -   ds/sqrt(2.), 5 + 2*ds + ds/sqrt(2.)}, 
            {4 - 2*ds/sqrt(2.), 5 + 2*ds + 2*ds/sqrt(2.)}};
        BOOST_TEST(br_left.vertices.at(0) == br_vetices_left.at(0));
        BOOST_TEST(br_left.vertices.at(1) == br_vetices_left.at(1));
        BOOST_TEST(br_left.vertices.at(2) == br_vetices_left.at(2));
    
        auto br_vetices_right = vector<Point>{
            {4,                 5 + 2*ds}, 
            {4 +   ds/sqrt(2.), 5 + 2*ds + ds/sqrt(2.)}, 
            {4 + 2*ds/sqrt(2.), 5 + 2*ds + 2*ds/sqrt(2.)}};
        BOOST_TEST(br_right.vertices.at(0) == br_vetices_right.at(0));
        BOOST_TEST(br_right.vertices.at(1) == br_vetices_right.at(1));
        BOOST_TEST(br_right.vertices.at(2) == br_vetices_right.at(2));
    }
}
*/

BOOST_AUTO_TEST_CASE( test_branch_growth_one_more, 
    *utf::tolerance(eps))
{
    Rivers tree;
    auto branch = Branch{{0.5, 0}, M_PI/2.};

    t_boundary_id boundary_id = 0;
    auto branch_id = tree.AddBranch(branch);
    double ds = 0.1;
    unsigned n = 4;
    tree.GrowTestTree(boundary_id, branch_id, ds, n);

    auto first_branch = Branch{{0.5, 0}, M_PI/2.},
        left_branch = Branch{{0.5, ds*n}, 3*M_PI/4.},
        right_branch = Branch{{0.5, ds*n}, M_PI/4.};

    first_branch.vertices.push_back({0.5, ds});
    first_branch.vertices.push_back({0.5, 2*ds});
    first_branch.vertices.push_back({0.5, 3*ds});
    first_branch.vertices.push_back({0.5, 4*ds});
    first_branch.lines = {{0, 1, boundary_id}, {1, 2, boundary_id}, 
        {2, 3, boundary_id}, {3, 4, boundary_id}};

    left_branch.vertices.push_back({0.5 -   ds/sqrt(2),  n*ds +   ds/sqrt(2)});
    left_branch.vertices.push_back({0.5 - 2*ds/sqrt(2),  n*ds + 2*ds/sqrt(2)});
    left_branch.vertices.push_back({0.5 - 3*ds/sqrt(2),  n*ds + 3*ds/sqrt(2)});
    left_branch.vertices.push_back({0.5 - 4*ds/sqrt(2),  n*ds + 4*ds/sqrt(2)});
    left_branch.lines = {{0, 1, boundary_id}, {1, 2, boundary_id}, 
        {2, 3, boundary_id}, {3, 4, boundary_id}};

    right_branch.vertices.push_back({0.5 +   ds/sqrt(2), n*ds +   ds/sqrt(2)});
    right_branch.vertices.push_back({0.5 + 2*ds/sqrt(2), n*ds + 2*ds/sqrt(2)});
    right_branch.vertices.push_back({0.5 + 3*ds/sqrt(2), n*ds + 3*ds/sqrt(2)});
    right_branch.vertices.push_back({0.5 + 4*ds/sqrt(2), n*ds + 4*ds/sqrt(2)});
    right_branch.lines = {{0, 1, boundary_id}, {1, 2, boundary_id}, 
        {2, 3, boundary_id}, {3, 4, boundary_id}};

    auto[left, right] = tree.GetSubBranches(branch_id);

    BOOST_TEST((tree.at(branch_id) == first_branch));
    BOOST_TEST((left == left_branch));
    BOOST_TEST((right == right_branch));
}

BOOST_AUTO_TEST_CASE( test_maximal_tip_curvature_distance, 
    *utf::tolerance(eps))
{
    Rivers tree;

    BOOST_TEST(tree.maximal_tip_curvature_distance() == 0.);

    Branch branch2({10, 0}, 0);
    branch2.AddPoint(Polar{1, 0}, 0);
    branch2.AddPoint(Polar{1, 0}, 0);

    tree.AddBranch(branch2);
    BOOST_TEST(tree.maximal_tip_curvature_distance() == 0.);

    Branch branch1({0, 0}, 0);
    branch1.AddPoint(Polar{1, 0}, 0);
    branch1.AddPoint(Polar{1, M_PI/2.}, 0);

    tree.AddBranch(branch1);

    BOOST_TEST(tree.maximal_tip_curvature_distance() == 1./sqrt(2.));
}

/*
BOOST_AUTO_TEST_CASE( test_remove_tip_points, 
    *utf::tolerance(eps))
{
    t_boundary_id boundary_id = 0;
    t_branch_id branch_id = 1;
    Region::trees_interface_t ids_points_angles = {
        {branch_id, {{0., 0.}, M_PI/2.}}
    };

    Rivers tree;
    tree.Initialize(ids_points_angles);
    double ds = 1,
        dalpha = 0.;
    unsigned n = 2;
    auto[left_id, right_id] = tree.GrowTestTree(boundary_id, branch_id, ds, n, dalpha);
    
    auto expected_point = Point{ - sqrt(2)*ds, 2 + sqrt(2)*ds};
    BOOST_TEST(tree.at(left_id).TipPoint()  == expected_point);
    
    expected_point = Point{ + sqrt(2)*ds, 2 + sqrt(2)*ds};
    BOOST_TEST(tree.at(right_id).TipPoint() == expected_point);

    tree.remove_tip_points();
    expected_point = Point{ - ds/sqrt(2), 2 + ds/sqrt(2)};
    BOOST_TEST(tree.at(left_id ).TipPoint() == expected_point);

    expected_point = Point{ + ds/sqrt(2), 2 + ds/sqrt(2)};
    BOOST_TEST(tree.at(right_id).TipPoint() == expected_point);

    tree.remove_tip_points();
    BOOST_TEST(tree.count(left_id ) == false);
    BOOST_TEST(tree.count(right_id) == false);

    expected_point = Point{0, 2};
    BOOST_TEST(tree.at(branch_id).TipPoint() == expected_point);

    tree.remove_tip_points();
    expected_point = Point{0, 1};
    BOOST_TEST(tree.at(branch_id).TipPoint() == expected_point);

    tree.remove_tip_points();
    expected_point = Point{0, 0};
    BOOST_TEST(tree.at(branch_id).TipPoint() == expected_point);

    tree.remove_tip_points();
    BOOST_TEST(tree.at(branch_id).TipPoint() == expected_point);
}

BOOST_AUTO_TEST_CASE( test_zero_lenght_tip_branches_ids, 
    *utf::tolerance(eps))
{
    Rivers tree;

    BOOST_TEST(tree.zero_lenght_tip_branches_ids(0.0001).empty());

    t_boundary_id boundary_id = 0;
    t_branch_id branch_id = 1;
    Region::trees_interface_t ids_points_angles = {
        {branch_id, {{0., 0.}, M_PI/2.}}
    };
    tree.Initialize(ids_points_angles);
    double ds = 1.,
        dalpha = 0.;
    unsigned n = 2;
    auto[left_id, right_id] = tree.GrowTestTree(boundary_id, branch_id, ds, n, dalpha);

    BOOST_TEST(tree.zero_lenght_tip_branches_ids(0.0001).empty());

    tree.at(left_id).Shrink(ds*n);
    tree.at(right_id).Shrink(ds*n);

    auto result = tree.zero_lenght_tip_branches_ids(0.0001);
    BOOST_TEST(result.size() == 1);
    BOOST_TEST(result.at(0) == branch_id);
}
*/

BOOST_AUTO_TEST_CASE( test_flatten_tip_curvature, 
    *utf::tolerance(eps))
{
    Rivers tree;

    auto branch = Branch{{0, 0}, M_PI/2.};
    branch.AddPoint(Polar{1, 0}, 0);
    auto id = tree.AddBranch(branch);

    BOOST_CHECK_THROW(tree.flatten_tip_curvature(), Exception);

    tree.Clear();

    branch.AddPoint(Polar{1, 0}, 0);
    id = tree.AddBranch(branch);
    BOOST_CHECK_NO_THROW(tree.flatten_tip_curvature());

    BOOST_TEST((tree.at(id).vertices.at(1) == Point{0, 1}));

    tree.at(id).AddPoint(Polar{1, -M_PI/2.}, 0);

    tree.flatten_tip_curvature();
    auto expected_point = Point{1./2., 1. + 1./2.};
    BOOST_TEST((tree.at(id).vertices.at(2) == expected_point));
}