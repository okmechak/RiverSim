//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "solver.hpp"
#include "border.hpp"
#include "tree.hpp"
#include "mesh.hpp"

using namespace River;

const double eps = 1e-4;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( integration_params_test, 
    *utf::tolerance(1e-1)*utf::disabled())
{   
    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.25};
    auto sources_id = vector<int>{1};

    Model mdl;

    //1e-12 - is to small and we receive strange mesh
    mdl.mesh.min_area = 1e-10;
    mdl.mesh.max_area = 0.01;
    mdl.mesh.min_angle = 30;
    mdl.mesh.refinment_radius = 0.25;
    mdl.mesh.exponant = 4;
    mdl.mesh.tip_points = {River::Point{0.25, 0.1}};
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    mdl.tree.Initialize(mdl.border.GetSourcesPoint(), mdl.border.GetSourcesNormalAngle(), mdl.border.GetSourcesId());
    mdl.tree.GetBranch(sources_id.at(0))->AddPoint(Polar{0.1, 0});

    auto mesh = BoundaryGenerator(mdl);

    Triangle tria(&mdl.mesh);
    tria.generate(mesh);
    mesh.convert();
    mesh.write("test.msh");

    //Simulation
    mdl.field_value = 1;
    River::Solver sim(&mdl);
    sim.SetBoundaryRegionValue(boundary_ids, 0.);
    sim.OpenMesh("test.msh");
    sim.run();
    
    auto tip_ids = mdl.tree.TipBranchesId();
    auto point = mdl.tree.GetBranch(tip_ids.at(0))->TipPoint();
    auto angle = mdl.tree.GetBranch(tip_ids.at(0))->TipAngle();
    auto series_params = sim.integrate(mdl, point, angle);

    BOOST_TEST(angle == M_PI/2);
    BOOST_TEST((point == River::Point{0.25, 0.1}));
    //Comparing to results given by Matlab program simulation
    BOOST_TEST(series_params.size() == 3);
    BOOST_TEST(series_params.at(0) == 0.101866);
    BOOST_TEST(series_params.at(1) == 0.0454618);
    BOOST_TEST(series_params.at(2) == 0.13087);
    sim.clear();
}





BOOST_AUTO_TEST_CASE( integration_test, 
    *utf::tolerance(1e-1)*utf::disabled())
{
    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.25};
    auto sources_id = vector<int>{1};

    Model mdl;
    //1e-12 - is to small and we receive strange mesh
    mdl.mesh.min_area = 1e-10;
    mdl.mesh.max_area = 0.01;
    mdl.mesh.min_angle = 30;
    mdl.mesh.refinment_radius = 0.25;
    mdl.mesh.exponant = 4;
    mdl.mesh.tip_points = {River::Point{0.25, 0.1}};
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    mdl.tree.Initialize(mdl.border.GetSourcesPoint(), mdl.border.GetSourcesNormalAngle(), mdl.border.GetSourcesId());
    mdl.tree.GetBranch(sources_id.at(0))->AddPoint(Polar{0.1, 0});

    auto mesh = BoundaryGenerator(mdl);

    Triangle tria(&mdl.mesh);
    tria.generate(mesh);
    mesh.convert();
    mesh.write("test.msh");

    //Simulation
    River::Solver sim(&mdl);
    
    sim.SetBoundaryRegionValue(boundary_ids, 0.);
    sim.OpenMesh("test.msh");
    sim.run();
    sim.field_value = 1;
    
    auto tip_ids = mdl.tree.TipBranchesId();
    auto point = mdl.tree.GetBranch(tip_ids.at(0))->TipPoint();
    auto dr = 0.01;
    auto integration = sim.integration_test(point, dr);
    auto integration_of_whole_region = sim.integration_test(point, 10);
    auto max_value = sim.max_value();

    //Comparing to result given by Mathematica program
    //see for notebook Testing.nb in results folder
    BOOST_TEST(mdl.integr.integration_radius == 0.01);
    BOOST_TEST(integration == 0.00000162911);

    BOOST_TEST(integration_of_whole_region == 0.03420202360857102);
    BOOST_TEST(max_value == 0.07257921834603551);
}


BOOST_AUTO_TEST_CASE( memory_leak, 
    *utf::tolerance(1e-1))
{   
    Model model;
    for(unsigned long int i = 0; i < 1e4; ++i)
        River::Solver sim(&model);
}