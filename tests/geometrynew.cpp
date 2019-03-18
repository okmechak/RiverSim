//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "GemometryNew Classes"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "geometrynew.hpp"

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

    BOOST_TEST(!br.Empty());
    BOOST_TEST(br.Size() == 1);
    BOOST_TEST(br.Lenght() == 0);
    BOOST_CHECK_THROW(br.AverageSpeed(), std::invalid_argument);
    BOOST_CHECK_THROW(br.TipVector(), std::invalid_argument);
    BOOST_TEST(br.TipPoint() == source_point);
    BOOST_TEST(br.TipAngle() == M_PI/2);
    BOOST_TEST(br.SourceAngle() == M_PI/2);
    BOOST_TEST(br.SourcePoint() == source_point );
    BOOST_CHECK_THROW(br.RemoveTipPoint(), std::invalid_argument);

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
    BOOST_CHECK_THROW(br.RemoveTipPoint(), invalid_argument);
    

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
    BOOST_CHECK_THROW(br.Vector(0), invalid_argument);

    br.AddPoint(Polar{1, 0});
    auto test_p = Point{1, 0};
    BOOST_TEST(br.Vector(0) == test_p);
    BOOST_CHECK_THROW(br.Vector(1), invalid_argument);

    br.AddPoint(Polar{1, M_PI/2});
    auto test_p_2 = Point{0, 1};
    BOOST_TEST(br.Vector(1) == test_p_2);
    BOOST_CHECK_THROW(br.Vector(2), invalid_argument);
}






BOOST_AUTO_TEST_CASE( Tree_Class, 
    *utf::tolerance(eps))
{   
    vector<int> ids{3, 4, 5};
    Tree tr(
        {{0.0, 0}, {0.1, 0}, {0.2, 0}}, 
        {0, 0, 0},
        ids);

    BOOST_TEST(tr.NumberOfSourceBranches() == 3);
    for(auto id: ids)
    {
        BOOST_TEST(tr.DoesExistBranch(id));
        BOOST_TEST(!tr.HasSubBranches(id));
        BOOST_TEST(!tr.IsSubBranch(id));
    }
    auto tips = tr.TipBranchesId();
    BOOST_TEST(tips == ids);


    //cool work with pointers.. but auto will force simple BranchNew
    BranchNew& br = tr.GetBranch(3);
    Point p{1, 1};
    br.AddPoint(p);
    BranchNew& br_new = tr.GetBranch(3);
    BOOST_TEST(br_new.Size() == 2);

    //addSources
    tr.AddSourceBranch(BranchNew{{0.3, 0}, 0}, 6);
    ids.push_back(6);
    BOOST_TEST(tr.TipBranchesId() == ids);
    BOOST_CHECK_THROW(tr.AddSourceBranch(BranchNew{{0.3, 0}, 0}, 6), invalid_argument);
    BOOST_CHECK_THROW(tr.IsSubBranch(1), invalid_argument);


    //addPoints
    //different size
    BOOST_CHECK_THROW(tr.AddPoints({{0, 0}, {0.1, 0}}, {1, 2, 3}), invalid_argument);
    
    auto test_point = Point{1, 1};
    auto tip_point = tr.GetBranch(3).TipPoint();

    BOOST_TEST(tr.AddPoints({test_point}, {3}).GetBranch(3).TipPoint() == (tip_point+test_point));

}




BOOST_AUTO_TEST_CASE( Tree_Class_methods, 
    *utf::tolerance(eps))
{   
    vector<int> ids{5, 4, 9};
    Tree tr(
        {{0.0, 0}, {0.1, 0}, {0.2, 0}}, 
        {0.0, 0.1, 0.2},
        ids);

    BranchNew 
        left_branch{tr.GetBranch(5).TipPoint(), 0.1},
        right_branch{tr.GetBranch(5).TipPoint(), -0.1};
    
    //ADD SUBBRANCHES TEST
    BOOST_TEST(!tr.HasSubBranches(5));         
    auto[b1, b2] = tr.AddSubBranches(5, left_branch, right_branch);
    BOOST_CHECK_THROW(tr.AddSubBranches(5, left_branch, right_branch), invalid_argument);
    BOOST_TEST(tr.HasSubBranches(5));
    BOOST_TEST(tr.DoesExistBranch(b1));
    BOOST_TEST(tr.DoesExistBranch(b2));

    //ADDBRANCH TEST
    BOOST_CHECK_THROW(tr.AddBranch(left_branch, b1), invalid_argument);
    BOOST_CHECK_THROW(tr.AddBranch(right_branch, b2), invalid_argument);

    ids = vector{10};
    tr = Tree{{{0.0, 0}}, {0.0}, ids};
    
    BOOST_TEST(tr.TipBranchesId() == ids);
    auto [c1, c2] = tr.AddSubBranches(10, left_branch, right_branch);
    auto new_ids = vector{c2, c1};
    BOOST_TEST(tr.TipBranchesId() == new_ids);
    BOOST_CHECK_THROW(tr.AddSubBranches(79, left_branch, right_branch), invalid_argument);
    BOOST_TEST(tr.IsSubBranch(c2));
    BOOST_TEST(tr.IsSubBranch(c1));

    //GETSOURCEBRANCH TEST
    BOOST_TEST(tr.GetSourceBranch(c1) == 10);
    BOOST_TEST(tr.GetSourceBranch(c2) == 10);

    BOOST_TEST(!tr.IsSubBranch(10));
    BOOST_TEST(tr.IsSubBranch(c1));
    BOOST_TEST(tr.IsSubBranch(c2));
    BOOST_CHECK_THROW(tr.GetSourceBranch(10), invalid_argument);
    BOOST_CHECK_THROW(tr.GetSourceBranch(79), invalid_argument);


    //GENERATE NEW ID
    BOOST_TEST(!tr.IsValidBranchId(0));
    BOOST_TEST(tr.IsValidBranchId(1));
    BOOST_TEST(tr.GenerateNewID(1, true) == 3);
    BOOST_TEST(tr.GenerateNewID(1, false) == 2);
    BOOST_CHECK_THROW(tr.GenerateNewID(0, false), invalid_argument);

    BOOST_CHECK_THROW(tr.AddBranch(left_branch, 0), invalid_argument);
}





BOOST_AUTO_TEST_CASE( boundary_generator_new, 
    *utf::tolerance(eps))
{   
    //Model mdl;
    //tethex::Mesh msh;
    //Border br(msh);
    //Tree tr(
    //    br.GetSourcesPoint(),
    //    br.GetSourcesNormalAngle(),
    //    br.GetSourcesId());
//
    //auto out_mesh = BoundaryGenerator(mdl, tr, br);
    //BOOST_TEST(out_mesh.get_n_vertices() == 0);
    //BOOST_TEST(out_mesh.get_n_points() == 0);
    //BOOST_TEST(out_mesh.get_n_lines() == 0);


}