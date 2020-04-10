//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Tree and Branch"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "tree.hpp"

using namespace River;

const double eps = 1e-15;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE( BranchNew_Class, 
    *utf::tolerance(eps))
{   
    //constructor: empty branch
    auto source_point = Point{0, 0};
    auto source_angle = M_PI/2;
    auto br = BranchNew(source_point, source_angle);

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
    br = BranchNew(source_point, M_PI/6);
    BOOST_CHECK_NO_THROW(br.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0}));
    Point test_p_1{sqrt(3), 1};
    BOOST_TEST(br.TipPoint() == test_p_1);
    BOOST_CHECK_NO_THROW(br.AddAbsolutePoint(Polar{2, 5./6.* M_PI}));
    Point test_p_2{0, 2};
    BOOST_TEST(br.TipPoint() == test_p_2);

    //now let make circle in clockwise direction:
    source_point = Point{0, 0};
    br = BranchNew(source_point, M_PI/6);
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

    br = BranchNew(source_point, 0);
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

BOOST_AUTO_TEST_CASE( BranchNew_vector, 
    *utf::tolerance(eps))
{   
    BranchNew br{Point{0, 0}, 0};
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

BOOST_AUTO_TEST_CASE( Tree_Class, 
    *utf::tolerance(eps))
{   
    vector<t_source_id> ids{3, 4, 5};
    Tree tr;
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

    auto& br = tr.at(3);
    Point p{1, 1};
    br.AddPoint(p);
    auto& br_new = tr.at(3);
    BOOST_TEST(br_new.size() == 2);

    //addSources
    tr.AddBranch(BranchNew{{0.3, 0}, 0}, 6);
    ids.push_back(6);
    BOOST_TEST(tr.TipBranchesIds() == ids);
    BOOST_CHECK_THROW(tr.AddBranch(BranchNew{{0.3, 0}, 0}, 6), Exception);
    BOOST_CHECK_THROW(tr.HasParentBranch(1), Exception);


    //addPoints
    //different size
    BOOST_CHECK_THROW(tr.AddPoints({1, 2, 3}, {{0, 0}, {0.1, 0}}), Exception);
    
    auto test_point = Point{1, 1};
    auto tip_point = tr.at(3).TipPoint();
    
    tr.AddPoints({3}, {test_point});
    BOOST_TEST(tr.at(3).TipPoint() == (tip_point + test_point));
}

BOOST_AUTO_TEST_CASE( Tree_Class_methods, 
    *utf::tolerance(eps))
{   
    vector<t_source_id> ids{5, 4, 9};
    Tree tr;
    tr.Initialize({
        {5, {{0.0, 0}, 0}},
        {4, {{0.1, 0}, 0.1}},
        {9, {{0.2, 0}, 0.2}}
    });

    BranchNew 
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
    tr = Tree();
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
    tr = Tree();
    tr.Initialize({{1, {{0.0, 0}, 0}}});

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

BOOST_AUTO_TEST_CASE( tree_tips_point_method, 
    *utf::tolerance(eps))
{
    Boundaries border;
    border[1]=
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {0.5, 0}, 
                {1, 0}, 
                {1, 1}, 
                {0, 1}
            }, 
            {/*lines*/
                {0, 1, 0},
                {1, 2, 0},
                {2, 3, 1},
                {3, 4, 2},
                {4, 0, 3} 
            },
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/

    Sources sources;
    sources[1] = {1, 1};

    Tree tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));

    BOOST_TEST(tr.TipPoints().size() == 1);
    BOOST_TEST((tr.TipPoints().at(0) == Point{0.5, 0}));
}

