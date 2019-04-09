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
    *utf::tolerance(1e-1))
{   
    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.25};
    auto sources_id = vector<int>{1};

    Model mdl;
    //FIXME
    AreaConstraint ac;
    //1e-12 - is to small and we receive strange mesh
    ac.min_area = 1e-10;
    ac.r0 = 0.25;
    ac.exponant = 4;
    ac.tip_points = {River::Point{0.25, 0.1}};
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.river_boundary_id = river_boundary_id;
    border.eps = mdl.eps;
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    Tree tr;
    tr.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
    tr.GetBranch(sources_id.at(0)).AddPoint(Polar{0.1, 0});

    auto mesh = BoundaryGenerator(mdl, tr, border);

    Triangle tria;
    tria.ConstrainAngle = tria.CustomConstraint = true;
    tria.MinAngle = 30;
    tria.AreaConstrain = true;
    tria.MaxTriaArea = 0.01;
    tria.generate(mesh, &ac);
    mesh.convert();
    mesh.write("test.msh");

    //Simulation
    River::Solver sim;
    
    sim.numOfRefinments = 1;
    sim.field_value = 1;
    sim.SetBoundaryRegionValue(boundary_ids, 0.);
    sim.OpenMesh("test.msh");
    sim.run(0);
    
    auto tip_ids = tr.TipBranchesId();
    auto point = tr.GetBranch(tip_ids.at(0)).TipPoint();
    auto angle = tr.GetBranch(tip_ids.at(0)).TipAngle();
    auto series_params = sim.integrate(point, angle);

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
    *utf::tolerance(1e-1))
{
    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.25};
    auto sources_id = vector<int>{1};

    Model mdl;
    //FIXME
    AreaConstraint ac;
    //1e-12 - is to small and we receive strange mesh
    ac.min_area = 1e-10;
    ac.r0 = 0.25;
    ac.exponant = 4;
    ac.tip_points = {River::Point{0.25, 0.1}};
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.river_boundary_id = river_boundary_id;
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    Tree tr;
    tr.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
    tr.GetBranch(sources_id.at(0)).AddPoint(Polar{0.1, 0});

    auto mesh = BoundaryGenerator(mdl, tr, border);

    Triangle tria;
    tria.AreaConstrain = tria.ConstrainAngle = tria.CustomConstraint = true;
    tria.MaxTriaArea = 0.1;
    tria.MinAngle = 30;
    tria.generate(mesh, &ac);
    mesh.convert();
    mesh.write("test.msh");

    //Simulation
    River::Solver sim;
    
    sim.numOfRefinments = 1;
    sim.SetBoundaryRegionValue(boundary_ids, 0.);
    sim.OpenMesh("test.msh");
    sim.run(0);
    sim.field_value = 1;
    
    auto tip_ids = tr.TipBranchesId();
    auto point = tr.GetBranch(tip_ids.at(0)).TipPoint();
    auto dr = 0.01;
    auto integration = sim.integration_test(point, dr);
    auto integration_of_whole_region = sim.integration_test(point, 10);
    auto max_value = sim.max_value();

    //Comparing to result given by Mathematica program
    //see for notebook Testing.nb in results folder
    BOOST_TEST(Model::Rmax == 0.01);
    BOOST_TEST(integration == 0.00000162911);

    BOOST_TEST(integration_of_whole_region == 0.03420202360857102);
    BOOST_TEST(max_value == 0.07257921834603551);
}