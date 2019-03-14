//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "GemometryNew Classes"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "geometrynew.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( constructor_and_methods, 
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