BOOST_AUTO_TEST_CASE( add_points_tests, 
    *utf::tolerance(eps))
{
    Boundaries border;
    border[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {0.5, 0},
                {0.6, 0},
                {0.7, 0}, 
                {1, 0}, 
                {1, 1}, 
                {0, 1}
            }, 
            {/*lines*/
                {0, 1, 0},
                {1, 2, 0},
                {2, 3, 0},
                {3, 4, 0},
                {4, 5, 1}, 
                {6, 7, 2}, 
                {7, 0, 3} 
            }, 
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/
        
    Sources sources;
    sources[1] = {1, 1};
    sources[2] = {1, 2};
    sources[3] = {1, 3};

    Tree tr;
    tr.Initialize(
        border.GetSourcesIdsPointsAndAngles(sources));

    tr.AddPoints({1, 2, 3}, {Point{0, 0.1}, Point{0, 0.1}, Point{0, 0.1}});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.1)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.1)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.1)));

    tr.AddPolars({1, 2, 3}, {Polar{0.1, 0}, Polar{0.1, 0}, Polar{0.1, 0}});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.2)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.2)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.2)));

    tr.AddAbsolutePolars({1, 2, 3}, {Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.3)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.3)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.3)));

    tr.AddAbsolutePolars({1, 2, 3}, {Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}, Polar{0.1, M_PI/2}});
    BOOST_TEST((tr.at(1).TipPoint() == Point(0.5, 0.4)));
    BOOST_TEST((tr.at(2).TipPoint() == Point(0.6, 0.4)));
    BOOST_TEST((tr.at(3).TipPoint() == Point(0.7, 0.4)));
}

BOOST_AUTO_TEST_CASE(shrink_test, *utf::tolerance(eps))
{
    BranchNew br{Point{0, 0}, 0};
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});
    br.AddPoint(Polar{1, 0});

    BOOST_TEST(br.Lenght() == 4.);
    BOOST_TEST(br.size() == 5);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 3.999);
    BOOST_TEST(br.size() == 5);

    br.Shrink(0.009);
    BOOST_TEST(br.Lenght() == 3.990);
    BOOST_TEST(br.size() == 5);

    br.Shrink(0.09);
    BOOST_TEST(br.Lenght() == 3.9);
    BOOST_TEST(br.size() == 5);

    br.Shrink(0.9);
    BOOST_TEST(br.Lenght() == 3.);
    BOOST_TEST(br.size() == 4);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 2.);
    BOOST_TEST(br.size() == 3);

    br.Shrink(1);
    BOOST_TEST(br.Lenght() == 1.);
    BOOST_TEST(br.size() == 2);

    br.Shrink(0.999);
    BOOST_TEST(br.Lenght() == 0.001);
    BOOST_TEST(br.size() == 2);

    br.Shrink(0.001);
    BOOST_TEST(br.Lenght() == 0.);
    BOOST_TEST(br.size() == 1);
}


// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Tree_UPD, 
    *utf::tolerance(eps))
{   
    Tree tree;
    BranchNew brTest({0, 0}, 0);

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

    BOOST_CHECK_THROW(tree.AddPoints({1, 2}, {Point{0, 0}, Point{1, 1}}), Exception);
    BOOST_CHECK_THROW(tree.AddAbsolutePolars({1, 2}, {Polar{0, 0}, Polar{1, 1}}), Exception);

    BOOST_TEST(tree.count(1) == false);
    BOOST_CHECK_THROW(tree.IsSourceBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasSubBranches(1), Exception);

    BOOST_TEST(tree.IsValidBranchId(0) == false);
    BOOST_TEST(tree.IsValidBranchId(1) == true);

    BranchNew br1({0, 0}, 1.4);
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});

    BranchNew br2({0, 0}, 1.7);
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});

    BranchNew br3({0, 0}, 1.9);
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});

    BranchNew br4({0, 0}, 2.4);
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});

    BranchNew br5({0, 0}, 4.4);
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});

    BranchNew br6({0, 0}, 0.4);
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});

    BranchNew br7({0, 0}, 0.6);
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});

    //building tree
    auto id = tree.AddBranch(br1);
    BOOST_TEST(id == 1);
    BOOST_TEST(tree.TipBranchesIds().size() == 1);
    BOOST_TEST(tree.TipBranchesIds().at(0) == id);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 1);
    BOOST_TEST(tree.TipPoints().size() == 1);
    BOOST_TEST(tree.TipPoints().at(0) == br1.TipPoint());
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
    Tree tree;
    BranchNew br1({0, 0}, 1.4);
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});
    br1.AddPoint(Polar{1, 0});

    BranchNew br2({0, 0}, 1.7);
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});
    br2.AddPoint(Polar{1, 0});

    BranchNew br3({0, 0}, 1.9);
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});
    br3.AddPoint(Polar{1, 0});

    BranchNew br4({0, 0}, 2.4);
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});
    br4.AddPoint(Polar{1, 0});

    BranchNew br5({0, 0}, 4.4);
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});
    br5.AddPoint(Polar{1, 0});

    BranchNew br6({0, 0}, 0.4);
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});
    br6.AddPoint(Polar{1, 0});

    BranchNew br7({0, 0}, 0.6);
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});
    br7.AddPoint(Polar{1, 0});

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
    Tree tree;
    BOOST_CHECK_THROW(tree.GetParentBranchId(1), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranchesIds(1), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranchId(1), Exception);
    BOOST_CHECK_THROW(tree.at(1), std::exception);
    BOOST_CHECK_THROW(tree.GetParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranches(1), Exception);
    BOOST_CHECK_THROW(tree.DeleteSubBranches(1), Exception);
    BOOST_CHECK_THROW(tree.DeleteBranch(1), Exception);
    BOOST_CHECK_THROW(tree.AddPoints({1, 2}, {{1, 1}, {2, 2}}), Exception);
    BOOST_CHECK_THROW(tree.AddPolars( {1, 2}, {{1, 1}, {2, 2}}), Exception);
    BOOST_CHECK_THROW(tree.AddAbsolutePolars({1, 2}, {{1, 1}, {2, 2}}), Exception);
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
    Tree tree_original;
    tree_original.Initialize({{1, {{0., 0.}, 10}}});

    Tree tree_copy{tree_original}, tree_not_copy;

    BOOST_TEST(tree_original == tree_copy);
    BOOST_TEST(!(tree_original == tree_not_copy));
    BOOST_TEST(tree_original.TipBranchesIds() == tree_copy.TipBranchesIds());
    BOOST_TEST(tree_original.TipIdsAndPoints() == tree_copy.TipIdsAndPoints());
}

