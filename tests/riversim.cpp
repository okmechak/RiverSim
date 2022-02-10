//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "riversim.hpp"
   
using namespace River;

const double eps = 1e-10;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Triangulate_Boundaries, 
    *utf::tolerance(eps)
    *utf::description("when model is empty"))
{
    Model model;
    Triangle triangle(&model.mesh);
    BOOST_CHECK_THROW(TriangulateBoundaries(model, triangle, "test"), Exception);
}

BOOST_AUTO_TEST_CASE( Solve_PDE, 
    *utf::tolerance(eps)
    *utf::description("if there is no msh file throw error"))
{
    Model model;
    River::Solver sim(model.solver_params, model.boundary_conditions, true);
    BOOST_CHECK_THROW(SolvePDE(model, sim, "test_unique"), Exception);
}

BOOST_AUTO_TEST_CASE( Evaluate_Series_Parameteres_Of_Tips, 
    *utf::tolerance(1e-2)
    *utf::description("normal case"))
{
    Model model;
    model.dx = 0.25;
    model.mesh.min_area = 1e-6;
    model.mesh.max_area = 0.01;
    model.mesh.refinment_radius = 0.25;
    model.mesh.exponant = 4;
    model.InitializeDirichlet();
    
    model.tree.at(1).AddPoint(Polar{0.1, 0}, (t_boundary_id)model.river_boundary_id);
    model.mesh.tip_points = {River::Point{0.25, 0.1}};

    //Triangle mesh object setup
    Triangle triangle(&model.mesh);

    //Simulation object setup
    River::Solver sim(model.solver_params, model.boundary_conditions, true);

    TriangulateBoundaries(model, triangle, "test");
    SolvePDE(model, sim, "test");

    auto id_series_params_map = EvaluateSeriesParameteresOfTips(model, sim);

    BOOST_TEST(id_series_params_map.count(1));
    BOOST_TEST(id_series_params_map.at(1).size() == 3);
    BOOST_TEST(id_series_params_map.at(1).at(0) == 0.101866);
    BOOST_TEST(id_series_params_map.at(1).at(1) == 0.0454618);
    BOOST_TEST(id_series_params_map.at(1).at(2) == 0.13087);
}

BOOST_AUTO_TEST_CASE( Evaluate_Series_Parameteres_Of_Tips_2, 
    *utf::tolerance(eps)
    *utf::description("case when solver wasn't invoked"))
{
    Model model;
    model.dx = 0.25;
    model.InitializeDirichlet();
    model.tree.at(1).AddPoint(Polar{0.1, 0}, (t_boundary_id)model.river_boundary_id);

    //Triangle mesh object setup
    Triangle triangle(&model.mesh);

    //Simulation object setup
    River::Solver sim(model.solver_params, model.boundary_conditions, true);

    TriangulateBoundaries(model, triangle, "test");

    BOOST_CHECK_THROW(EvaluateSeriesParameteresOfTips(model, sim), Exception);
}

BOOST_AUTO_TEST_CASE( Maximal_A1_Value, 
    *utf::tolerance(eps)
    *utf::description("edge test and simple max value"))
{
    BOOST_TEST(MaximalA1Value({}) == 0);

    map<t_branch_id, vector<double>> id_series_params;
    id_series_params[1] = {1,  2, 3};
    id_series_params[2] = {10, 2, 3};
    id_series_params[3] = {2,  2, 3};
    id_series_params[4] = {9,  2, 3};

    BOOST_TEST(MaximalA1Value(id_series_params) == 10.);
}

BOOST_AUTO_TEST_CASE( Grow_Tree, 
    *utf::tolerance(eps)
    *utf::description("edge test and simple max value"))
{
    //if everything is empty
    Model model;
    model.ds = 0.1;
    model.bifurcation_type = 1;
    model.bifurcation_threshold = -1;
    model.eta = 0;
    map<t_branch_id, vector<double>> id_series_params;
    BOOST_CHECK_THROW(GrowTree(model, id_series_params, 0), Exception);

    auto source_branch = BranchNew{{0, 0}, M_PI/2.};
    auto id = model.tree.AddBranch(source_branch);
    id_series_params[id] = {1, 0, 3};

    BOOST_CHECK_NO_THROW(GrowTree(model, id_series_params, 1));

    auto expected_tip_point = River::Point{0, 0.1};
    BOOST_TEST(model.tree.TipIdsAndPoints().at(id) == expected_tip_point);

    BOOST_CHECK_NO_THROW(GrowTree(model, id_series_params, 1));

    auto expected_tip_point_2 = River::Point{0, 0.2};
    BOOST_TEST(model.tree.TipIdsAndPoints().at(id) == expected_tip_point_2);
}

BOOST_AUTO_TEST_CASE( Grow_Tree_biffurcation, 
    *utf::tolerance(eps)
    *utf::description("test growth tree with bifurcation"))
{
    //if everything is empty
    Model model;
    model.ds = 0.1;
    model.bifurcation_type = 1;
    model.bifurcation_threshold = 1;
    model.eta = 1;
    model.growth_type = 0;
    model.growth_threshold = 0;
    model.bifurcation_min_dist = 0;

    auto source_branch = BranchNew{{0, 0}, 0};
    auto id = model.tree.AddBranch(source_branch);
    map<t_branch_id, vector<double>> id_series_params;
    id_series_params[id] = {1, 0, 0};

    BOOST_CHECK_NO_THROW(GrowTree(model, id_series_params, 1));
    auto tips_points = model.tree.TipIdsAndPoints();

    BOOST_TEST(model.tree.HasSubBranches(id));

    auto[left_id, right_id] = model.tree.GetSubBranchesIds(id);
    BOOST_TEST(model.tree.at(left_id).TipAngle() == model.bifurcation_angle);
    BOOST_TEST(model.tree.at(right_id).TipAngle() == -model.bifurcation_angle);
}

