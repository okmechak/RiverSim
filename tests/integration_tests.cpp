//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "io.hpp"

using namespace River;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE( integration_params_test, 
    *utf::tolerance(EPS))
{   
    Model mdl;

    //1e-12 - is to small and we receive strange mesh
    mdl.mesh_params.min_area = 1e-9;
    mdl.mesh_params.max_area = 0.01;
    mdl.mesh_params.min_angle = 30;
    mdl.mesh_params.refinment_radius = 0.10;
    mdl.mesh_params.exponant = 2;
    mdl.dx = 0.25;
    mdl.InitializeDirichlet();

    mdl.rivers.at(1).AddPoint(Polar{0.1, 0}, (t_boundary_id)mdl.region_params.river_boundary_id);
    mdl.mesh_params.tip_points = {River::Point{0.25, 0.1}};

    auto boundary = BoundaryGenerator(mdl.sources, mdl.region, mdl.rivers, mdl.region_params);

    Triangle tria(mdl.mesh_params);
    t_PointList holes;
    auto mesh = tria.generate_quadrangular_mesh(boundary, holes);;

    //Simulation
    mdl.solver_params.adaptive_refinment_steps = 4;
    River::Solver sim(mdl.solver_params);
    sim.setBoundaryConditions(mdl.boundary_conditions);
    sim.OpenMesh(mesh);
    sim.run();
    
    auto tip_ids = mdl.rivers.TipBranchesIds();
    auto point = mdl.rivers.at(tip_ids.at(0)).TipPoint();
    auto angle = mdl.rivers.at(tip_ids.at(0)).TipAngle();
    mdl.integr.n_rho = 20;
    auto series_params = sim.integrate_new(mdl.integr, point, angle);

    BOOST_TEST(angle == M_PI/2);
    BOOST_TEST((point == River::Point{0.25, 0.1}));
    //Comparing to results given by Matlab program simulation
    BOOST_TEST(series_params.size() == 3);
    BOOST_TEST(series_params.at(0) == 0.101866);
    BOOST_TEST(series_params.at(1) == 0.0454618);
    BOOST_TEST(series_params.at(2) == 0.13087);
    sim.clear();
}