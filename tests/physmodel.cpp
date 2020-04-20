//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Physical Model Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "physmodel.hpp"

using namespace River;

const double eps = 1e-12;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( phys_model_methods, 
    *utf::tolerance(eps))
{
    River::Model mdl;

    BOOST_TEST(mdl.integr.BaseVector(1, 1) == 1.);
    BOOST_TEST(mdl.integr.BaseVector(1, 2) == sqrt(2.));
    BOOST_TEST(mdl.integr.BaseVector(2, 1) == 0.);
    BOOST_TEST(mdl.integr.BaseVector(2, complex<double>(0., 1.)) == -1.);
    BOOST_TEST(mdl.integr.BaseVector(3, 1) == 1.);
    BOOST_TEST(mdl.integr.BaseVector(3, complex<double>(0., 2.)) == -2.);
}

BOOST_AUTO_TEST_CASE( next_point_method, 
    *utf::tolerance(eps))
{
    Model mdl;
    mdl.ds = 0.003;
    auto p = mdl.next_point({0.0233587, 0.0117224, 0.621602}, 0, 1/*maximal a series param*/);
    BOOST_TEST(p.r == pow(mdl.ds, mdl.eta));
    BOOST_TEST(p.phi ==-0.054918865933649114);
}

BOOST_AUTO_TEST_CASE( mesh_params, 
    *utf::tolerance(eps))
{
    MeshParams mesh_params;
    mesh_params.tip_points = {{0., 0.}, {100, 100}};
    mesh_params.exponant = 100;
    mesh_params.sigma = 1;
    mesh_params.min_area = 0.1;
    mesh_params.max_area = 10;
    mesh_params.refinment_radius = 1;

    BOOST_TEST(mesh_params(0, 0) == mesh_params.min_area);
    BOOST_TEST(mesh_params(10, 10) == mesh_params.max_area);
    BOOST_TEST(mesh_params(100, 100) == mesh_params.min_area);
}

BOOST_AUTO_TEST_CASE( q_bifurcate, 
    *utf::tolerance(eps))
{
    Model model;
    model.bifurcation_min_dist = 10;
    model.bifurcation_type = 0;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 6) == false);

    model.bifurcation_type = 0;
    model.bifurcation_threshold = 3;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 11) == true);

    model.bifurcation_threshold = 2.999;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 11) == false);

    model.bifurcation_type = 1;
    model.bifurcation_threshold_2 = 10;
    BOOST_TEST(model.q_bifurcate({10, 2, 3}, 11) == true);

    model.bifurcation_threshold_2 = 10;
    BOOST_TEST(model.q_bifurcate({9.99, 2, 3}, 11) == false);
}

BOOST_AUTO_TEST_CASE( q_growth, 
    *utf::tolerance(eps))
{
    Model model;
    model.growth_threshold = 1;
    BOOST_TEST(model.q_growth({1, 2, 3}) == true);

    model.growth_threshold = 1.00001;
    BOOST_TEST(model.q_growth({1, 2, 3}) == false);
}

BOOST_AUTO_TEST_CASE( next_point, 
    *utf::tolerance(eps))
{
    Model model;
    model.ds = 0.1;
    model.eta = 1;
    model.growth_min_distance = 0;

    BOOST_TEST((model.next_point({1, 0, 1}, 0, 1) == Polar{model.ds, 0}));

    model.eta = 2;
    model.growth_min_distance = 1;
    BOOST_TEST((model.next_point({1, 0, 1}, 0, 1) == Polar{model.ds, 0}));
    BOOST_TEST((model.next_point({0.5, 0, 1}, 0, 1) == Polar{model.ds, 0}));

    BOOST_TEST((model.next_point({1, 0, 1}, 1, 1) == Polar{model.ds, 0}));
    BOOST_TEST((model.next_point({0.5, 0, 1}, 1, 1) == Polar{model.ds*0.25, 0}));
}

BOOST_AUTO_TEST_CASE( Model_Simulation_Data, 
    *utf::tolerance(eps))
{
    SeriesParameters series_parameters;
    BOOST_TEST(model_sim_data.empty());

    map<t_branch_id, vector<double>> id_series_params;
    id_series_params[1] = {1., 2., 3.};
    id_series_params[2] = {4., 5., 6.};
    id_series_params[3] = {7., 8., 9.};
    
    series_parameters.record(id_series_params);
    BOOST_TEST((series_parameters.at(1).at(0) == vector<double>{1., 2., 3.}));
    BOOST_TEST((series_parameters.at(2).at(0) == vector<double>{4., 5., 6.}));
    BOOST_TEST((series_parameters.at(3).at(0) == vector<double>{7., 8., 9.}));

    series_parameters.record(id_series_params);
    BOOST_TEST((series_parameters.at(1).at(1) == vector<double>{1., 2., 3.}));
    BOOST_TEST((series_parameters.at(2).at(1) == vector<double>{4., 5., 6.}));
    BOOST_TEST((series_parameters.at(3).at(1) == vector<double>{7., 8., 9.}));

}