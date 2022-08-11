//Link to Boost
//#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "IO functions"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "io.hpp"

using namespace River;
using namespace std;

const double eps = EPS;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( Check_For_Boundary_Intersection_debug, 
    *utf::tolerance(eps))
{
    Model model;
    Open(model, "big_simulation_with_intersection.json");

    auto region_boundary = BoundaryGenerator(model.sources, model.region, model.rivers, model.region_params);
    auto tip_boundary = model.rivers.TipBoundary();

    auto num_of_intersects = NumOfBoundaryIntersection(region_boundary, tip_boundary);
    BOOST_TEST(num_of_intersects == 2);
}


BOOST_AUTO_TEST_CASE( model_io, 
    *utf::tolerance(eps))
{
    Model model_out;

    model_out.InitializeDirichletWithHole();

    model_out.bifurcation_angle = 1;
    model_out.bifurcation_min_dist = 2;
    model_out.bifurcation_threshold = 3;
    model_out.bifurcation_type = 4;
    model_out.ds = 5;
    model_out.dx = 6;
    model_out.eta = 7;
    model_out.growth_min_distance = 8;
    model_out.growth_threshold = 9;
    model_out.growth_type = 10;
    model_out.height = 11;
    model_out.number_of_steps = 12;
    model_out.width = 13;
    
    model_out.integr.eps = 14;
    model_out.integr.exponant = 15;
    model_out.integr.integration_radius = 16;
    model_out.integr.n_rho = 17;
    model_out.integr.weigth_func_radius = 18;

    model_out.mesh_params.exponant = 19;
    model_out.mesh_params.max_area = 20;
    model_out.mesh_params.max_edge = 21;
    model_out.mesh_params.min_angle = 22;
    model_out.mesh_params.min_area = 23;
    model_out.mesh_params.min_edge = 24;
    model_out.mesh_params.ratio = 25;
    model_out.mesh_params.refinment_radius = 26;
    model_out.mesh_params.sigma = 27;

    model_out.solver_params.adaptive_refinment_steps = 28;
    model_out.solver_params.field_value = 29;
    model_out.solver_params.num_of_iterrations = 30;
    model_out.solver_params.quadrature_degree = 31;
    model_out.solver_params.refinment_fraction = 32;
    model_out.solver_params.renumbering_type = 33;
    model_out.solver_params.static_refinment_steps = 34;
    model_out.solver_params.tollerance = 35;

    model_out.region_params.ignored_smoothness_length = 36;
    model_out.region_params.river_boundary_id = 37;
    model_out.region_params.river_width = 39;
    model_out.region_params.smoothness_degree = 40;

    Save(model_out, "model_out.json");

    Model model_in;
    Open(model_in, "model_out.json");

    BOOST_TEST((model_out == model_in));
}


BOOST_AUTO_TEST_CASE( point_to_json, 
    *utf::tolerance(eps))
{
    River::Point p {1, 2};

    json j = p;

    auto p2 = j.get<River::Point>();

    BOOST_TEST((p == p2));
}

BOOST_AUTO_TEST_CASE( polar_to_json, 
    *utf::tolerance(eps))
{
    Polar p {1, 2};

    json j = p;

    auto p2 = j.get<Polar>();

    BOOST_TEST((p == p2));
}


BOOST_AUTO_TEST_CASE( BoundaryCondition_to_json, 
    *utf::tolerance(eps))
{
    BoundaryCondition bc;

    bc.type = NEUMAN;
    bc.value = -90;

    json j = bc;

    auto bcj = j.get<BoundaryCondition>();

    BOOST_TEST((bc == bcj));
}


BOOST_AUTO_TEST_CASE( BoundaryConditions_to_json, 
    *utf::tolerance(eps))
{
    BoundaryConditions bc;

    bc[1].type = NEUMAN;
    bc[1].value = -90;
    bc[3].type = DIRICHLET;
    bc[3].value = 90;

    json j = bc;

    auto bcj = j.get<BoundaryConditions>();

    BOOST_TEST((bc == bcj));
}

BOOST_AUTO_TEST_CASE( Sources_to_json, 
    *utf::tolerance(eps))
{
    Sources sr;

    sr[1] = {2, 3};
    sr[3] = {4, 5};

    json j = sr;

    auto srj = j.get<Sources>();

    BOOST_TEST((sr == srj));
}

BOOST_AUTO_TEST_CASE( Line_to_json, 
    *utf::tolerance(eps))
{
    Line line(1, 2, 10);

    json j = line;

    auto linej = j.get<Line>();

    BOOST_TEST((line == linej));
}

