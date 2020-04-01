//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "GemometryNew Classes"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "tree.hpp"
#include "border.hpp"

using namespace River;

const double eps = 1e-15;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( BranchNew_Class, 
    *utf::tolerance(eps))
{   
    //or seed point
    auto source_point = Point{0, 0};
    auto br = BranchNew(source_point, M_PI/2);

    BOOST_TEST(br.Empty());
    BOOST_TEST(br.Size() == 1);
    BOOST_TEST(br.Lenght() == 0);
    BOOST_CHECK_THROW(br.AverageSpeed(), Exception);
    BOOST_CHECK_THROW(br.TipVector(), Exception);
    BOOST_TEST(br.TipPoint() == source_point);
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point );
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);

    //now lets modify a little this branch
    br.AddPoint(Point{0, 1});
    br.AddAbsolutePoint(Point{0, 2});
    br.AddPoint(Polar{1, 0});
    br.AddAbsolutePoint(Polar{1, M_PI/2});

    BOOST_TEST(!br.Empty());
    BOOST_TEST(br.Size() == 5);
    BOOST_TEST(br.Lenght() == 4);
    BOOST_TEST(br.AverageSpeed() == 1);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 4}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point );

    //now let remove two points
    br.RemoveTipPoint().RemoveTipPoint();
    BOOST_TEST(!br.Empty());
    BOOST_TEST(br.Size() == 3);
    BOOST_TEST(br.Lenght() == 2);
    BOOST_TEST(br.AverageSpeed() == 1);
    BOOST_TEST((br.TipVector() == Point{0, 1}));
    BOOST_TEST((br.TipPoint() == Point{0, 2}));
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point );
    
    br.Shrink(2);
    BOOST_TEST(br.Lenght() == 0);
    BOOST_TEST(br.TipPoint() == source_point);
    BOOST_TEST(br.SourcePoint() == source_point);
    BOOST_CHECK_THROW(br.RemoveTipPoint(), Exception);
    

    //now let add points with different angles
    source_point = Point{0, 0};
    br = BranchNew(source_point, M_PI/6);
    br.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    Point test_p_1{sqrt(3), 1};
    BOOST_TEST(br.TipPoint() == test_p_1);
    br.AddAbsolutePoint(Polar{2, 5./6.* M_PI});
    Point test_p_2{0, 2};
    BOOST_TEST(br.TipPoint() == test_p_2);

    //now let make circle in clockwise direction:
    source_point = Point{0, 0};
    br = BranchNew(source_point, M_PI/6);
    //clockwise
    for(int i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, 2*M_PI/10});
    //counterclockwise
    for(int i = 0; i < 10; ++i)
        br.AddPoint(Polar{1, -2*M_PI/10});

    BOOST_TEST(br.TipPoint() == br.SourcePoint());
    BOOST_TEST(source_point == br.SourcePoint());

    br.Shrink(20);
    BOOST_TEST(br.TipPoint() == br.SourcePoint());
    BOOST_TEST(source_point == br.SourcePoint());


    br = BranchNew(source_point, 0);
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.Shrink(0.5);
    BOOST_TEST(br.Size() == 3);
    auto test_p_4 = Point{1.5, 0};
    BOOST_TEST(br.TipPoint() == test_p_4);
    br.Shrink(0.5);
    BOOST_TEST(br.Size() == 2);
    BOOST_TEST(br.Lenght() == 1);
    auto test_p_3 = Point{1, 0};
    BOOST_TEST(br.TipPoint() == test_p_3);
}



BOOST_AUTO_TEST_CASE( BranchNew_vector, 
    *utf::tolerance(eps))
{   
    BranchNew br{Point{0, 0}, 0};
    BOOST_CHECK_THROW(br.Vector(0), Exception);

    br.AddPoint(Polar{1, 0});
    auto test_p = Point{1, 0};
    BOOST_TEST(br.Vector(1) == test_p);
    BOOST_TEST(br.Size() == 2);
    BOOST_CHECK_THROW(br.Vector(2), Exception);

    br.AddPoint(Polar{1, M_PI/2});
    auto test_p_2 = Point{0, 1};
    BOOST_TEST(br.Vector(2) == test_p_2);
    BOOST_CHECK_THROW(br.Vector(3), Exception);
}






