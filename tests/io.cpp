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

/*
BOOST_AUTO_TEST_CASE( files_io_methods, 
    *utf::tolerance(eps))
{
    Model mdl_out;

    //program options
    mdl_out.number_of_steps = 36;

    //mesh options
    mdl_out.mesh_params.refinment_radius = 20;
    mdl_out.mesh_params.exponant = 21;
    mdl_out.mesh_params.min_area = 22;
    mdl_out.mesh_params.max_area = 23;
    mdl_out.mesh_params.min_angle = 24;
    mdl_out.mesh_params.max_edge = 25;
    mdl_out.mesh_params.min_edge = 26;
    mdl_out.mesh_params.ratio = 27;
    mdl_out.mesh_params.sigma = 29;

    //integration options
    mdl_out.integr.weigth_func_radius = 10;
    mdl_out.integr.integration_radius = 9;
    mdl_out.integr.exponant = 3;

    //solver options
    mdl_out.solver_params.quadrature_degree = 31;
    mdl_out.solver_params.refinment_fraction = 10;
    mdl_out.solver_params.adaptive_refinment_steps = 32;
    mdl_out.solver_params.static_refinment_steps = 30;
    mdl_out.solver_params.tollerance = 33;
    mdl_out.solver_params.num_of_iterrations = 34;
    mdl_out.solver_params.field_value = 7;

    //model options
    mdl_out.dx = 1;
    mdl_out.width = 2;
    mdl_out.height = 3;
    mdl_out.eta = 8;
    mdl_out.bifurcation_type = 9;
    mdl_out.bifurcation_threshold = 10;
    mdl_out.bifurcation_min_dist = 12;
    mdl_out.bifurcation_angle = 13;
    mdl_out.growth_type = 14;
    mdl_out.growth_threshold = 15;
    mdl_out.growth_min_distance = 16;
    mdl_out.ds = 17;

    //Boundary conditions
    mdl_out.boundary_conditions[2] = {DIRICHLET, 10};
    mdl_out.boundary_conditions[3] = {NEUMAN, 11};

    //Boundary object setup.. Rectangular boundaries
    auto region_size = vector<double>{2, 3};
    auto sources_x_coord = vector<double>{0.3};
    auto sources_id = vector<t_source_id>{1};
    
    mdl_out.sources = mdl_out.region.MakeRectangular();
    
    //Rivers object setup
    mdl_out.rivers.Initialize(mdl_out.region.GetSourcesIdsPointsAndAngles(mdl_out.sources));

    Branch 
        br1left({0, 1}, 2), br1right({3, 4}, 5), 
        br2left({6, 7}, 8), br2right({9, 10}, 11);

    br1left.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br1right.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br2left.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br2right.AddAbsolutePoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddAbsolutePoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    mdl_out.tree.AddSubBranches(1, br1left, br1right);
    mdl_out.tree.AddSubBranches(3, br2left, br2right);

   

    BOOST_TEST_CHECKPOINT("Output");
    Save(mdl_out, "iotest");
    
    BOOST_TEST_CHECKPOINT("Input");

    Model mdl_in;
    Open(mdl_in);

    BOOST_TEST_CHECKPOINT("After Input");

    //program options
    BOOST_TEST(mdl_in.prog_opt.verbose == true);
    BOOST_TEST(mdl_in.prog_opt.debug == true);
    BOOST_TEST(mdl_in.number_of_steps == 36);
    BOOST_TEST(mdl_in.prog_opt.maximal_river_height == 37);
    BOOST_TEST(mdl_in.prog_opt.number_of_backward_steps == 38);
    BOOST_TEST(mdl_in.prog_opt.save_vtk == true);
    BOOST_TEST(mdl_in.prog_opt.simulation_type == 40);
    //BOOST_TEST(mdl_in.prog_opt.output_file_name == "simdata");
    BOOST_TEST(mdl_in.prog_opt.input_file_name == "kakaka");
    BOOST_TEST(mdl_in.prog_opt.save_each_step == true);

    //mesh options
    //BOOST_TEST(mdl_in.mesh.number_of_quadrangles == 18); these are only output fields
    //BOOST_TEST(mdl_in.mesh.number_of_refined_quadrangles == 19);
    BOOST_TEST(mdl_in.mesh.refinment_radius == 20);
    BOOST_TEST(mdl_in.mesh.exponant == 21);
    BOOST_TEST(mdl_in.mesh.min_area == 22);
    BOOST_TEST(mdl_in.mesh.max_area == 23);
    BOOST_TEST(mdl_in.mesh.min_angle == 24);
    BOOST_TEST(mdl_in.mesh.max_edge == 25);
    BOOST_TEST(mdl_in.mesh.min_edge == 26);
    BOOST_TEST(mdl_in.mesh.ratio == 27);
    BOOST_TEST(mdl_in.mesh.eps == 28);
    BOOST_TEST(mdl_in.mesh.sigma == 29);

    //integration options
    BOOST_TEST(mdl_in.integr.weigth_func_radius == 10);
    BOOST_TEST(mdl_in.integr.integration_radius == 9);
    BOOST_TEST(mdl_in.integr.exponant == 3);

    //solver parameters
    BOOST_TEST(mdl_in.solver_params.quadrature_degree == 31);
    BOOST_TEST(mdl_in.solver_params.refinment_fraction == 10);
    BOOST_TEST(mdl_in.solver_params.adaptive_refinment_steps == 32);
    BOOST_TEST(mdl_in.solver_params.static_refinment_steps == 30);
    BOOST_TEST(mdl_in.solver_params.tollerance == 33);
    BOOST_TEST(mdl_in.solver_params.num_of_iterrations == 34);
    BOOST_TEST(mdl_in.solver_params.max_distance == 99);
    BOOST_TEST(mdl_in.solver_params.field_value == 7);

    //model parameters
    BOOST_TEST(mdl_in.dx == 1);
    BOOST_TEST(mdl_in.width == 2);
    BOOST_TEST(mdl_in.height == 3);
    BOOST_TEST(mdl_in.river_boundary_id == 10);
    BOOST_TEST(mdl_in.eta == 8);
    BOOST_TEST(mdl_in.bifurcation_type == 9);
    BOOST_TEST(mdl_in.bifurcation_threshold == 10);
    BOOST_TEST(mdl_in.bifurcation_min_dist == 12);
    BOOST_TEST(mdl_in.bifurcation_angle == 13);
    BOOST_TEST(mdl_in.growth_type == 14);
    BOOST_TEST(mdl_in.growth_threshold == 15);
    BOOST_TEST(mdl_in.growth_min_distance == 16);
    BOOST_TEST(mdl_in.ds == 17);

    //Boundary Test
    //BOOST_TEST((mdl_in.border.vertices.front() == Point{2, 0}));
    //BOOST_TEST((mdl_in.border.vertices.back() == Point{0.3, 0}));

    //Rivers Test
    //BOOST_TEST(mdl_in.tree.source_branches_id == sources_id);
    map<t_branch_id, pair<t_branch_id, t_branch_id>> t = {{1, {2, 3}}, {3, {4, 5}}};
    BOOST_TEST(mdl_in.tree.branches_relation == t);
    BOOST_TEST(mdl_in.tree.at(5).TipPoint().x == 11.);
    BOOST_TEST(mdl_in.tree.at(5).TipPoint().y == 10.);
    BOOST_TEST(mdl_in.tree.at(1).TipPoint().x == 0.25);
    BOOST_TEST(mdl_in.tree.at(1).TipPoint().y == 0.);
    

    BOOST_TEST(mdl_out.border == mdl_in.border);
    BOOST_TEST(mdl_out.tree == mdl_in.tree);
    BOOST_TEST(mdl_out.sources == mdl_in.sources);
    BOOST_TEST(mdl_out.boundary_conditions == mdl_in.boundary_conditions);
}
*/

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