BOOST_AUTO_TEST_CASE( Boundary_to_json, 
    *utf::tolerance(eps))
{
    Boundary boundary;

    boundary.vertices = {{1, 1}, {2, 2}};
    boundary.lines = {{1, 2, 3}, {2, 3, 4}};

    json j = boundary;

    auto boundaryj = j.get<Boundary>();

    BOOST_TEST((boundary == boundaryj));
}

BOOST_AUTO_TEST_CASE( RegionParams_to_json, 
    *utf::tolerance(eps))
{
    RegionParams rp;
    rp.ignored_smoothness_length = 10;
    rp.smoothness_degree = 11;
    rp.river_boundary_id = 101;
    rp.river_width = 2;

    json j = rp;
    
    auto rpj = j.get<RegionParams>();

    BOOST_TEST((rpj == rp));
}

BOOST_AUTO_TEST_CASE( Region_to_json, 
    *utf::tolerance(eps))
{
    Region region;

    region.MakeRectangularWithHole();

    json j = region;

    auto regionj = j.get<Region>();

    BOOST_TEST((region == regionj));
}

//Rivers
//Branch
BOOST_AUTO_TEST_CASE( Branch_to_json, 
    *utf::tolerance(eps))
{

    Branch branch({0, 1}, M_PI/3.);
    branch.AddPoint(Polar{1, 1}, (t_boundary_id)0);
    branch.AddPoint(Polar{2, 6}, (t_boundary_id)1);
    branch.AddPoint(Polar{3, 7}, (t_boundary_id)2);
    branch.AddPoint(Polar{4, 8}, (t_boundary_id)3);
    branch.AddPoint(Polar{5, 9}, (t_boundary_id)4);

    json j = branch;

    auto branchj = j.get<Branch>();

    BOOST_TEST((branchj == branch));
}

//Rivers
BOOST_AUTO_TEST_CASE( Rivers_to_json, 
    *utf::tolerance(eps))
{
    Branch branch({0, 1}, M_PI/3.);
    Rivers rivers;
    auto branch_id = rivers.AddBranch(branch);
    t_boundary_id boundary_id = 1;
    rivers.GrowTestTree(boundary_id, branch_id, 0.01, 5, 0.001);

    json j = rivers;

    auto riversj = j.get<Rivers>();

    BOOST_TEST((rivers == riversj));
}

//PhysModel

//MeshParams
BOOST_AUTO_TEST_CASE( MeshParams_to_json, 
    *utf::tolerance(eps))
{
    MeshParams data;
    data.refinment_radius = 10;
    data.exponant = 11;
    data.sigma = 12;
    data.min_area = 13;
    data.max_area = 14;
    data.min_angle = 15;
    data.max_edge = 16;
    data.min_edge = 17;
    data.ratio = 18;
    
    json j = data;

    auto dataj = j.get<MeshParams>();

    BOOST_TEST((dataj == data));
}

//IntegrationParams
BOOST_AUTO_TEST_CASE( IntegrationParams_to_json, 
    *utf::tolerance(eps))
{
    IntegrationParams data;
    data.weigth_func_radius = 10;
    data.integration_radius = 11;
    data.exponant = 12;
    data.eps = 13;
    data.n_rho = 14;

    json j = data;

    auto dataj = j.get<IntegrationParams>();

    BOOST_TEST((dataj == data));
}

//SolverParams
BOOST_AUTO_TEST_CASE( SolverParams_to_json, 
    *utf::tolerance(eps))
{
    SolverParams data;

    data.field_value = 5;
    data.tollerance = 10;
    data.num_of_iterrations = 11;
    data.adaptive_refinment_steps = 12;
    data.static_refinment_steps = 49;
    data.refinment_fraction = 13;
    data.quadrature_degree = 14;
    data.renumbering_type = 15;
    
    json j = data;

    auto dataj = j.get<SolverParams>();

    BOOST_TEST((dataj == data));
}

//Model
BOOST_AUTO_TEST_CASE( Model_to_json, 
    *utf::tolerance(eps))
{
    Model data;

    data.InitializeDirichletWithHole();
    data.number_of_steps = 243;
    data.dx = 1;
    data.width = 2;
    data.height = 3;
    data.ds = 15;
    data.eta = 6;
    data.bifurcation_type = 7;
    data.bifurcation_threshold = 8;
    data.bifurcation_min_dist = 10;
    data.bifurcation_angle = 11;
    data.growth_type = 12;
    data.growth_threshold = 13;
    data.growth_min_distance = 14;

    json j = data;

    auto dataj = j.get<Model>();

    BOOST_TEST((dataj == data));
}