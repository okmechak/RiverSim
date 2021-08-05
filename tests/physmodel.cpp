//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Physical Model Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "river.hpp"

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
    model.bifurcation_type = 1;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 6) == false);

    model.bifurcation_type = 1;
    model.bifurcation_threshold = 3;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 11) == true);

    model.bifurcation_threshold = 2.999;
    BOOST_TEST(model.q_bifurcate({1, 2, 3}, 11) == false);
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
    BOOST_TEST(series_parameters.empty());

    map<t_branch_id, vector<double>> id_series_params;
    id_series_params[1] = {1., 2., 3.};
    id_series_params[2] = {4., 5., 6.};
    id_series_params[3] = {7., 8., 9.};
    
    series_parameters.record(id_series_params);
    BOOST_TEST((series_parameters.at(1).at(0) == vector<double>{1.}));
    BOOST_TEST((series_parameters.at(2).at(0) == vector<double>{4.}));
    BOOST_TEST((series_parameters.at(3).at(0) == vector<double>{7.}));

    BOOST_TEST((series_parameters.at(1).at(1) == vector<double>{2.}));
    BOOST_TEST((series_parameters.at(2).at(1) == vector<double>{5.}));
    BOOST_TEST((series_parameters.at(3).at(1) == vector<double>{8.}));

    BOOST_TEST((series_parameters.at(1).at(2) == vector<double>{3.}));
    BOOST_TEST((series_parameters.at(2).at(2) == vector<double>{6.}));
    BOOST_TEST((series_parameters.at(3).at(2) == vector<double>{9.}));

    series_parameters.record(id_series_params);
    BOOST_TEST((series_parameters.at(1).at(0) == vector<double>{1., 1.}));
    BOOST_TEST((series_parameters.at(2).at(0) == vector<double>{4., 4.}));
    BOOST_TEST((series_parameters.at(3).at(0) == vector<double>{7., 7.}));

    BOOST_TEST((series_parameters.at(1).at(1) == vector<double>{2., 2.}));
    BOOST_TEST((series_parameters.at(2).at(1) == vector<double>{5., 5.}));
    BOOST_TEST((series_parameters.at(3).at(1) == vector<double>{8., 8.}));

    BOOST_TEST((series_parameters.at(1).at(2) == vector<double>{3., 3.}));
    BOOST_TEST((series_parameters.at(2).at(2) == vector<double>{6., 6.}));
    BOOST_TEST((series_parameters.at(3).at(2) == vector<double>{9., 9.}));
}

BOOST_AUTO_TEST_CASE( revert_simulation, 
    *utf::tolerance(eps))
{
    Model model;
    model.InitializeLaplace();
    model.tree.GrowTestTree(1, 1., 2, 0.);
    model.series_parameters[1] = {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}};
    model.series_parameters[2] = {{4, 4, 4}, {5, 5, 5}, {6, 6, 6}};
    model.series_parameters[3] = {{7, 7, 7}, {8, 8, 8}, {9, 9, 9}};

    t_SeriesParameters series_parameters = 
    {
        {1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}}},
        {2, {{4, 4, 4}, {5, 5, 5}, {6, 6, 6}}},
        {3, {{7, 7, 7}, {8, 8, 8}, {9, 9, 9}}}
    };
    
    auto sim_data = SimulationData{
        {"mesh", {1., 2., 3., 4., 5., 6.}},
        {"cash", {0.1, 0.2, 0.3, 0.4, 0.5, 0.6}}
    };

    model.sim_data = sim_data;

    model.RevertLastSimulationStep();

    sim_data = SimulationData 
    {
        {"mesh", {1., 2., 3., 4., 5.}},
        {"cash", {0.1, 0.2, 0.3, 0.4, 0.5}}
    };
    BOOST_TEST(model.sim_data == sim_data);

    series_parameters = 
    {
        {1, {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}}},
        {2, {{4, 4, 4}, {5, 5, 5}}},
        {3, {{7, 7, 7}, {8, 8, 8}}}
    };
    BOOST_TEST((t_SeriesParameters)model.series_parameters == series_parameters);
}

BOOST_AUTO_TEST_CASE( BackwardData_equal, 
    *utf::tolerance(eps))
{
    BackwardData data1, data2;

    data1.a1 = {1, 2, 3};
    data1.a2 = {3, 4, 5};
    data1.a3 = {7, 8, 9};

    data1.init = {{1, 2}};
    data1.backward = {{2, 3}};
    data1.backward_forward = {{4, 5}};

    data1.branch_lenght_diff = 9;

    BOOST_TEST(!(data1 == data2));

    data2.a1 = {1, 2, 3};
    data2.a2 = {3, 4, 5};
    data2.a3 = {7, 8, 9};

    data2.init = {{1, 2}};
    data2.backward = {{2, 3}};
    data2.backward_forward = {{4, 5}};

    data2.branch_lenght_diff = 9;

    BOOST_TEST(data1 == data2);

}

