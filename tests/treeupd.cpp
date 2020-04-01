//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Tree Upd Tests"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "tree.hpp"
#include "border.hpp"

using namespace River;

const double eps = 1e-15;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Tree_UPD, 
    *utf::tolerance(eps))
{   
    Tree tree;
    BranchNew brTest({0, 0}, 0);

    BOOST_TEST(tree.NumberOfSourceBranches() == 0);
    BOOST_TEST(tree.TipBranchesId().size() == 0);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 0);
    BOOST_TEST(tree.TipPoints().size() == 0);
    BOOST_TEST(tree.HasEmptySourceBranch() == true);
    BOOST_TEST(tree.GenerateNewID() == 1);
    
    BOOST_CHECK_THROW(tree.AddSourceBranch(brTest, 0), Exception);
    BOOST_CHECK_THROW(tree.AddBranch(brTest, 0), Exception);
    BOOST_CHECK_THROW(tree.AddSubBranches(0, brTest, brTest), Exception);

    BOOST_CHECK_THROW(tree.GetParentBranchId(0), Exception);
    BOOST_CHECK_THROW(tree.GetParentBranchId(2), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranchesId(2), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranchId(2), Exception);

    BOOST_CHECK_THROW(tree.GetBranch(2), Exception);
    BOOST_CHECK_THROW(tree.GetParentBranch(2), Exception);
    BOOST_CHECK_THROW(tree.GetAdjacentBranch(2), Exception);
    BOOST_CHECK_THROW(tree.GetSubBranches(2), Exception);

    BOOST_CHECK_THROW(tree.DeleteSubBranches(5), Exception);
    BOOST_CHECK_THROW(tree.DeleteBranch(5), Exception);

    BOOST_CHECK_THROW(tree.AddPoints({Point{0, 0}, Point{1, 1}}, {1, 2}), Exception);
    BOOST_CHECK_THROW(tree.AddPolars({Polar{0, 0}, Polar{1, 1}}, {1, 2}), Exception);
    BOOST_CHECK_THROW(tree.AddAbsolutePolars({Polar{0, 0}, Polar{1, 1}}, {1, 2}), Exception);


    BOOST_TEST(tree.DoesExistBranch(1) == false);
    BOOST_CHECK_THROW(tree.IsSourceBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasParentBranch(1), Exception);
    BOOST_CHECK_THROW(tree.HasSubBranches(1), Exception);
    BOOST_TEST(tree.HasEmptySourceBranch() == true);

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
    auto id = tree.AddSourceBranch(br1);
    BOOST_TEST(id == 1);
    BOOST_TEST(tree.NumberOfSourceBranches() == 1);
    BOOST_TEST(tree.TipBranchesId().size() == 1);
    BOOST_TEST(tree.TipBranchesId().at(0) == id);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 1);
    BOOST_TEST(tree.TipPoints().size() == 1);
    BOOST_TEST(tree.TipPoints().at(0) == br1.TipPoint());
    BOOST_TEST(tree.HasEmptySourceBranch() == false);
    BOOST_TEST(tree.GenerateNewID() == 2);

    BOOST_TEST(tree.DoesExistBranch(id) == true);
    BOOST_TEST(tree.IsSourceBranch(id) == true);
    BOOST_TEST(tree.HasParentBranch(id) == false);
    BOOST_TEST(tree.HasSubBranches(id) == false);
    BOOST_TEST(tree.HasEmptySourceBranch() == false);


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

    BOOST_TEST(tree.TipBranchesId().size() == 2);
    BOOST_TEST((tree.TipBranchesId().at(0) == id2 ||
        tree.TipBranchesId().at(0) == id3));
    BOOST_TEST((tree.TipBranchesId().at(1) == id2 ||
        tree.TipBranchesId().at(1) == id3));


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


    BOOST_TEST((tree.GetSubBranchesId(id2) == pair<int, int>{id4, id5}));
    BOOST_TEST((tree.GetSubBranchesId(id3) == pair<int, int>{id6, id7}));
    BOOST_TEST(tree.GetAdjacentBranchId(id4) == id5);
    BOOST_TEST(tree.GetAdjacentBranchId(id5) == id4);
    BOOST_TEST(tree.GetAdjacentBranchId(id6) == id7);
    BOOST_TEST(tree.GetAdjacentBranchId(id7) == id6);
    BOOST_TEST(tree.GetParentBranchId(id6) == id3);
    BOOST_TEST(tree.GetParentBranchId(id7) == id3);
    BOOST_TEST(tree.GetParentBranchId(id4) == id2);
    BOOST_TEST(tree.GetParentBranchId(id5) == id2);


    BOOST_TEST(tree.NumberOfSourceBranches() == 1);
    BOOST_TEST(tree.TipBranchesId().size() == 4);
    BOOST_TEST(tree.TipIdsAndPoints().size() == 4);
    BOOST_TEST(tree.TipPoints().size() == 4);
    BOOST_TEST(tree.HasEmptySourceBranch() == false);
    BOOST_TEST((tree.GenerateNewID() == (unsigned int)(id7 + 1)));


    //delete
    tree.DeleteSubBranches(id3);
    BOOST_TEST(tree.DoesExistBranch(id6) == false);
    BOOST_TEST(tree.DoesExistBranch(id7) == false);
    BOOST_TEST(tree.IsSourceBranch(id3) == false);
    BOOST_TEST(tree.HasParentBranch(id3) == true);
    BOOST_TEST(tree.HasSubBranches(id3) == false);



    tree.DeleteSubBranches(id);
    BOOST_TEST(tree.DoesExistBranch(id4) == false);
    BOOST_TEST(tree.DoesExistBranch(id5) == false);
    BOOST_TEST(tree.DoesExistBranch(id3) == false);
    BOOST_TEST(tree.DoesExistBranch(id2) == false);
    BOOST_TEST(tree.HasSubBranches(id) == false);


}


// ------------- Tests Follow --------------
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

    auto id1 = tree.AddSourceBranch(br1);
    auto[id2, id3] = tree.AddSubBranches(id1, br2, br3);
    auto[id4, id5] = tree.AddSubBranches(id2, br4, br5);
    auto[id6, id7] = tree.AddSubBranches(id3, br6, br7);

    BOOST_TEST(tree.DoesExistBranch(id2) == true);
    BOOST_TEST(tree.DoesExistBranch(id3) == true);
    BOOST_TEST(tree.DoesExistBranch(id4) == true);
    BOOST_TEST(tree.DoesExistBranch(id5) == true);
    BOOST_TEST(tree.DoesExistBranch(id6) == true);
    BOOST_TEST(tree.DoesExistBranch(id7) == true);

    tree.DeleteSubBranches(id1);
    BOOST_TEST(tree.DoesExistBranch(id2) == false);
    BOOST_TEST(tree.DoesExistBranch(id3) == false);
    BOOST_TEST(tree.DoesExistBranch(id4) == false);
    BOOST_TEST(tree.DoesExistBranch(id5) == false);
    BOOST_TEST(tree.DoesExistBranch(id6) == false);
    BOOST_TEST(tree.DoesExistBranch(id7) == false);
}

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Tree_UPD_tips, 
    *utf::tolerance(eps))
{
    //TODO test tips: points, ids etc.. 
}