BOOST_AUTO_TEST_CASE( Tree_Class, 
    *utf::tolerance(eps))
{   
    vector<int> ids{3, 4, 5};
    Tree tr;
    tr.Initialize(
        {{0.0, 0}, {0.1, 0}, {0.2, 0}}, 
        {0, 0, 0},
        ids);

    BOOST_TEST(tr.NumberOfSourceBranches() == 3);
    for(auto id: ids)
    {
        BOOST_TEST(tr.DoesExistBranch(id));
        BOOST_TEST(!tr.HasSubBranches(id));
        BOOST_TEST(!tr.HasParentBranch(id));
    }
    auto tips = tr.TipBranchesId();
    BOOST_TEST(tips == ids);


    //cool work with pointers.. but auto will force simple BranchNew
    auto br = tr.GetBranch(3);
    Point p{1, 1};
    br->AddPoint(p);
    auto br_new = tr.GetBranch(3);
    BOOST_TEST(br_new->Size() == 2);

    //addSources
    tr.AddSourceBranch(BranchNew{{0.3, 0}, 0}, 6);
    ids.push_back(6);
    BOOST_TEST(tr.TipBranchesId() == ids);
    BOOST_CHECK_THROW(tr.AddSourceBranch(BranchNew{{0.3, 0}, 0}, 6), Exception);
    BOOST_CHECK_THROW(tr.HasParentBranch(1), Exception);


    //addPoints
    //different size
    BOOST_CHECK_THROW(tr.AddPoints({{0, 0}, {0.1, 0}}, {1, 2, 3}), Exception);
    
    auto test_point = Point{1, 1};
    auto tip_point = tr.GetBranch(3)->TipPoint();

    BOOST_TEST(tr.AddPoints({test_point}, {3}).GetBranch(3)->TipPoint() == (tip_point+test_point));

}




BOOST_AUTO_TEST_CASE( Tree_Class_methods, 
    *utf::tolerance(eps))
{   
    vector<int> ids{5, 4, 9};
    Tree tr;
    tr.Initialize(
        {{0.0, 0}, {0.1, 0}, {0.2, 0}}, 
        {0.0, 0.1, 0.2},
        ids);

    BranchNew 
        left_branch{tr.GetBranch(5)->TipPoint(), 0.1},
        right_branch{tr.GetBranch(5)->TipPoint(), -0.1};
    
    //ADD SUBBRANCHES TEST
    BOOST_TEST(!tr.HasSubBranches(5));         
    auto[b1, b2] = tr.AddSubBranches(5, left_branch, right_branch);
    BOOST_CHECK_THROW(tr.AddSubBranches(5, left_branch, right_branch), Exception);
    BOOST_TEST(tr.HasSubBranches(5));
    BOOST_TEST(tr.DoesExistBranch(b1));
    BOOST_TEST(tr.DoesExistBranch(b2));

    //ADDBRANCH TEST
    BOOST_CHECK_THROW(tr.AddBranch(left_branch, b1), Exception);
    BOOST_CHECK_THROW(tr.AddBranch(right_branch, b2), Exception);

    ids = vector{10};
    tr = Tree();
    tr.Initialize({{0.0, 0}}, {0.0}, ids);
    
    BOOST_TEST(tr.TipBranchesId() == ids);
    auto [c1, c2] = tr.AddSubBranches(10, left_branch, right_branch);
    auto new_ids = vector{c1, c2};
    auto new_ids_r = vector{c2, c1};
    BOOST_TEST(((tr.TipBranchesId() == new_ids) || (tr.TipBranchesId() == new_ids_r)));
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
    tr = Tree();
    tr.Initialize({{0.0, 0}}, {0.0}, {1});

    BOOST_TEST(!tr.IsValidBranchId(0));
    BOOST_TEST(tr.IsValidBranchId(1));
    BOOST_TEST(tr.GenerateNewID() == 2);
    tr.AddBranch(BranchNew(Point{0, 0}, 0), 2);
    BOOST_TEST(tr.GenerateNewID() == 3);
    tr.AddBranch(BranchNew(Point{0, 0}, 0), 3);
    BOOST_TEST(tr.GenerateNewID() == 4);
    tr.AddBranch(BranchNew(Point{0, 0}, 0), 4);
    BOOST_TEST(tr.GenerateNewID() == 5);

    BOOST_CHECK_THROW(tr.AddBranch(left_branch, 0), Exception);
}



void TEST_POINT(Point p1, Point p2)
{
    BOOST_TEST(p1 == p2);
}