BOOST_AUTO_TEST_CASE( ProgramOptions_equal, 
    *utf::tolerance(eps))
{
    ProgramOptions data1, data2;

    BOOST_TEST(data1 == data2);

    data1.simulation_type = 10;
    data1.number_of_steps = 11;
    data1.maximal_river_height = 12;
    data1.number_of_backward_steps = 13;
    data1.save_vtk = true;
    data1.save_each_step = true;
    data1.verbose = true;
    data1.debug = true;
    data1.output_file_name = "lalalala";

    BOOST_TEST(!(data1 == data2));

    data2.simulation_type = 10;
    data2.number_of_steps = 11;
    data2.maximal_river_height = 12;
    data2.number_of_backward_steps = 13;
    data2.save_vtk = true;
    data2.save_each_step = true;
    data2.verbose = true;
    data2.debug = true;
    data2.output_file_name = "lalalala";
    BOOST_TEST(data1 == data2);
}

BOOST_AUTO_TEST_CASE( MeshParams_equal, 
    *utf::tolerance(eps))
{
    MeshParams data1, data2;
    
    BOOST_TEST(data1 == data2);

    data1.refinment_radius = 10;
    data1.exponant = 11;
    data1.sigma = 12;
    data1.min_area = 13;
    data1.max_area = 14;
    data1.min_angle = 15;
    data1.max_edge = 16;
    data1.min_edge = 17;
    data1.ratio = 18;
    data1.eps = 19;

    BOOST_TEST(!(data1 == data2));

    data2.refinment_radius = 10;
    data2.exponant = 11;
    data2.sigma = 12;
    data2.min_area = 13;
    data2.max_area = 14;
    data2.min_angle = 15;
    data2.max_edge = 16;
    data2.min_edge = 17;
    data2.ratio = 18;
    data2.eps = 19;

    BOOST_TEST(data1 == data2);
}

BOOST_AUTO_TEST_CASE( IntegrationParams_equal, 
    *utf::tolerance(eps))
{
    IntegrationParams data1, data2;

    BOOST_TEST(data1 == data2);

    data1.weigth_func_radius = 10;
    data1.integration_radius = 11;
    data1.exponant = 12;

    BOOST_TEST(!(data1 == data2));

    data2.weigth_func_radius = 10;
    data2.integration_radius = 11;
    data2.exponant = 12;

    BOOST_TEST(data1 == data2);
}

BOOST_AUTO_TEST_CASE( SolverParams_equal, 
    *utf::tolerance(eps))
{
    SolverParams data1, data2;

    BOOST_TEST(data1 == data2);

    data1.tollerance = 10;
    data1.num_of_iterrations = 11;
    data1.adaptive_refinment_steps = 12;
    data1.refinment_fraction = 13;
    data1.quadrature_degree = true;
    data1.renumbering_type = true;
    data1.max_distance = true;

    BOOST_TEST(!(data1 == data2));

    data2.tollerance = 10;
    data2.num_of_iterrations = 11;
    data2.adaptive_refinment_steps = 12;
    data2.refinment_fraction = 13;
    data2.quadrature_degree = true;
    data2.renumbering_type = true;
    data2.max_distance = true;

    BOOST_TEST(data1 == data2);
}

BOOST_AUTO_TEST_CASE( PhysModel_equal, 
    *utf::tolerance(eps))
{
    Model data1, data2;

    BOOST_TEST(data1 == data2);

    data1.InitializeDirichletWithHole();
    data1.dx = 1;
    data1.width = 2;
    data1.height = 3;
    data1.river_boundary_id = 4;
    data1.field_value = 5;
    data1.eta = 6;
    data1.bifurcation_type = 7;
    data1.bifurcation_threshold = 8;
    data1.bifurcation_min_dist = 10;
    data1.bifurcation_angle = 11;
    data1.growth_type = 12;
    data1.growth_threshold = 13;
    data1.growth_min_distance = 14;
    data1.ds = 15;

    BOOST_TEST(!(data1 == data2));

    data2.InitializeDirichletWithHole();
    data2.dx = 1;
    data2.width = 2;
    data2.height = 3;
    data2.river_boundary_id = 4;
    data2.field_value = 5;
    data2.eta = 6;
    data2.bifurcation_type = 7;
    data2.bifurcation_threshold = 8;
    data2.bifurcation_min_dist = 10;
    data2.bifurcation_angle = 11;
    data2.growth_type = 12;
    data2.growth_threshold = 13;
    data2.growth_min_distance = 14;
    data2.ds = 15;

    BOOST_TEST(data1 == data2);
}