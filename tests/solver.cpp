//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "solver.hpp"
#include "border.hpp"
#include "geometrynew.hpp"
#include "mesh.hpp"

using namespace River;

const double eps = 1e-13;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( integration_params_test, 
    *utf::tolerance(eps))
{   
    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.25};
    auto sources_id = vector<int>{1};

    Model mdl;
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    Tree tr(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
    tr.GetBranch(sources_id.at(0)).AddPoint(Polar{0.1, 0});

    auto mesh = BoundaryGenerator(mdl, tr, border, river_boundary_id);

    Triangle tria;
    tria.AreaConstrain = tria.ConstrainAngle = true;
    tria.MaxTriaArea = 0.01;
    tria.MinAngle = 30;
    tria.generate(mesh);
    mesh.convert();
    mesh.write("test.msh");

    //Simulation
    River::Solver sim;
    
    sim.numOfRefinments = 4;
    sim.SetBoundaryRegionValue(boundary_ids, 0.);
    sim.OpenMesh("test.msh");
    sim.run(0);

    
    auto tip_ids = tr.TipBranchesId();
    auto point = tr.GetBranch(tip_ids.at(0)).TipPoint();
    auto angle = tr.GetBranch(tip_ids.at(0)).TipAngle();
    auto series_params = sim.integrate(point, angle);

    //Comparing to results given by Matlab program simulation
    BOOST_TEST(series_params.size() == 3);
    BOOST_TEST(series_params.at(0) == 0.0250);
    BOOST_TEST(series_params.at(1) == 0.0114);
    BOOST_TEST(series_params.at(2) == 0.0325);
    sim.clear();
    BOOST_TEST_CHECKPOINT("lalal");
}