BOOST_AUTO_TEST_CASE( tree_initializtion_and_clear, 
    *utf::tolerance(eps))
{
    Boundaries::trees_interface_t ids_points_angles = 
    {
        {1, {{2, 3}, 4}},
        {3, {{3, 4}, 5}},
        {5, {{4, 5}, 6}},
        {7, {{5, 6}, 7}}};
    auto source_branches_id = vector<t_source_id>{1, 3, 5, 7};
    auto tip_points = vector<Point>{{2, 3}, {3, 4}, {4, 5}, {5, 6}};
    auto tip_ids_and_points = 
        map<t_source_id, Point>{{1, {2, 3}}, {3, {3, 4}}, {5, {4, 5}}, {7, {5, 6}}};

    Tree tree;
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

BOOST_AUTO_TEST_CASE( tree_add_and_delete_branch, 
    *utf::tolerance(eps))
{
    Tree tree;
    BranchNew 
        branch_1{Point{1, 2}, 3}, 
        branch_2{Point{2, 3}, 4},
        branch_3{Point{3, 4}, 5},
        branch_1_duplicate{Point{1, 2}, 3};

    BOOST_CHECK_THROW(tree.AddBranch(branch_1, 0), Exception);
    t_branch_id id;
    BOOST_CHECK_NO_THROW(id = tree.AddBranch(branch_1));
    BOOST_CHECK_THROW(tree.AddBranch(branch_1, id), Exception);

    //.....
}

BOOST_AUTO_TEST_CASE( test_branch_growth, 
    *utf::tolerance(eps))
{
    Boundaries::trees_interface_t ids_points_angles = {
        {1, {{2, 3}, 0}},
        {3, {{3, 4},   M_PI/4.}},
        {5, {{4, 5},   M_PI/2.}},
        {7, {{5, 6}, 3*M_PI/4.}}
    };

    Tree tree;
    tree.Initialize(ids_points_angles);
    {
        auto ds = 0.05,
            dalpha = 0.0;
        auto n = 3;
        auto id = 1;
        auto[left_id, right_id] = tree.GrowTestTree(id, ds, n, dalpha);

        auto& br = tree.at(id), 
            &br_left = tree.at(left_id),
            &br_right = tree.at(right_id);

        auto br_vetices = vector<Point>{
            {2, 3}, 
            {2 +   ds, 3}, 
            {2 + 2*ds, 3}, 
            {2 + 3*ds, 3}};
        BOOST_TEST(br.at(0) == br_vetices.at(0));
        BOOST_TEST(br.at(1) == br_vetices.at(1));
        BOOST_TEST(br.at(2) == br_vetices.at(2));
        BOOST_TEST(br.at(3) == br_vetices.at(3));

        auto br_vetices_left = vector<Point>{
            {2, 3}, 
            {2 +   ds/sqrt(2.), 3 + ds/sqrt(2.)}, 
            {2 + 2*ds/sqrt(2.), 3 + 2*ds/sqrt(2.)}, 
            {2 + 3*ds/sqrt(2.), 3 + 3*ds/sqrt(2.)}};
        BOOST_TEST(br_left.at(0) == br_vetices_left.at(0));
        BOOST_TEST(br_left.at(1) == br_vetices_left.at(1));
        BOOST_TEST(br_left.at(2) == br_vetices_left.at(2));
        BOOST_TEST(br_left.at(3) == br_vetices_left.at(3));

        auto br_vetices_right = vector<Point>{
            {2, 3}, 
            {2 +   ds/sqrt(2.), 3 - ds/sqrt(2.)}, 
            {2 + 2*ds/sqrt(2.), 3 - 2*ds/sqrt(2.)}, 
            {2 + 3*ds/sqrt(2.), 3 - 3*ds/sqrt(2.)}};
        BOOST_TEST(br_right.at(0) == br_vetices_right.at(0));
        BOOST_TEST(br_right.at(1) == br_vetices_right.at(1));
        BOOST_TEST(br_right.at(2) == br_vetices_right.at(2));
        BOOST_TEST(br_right.at(3) == br_vetices_right.at(3));
    }

    //lets try different branch with different id
    {
        auto id = 5;
        auto ds = 0.1,
            dalpha = 0.0;
        auto n = 2;
        auto[left_id, right_id] = tree.GrowTestTree(id, ds, n, dalpha);
        
        auto& br = tree.at(id), 
            &br_left = tree.at(left_id),
            &br_right = tree.at(right_id);
        auto br_vetices = vector<Point>{
            {4, 5}, 
            {4, 5 +   ds}, 
            {4, 5 + 2*ds}};
        BOOST_TEST(br.at(0) == br_vetices.at(0));
        BOOST_TEST(br.at(1) == br_vetices.at(1));
        BOOST_TEST(br.at(2) == br_vetices.at(2));
    
        auto br_vetices_left = vector<Point>{
            {4,                 5}, 
            {4 -   ds/sqrt(2.), 5 + ds/sqrt(2.)}, 
            {4 - 2*ds/sqrt(2.), 5 + 2*ds/sqrt(2.)}};
        BOOST_TEST(br_left.at(0) == br_vetices_left.at(0));
        BOOST_TEST(br_left.at(1) == br_vetices_left.at(1));
        BOOST_TEST(br_left.at(2) == br_vetices_left.at(2));
    
        auto br_vetices_right = vector<Point>{
            {4,                 5}, 
            {4 +   ds/sqrt(2.), 5 + ds/sqrt(2.)}, 
            {4 + 2*ds/sqrt(2.), 5 + 2*ds/sqrt(2.)}};
        BOOST_TEST(br_right.at(0) == br_vetices_right.at(0));
        BOOST_TEST(br_right.at(1) == br_vetices_right.at(1));
        BOOST_TEST(br_right.at(2) == br_vetices_right.at(2));
    }
}