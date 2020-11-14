//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Tree and Branch"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "branch.hpp"

using namespace River;

const double eps = 1e-15;
namespace utf = boost::unit_test;

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