BOOST_AUTO_TEST_CASE( boundary_generator_new, 
    *utf::tolerance(eps))
{   
    Border border;
    border.MakeRectangular(
        {1, 1},
        {0 ,1, 2, 3},
        {0.5},{1});

    Tree tr;
    tr.Initialize(
        border.GetSourcesPoint(), 
        border.GetSourcesNormalAngle(),
        border.GetSourcesId());
    
    vector<Point> tree_vector;
    TreeVector(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 1);
    auto p = Point{0.5, 0};
    BOOST_TEST(tree_vector.at(0) == p);

    auto br = tr.GetBranch(1);
    br->AddPoint(Polar{0.1, 0});
    tree_vector.clear();
    
    TreeVector(tree_vector, 1, tr, 1e-3);
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

    TreeVector(tree_vector, 1, tr, 1e-3);
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
    Border border;
    border.MakeRectangular(
        {1, 1},
        {0 ,1, 2, 3},
        {0.5},{1});

    Tree tr;
    tr.Initialize(
        border.GetSourcesPoint(), 
        border.GetSourcesNormalAngle(),
        border.GetSourcesId());
    
    vector<Point> tree_vector;
    TreeVector(tree_vector, 1, tr, 1e-3);
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
    TreeVector(tree_vector, 1, tr, 1e-3);
    BOOST_TEST(tree_vector.size() == 3);
    TEST_POINT(tree_vector.at(0), Point{0.5 - 1e-3/2 * sqrt(2)/2, -1e-3/2 * sqrt(2)/2});
    TEST_POINT(tree_vector.at(1), Point{0.5 - sqrt(2)/2*0.1, sqrt(2)/2*0.1});
    TEST_POINT(tree_vector.at(2), Point{0.5, 0});    
}


BOOST_AUTO_TEST_CASE( tree_tips_point_method, 
    *utf::tolerance(eps))
{
    Border border;
    border.MakeRectangular(
        {1, 1},
        {0 ,1, 2, 3},
        {0.5}, {1});

    Tree tr;
    tr.Initialize(
        border.GetSourcesPoint(), 
        border.GetSourcesNormalAngle(),
        border.GetSourcesId());

    BOOST_TEST(tr.TipPoints().size() == 1);
    TEST_POINT(tr.TipPoints().at(0), Point{0.5, 0});
}

BOOST_AUTO_TEST_CASE( add_points_tests, 
    *utf::tolerance(eps))
{
    Border border;
    border.MakeRectangular(
        {1, 1},
        {0 ,1, 2, 3},
        {0.5, 0.6, 0.7}, {1, 2, 3});

    Tree tr;
    tr.Initialize(
        border.GetSourcesPoint(), 
        border.GetSourcesNormalAngle(),
        border.GetSourcesId());

    tr.AddPoints({Point{0, 0.1}, Point{0, 0.1}, Point{0, 0.1}}, {1, 2, 3});
    TEST_POINT(tr.GetBranch(1)->TipPoint(), Point(0.5, 0.1));
    TEST_POINT(tr.GetBranch(2)->TipPoint(), Point(0.6, 0.1));
    TEST_POINT(tr.GetBranch(3)->TipPoint(), Point(0.7, 0.1));

    tr.AddPolars({Polar{0.1, 0}, Polar{0.1, 0}, Polar{0.1, 0}}, {1, 2, 3});
    TEST_POINT(tr.GetBranch(1)->TipPoint(), Point(0.5, 0.2));
    TEST_POINT(tr.GetBranch(2)->TipPoint(), Point(0.6, 0.2));
    TEST_POINT(tr.GetBranch(3)->TipPoint(), Point(0.7, 0.2));

    tr.AddAbsolutePolars({Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}}, {1, 2, 3});
    TEST_POINT(tr.GetBranch(1)->TipPoint(), Point(0.5, 0.3));
    TEST_POINT(tr.GetBranch(2)->TipPoint(), Point(0.6, 0.3));
    TEST_POINT(tr.GetBranch(3)->TipPoint(), Point(0.7, 0.3));

    tr.AddAbsolutePolars({Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}}, {1, 2, 3});
    TEST_POINT(tr.GetBranch(1)->TipPoint(), Point(0.5, 0.4));
    TEST_POINT(tr.GetBranch(2)->TipPoint(), Point(0.6, 0.4));
    TEST_POINT(tr.GetBranch(3)->TipPoint(), Point(0.7, 0.4));
}


BOOST_AUTO_TEST_CASE(shrink_test, *utf::tolerance(eps))
{
    BranchNew br{Point{0, 0}, 0};
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});

    BOOST_TEST(br.Lenght() == 4.);
    BOOST_TEST(br.Size() == 5);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 3.999);
    BOOST_TEST(br.Size() == 5);

    br.Shrink(0.009);
    BOOST_TEST(br.Lenght() == 3.990);
    BOOST_TEST(br.Size() == 5);

    br.Shrink(0.09);
    BOOST_TEST(br.Lenght() == 3.9);
    BOOST_TEST(br.Size() == 5);

    br.Shrink(0.9);
    BOOST_TEST(br.Lenght() == 3.);
    BOOST_TEST(br.Size() == 4);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 2.);
    BOOST_TEST(br.Size() == 3);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 1.);
    BOOST_TEST(br.Size() == 2);

    br.Shrink(0.999);
    BOOST_TEST(br.Lenght() == 0.001);
    BOOST_TEST(br.Size() == 2);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 0.);
    BOOST_TEST(br.Size() == 1);
}