BOOST_AUTO_TEST_CASE( Shrink_Tree, 
    *utf::tolerance(eps)
    *utf::description("edge test and simple max value"))
{
    Model model;
    map<t_branch_id, vector<double>> id_series_params;

    //Empty test
    BOOST_CHECK_THROW(ShrinkTree(model, id_series_params, 0), Exception);

    model.ds = 0.1;
    model.dx = 0.5;
    model.eta = 1;
    model.growth_type = 0;
    model.growth_min_distance = 0;
    model.growth_threshold = 0;
    model.InitializeLaplace();
    unsigned branch_id = 1, n = 4;
    model.tree.GrowTestTree(model.river_boundary_id, branch_id, model.ds, n);
    BOOST_TEST((model.tree.at(branch_id).TipPoint() == River::Point{model.dx, model.ds*n}));

    auto[left_id, right_id] = model.tree.GetSubBranchesIds(branch_id);
    
    id_series_params[left_id] = {1, 0, 0};
    id_series_params[right_id] = {2, 0, 0};

    auto 
        expected_p_left = River::Point{model.dx, model.ds*n} + Polar{model.ds*n, 3./4.*M_PI},
        expected_p_right = River::Point{model.dx, model.ds*n} + Polar{model.ds*n, 1./4.*M_PI};

    BOOST_TEST(model.tree.at(left_id ).TipPoint() == expected_p_left);
    BOOST_TEST(model.tree.at(right_id).TipPoint() == expected_p_right);

    BOOST_TEST(model.next_point({1, 0, 0}, 0, 2).r == model.ds/2);
    BOOST_TEST(model.next_point({2, 0, 0}, 0, 2).r == model.ds);

    auto branch_shrink_test = model.tree.at(right_id);
    BOOST_TEST(branch_shrink_test.Lenght() == model.ds*n);
    BOOST_CHECK_NO_THROW(branch_shrink_test.Shrink(model.ds));
    BOOST_TEST(branch_shrink_test.Lenght() = model.ds*n - model.dx);

    BOOST_CHECK_NO_THROW(ShrinkTree(model, id_series_params, 2));
    
     
    expected_p_left  = River::Point{model.dx, model.ds*n} + Polar{model.ds*n - model.ds/2., 3./4.*M_PI};
    expected_p_right = River::Point{model.dx, model.ds*n} + Polar{model.ds*n - model.ds, 1./4.*M_PI};

    BOOST_TEST(model.tree.at(left_id).TipPoint() == expected_p_left);
    BOOST_TEST(model.tree.at(right_id).TipPoint() == expected_p_right);
    BOOST_TEST(branch_shrink_test.TipPoint() == expected_p_right);

    BOOST_CHECK_NO_THROW(ShrinkTree(model, id_series_params, 2));

    expected_p_left  = River::Point{
            model.dx - model.ds*(n - 1)/sqrt(2), 
            model.ds*n + model.ds*(n - 1)/sqrt(2)},
        expected_p_right = River::Point{
            model.dx + model.ds*(n - 2)/sqrt(2), 
            model.ds*n + model.ds*(n - 2)/sqrt(2)};

    BOOST_TEST(model.tree.at(left_id).TipPoint() == expected_p_left);
    BOOST_TEST(model.tree.at(right_id).TipPoint() == expected_p_right);
}

BOOST_AUTO_TEST_CASE( Forward_River_Evolution, 
    *utf::tolerance(1e-2)
    *utf::description("symmetrical growth"))
{
    Model model;
    model.dx = 0.5;
    model.height = 5;
    model.width = 1;
    model.ds = 0.1;
    model.mesh.min_area = 1e-5;
    model.mesh.max_area = 0.01;
    model.mesh.refinment_radius = 0.25;
    model.mesh.exponant = 3;
    model.InitializeLaplace();

    //Triangle mesh object setup
    Triangle triangle(&model.mesh);

    //Simulation object setup
    River::Solver sim(model.solver_params, model.boundary_conditions, true);

    ForwardRiverEvolution(model, triangle, sim, "Forward_River_Evolution", -1);

    auto expected_point = River::Point{model.dx, model.ds};
    BOOST_TEST((model.tree.TipPoints().at(0) - expected_point).norm() < 1e-3);

    ForwardRiverEvolution(model, triangle, sim, "Forward_River_Evolution", -1);

    expected_point = River::Point{model.dx, 2*model.ds};
    BOOST_TEST((model.tree.TipPoints().at(0) - expected_point).norm() < 1e-3);

    ForwardRiverEvolution(model, triangle, sim, "Forward_River_Evolution", -1);

    expected_point = River::Point{model.dx, 3*model.ds};
    BOOST_TEST((model.tree.TipPoints().at(0) - expected_point).norm() < 1e-3);
}