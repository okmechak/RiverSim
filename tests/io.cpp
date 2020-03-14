//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "IO functions"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "io.hpp"

using namespace River;

const double eps = 1e-12;
namespace utf = boost::unit_test;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( io_methods, 
    *utf::tolerance(eps))
{
    Model mdl_out, mdl_in;

    mdl_out.dx = 1;
    mdl_out.width = 2;
    mdl_out.height = 3;
    mdl_out.river_boundary_id = 4;
    auto vec = vector<int>{3, 4, 5, 6};
    mdl_out.boundary_ids = vec;
    mdl_out.boundary_condition = 6;
    mdl_out.field_value = 7;
    mdl_out.eta = 8;
    mdl_out.bifurcation_type = 9;
    mdl_out.bifurcation_threshold = 10;
    mdl_out.bifurcation_threshold_2 = 11;
    mdl_out.bifurcation_min_dist = 12;
    mdl_out.bifurcation_angle = 13;
    mdl_out.growth_type = 14;
    mdl_out.growth_threshold = 15;
    mdl_out.growth_min_distance = 16;
    mdl_out.ds = 17;

    mdl_out.mesh.number_of_quadrangles = 18;
    mdl_out.mesh.number_of_refined_quadrangles = 19;
    mdl_out.mesh.refinment_radius = 20;
    mdl_out.mesh.exponant = 21;
    mdl_out.mesh.min_area = 22;
    mdl_out.mesh.max_area = 23;
    mdl_out.mesh.min_angle = 24;
    mdl_out.mesh.max_edge = 25;
    mdl_out.mesh.min_edge = 26;
    mdl_out.mesh.ratio = 27;
    mdl_out.mesh.eps = 28;
    mdl_out.mesh.sigma = 29;
    mdl_out.mesh.static_refinment_steps = 30;

    mdl_out.solver_params.quadrature_degree = 31;
    mdl_out.solver_params.adaptive_refinment_steps = 32;
    mdl_out.solver_params.tollerance = 33;
    mdl_out.solver_params.num_of_iterrations = 34;

    mdl_out.prog_opt.verbose = true;
    mdl_out.prog_opt.number_of_steps = 36;
    mdl_out.prog_opt.maximal_river_height = 37;
    mdl_out.prog_opt.number_of_backward_steps = 38;
    mdl_out.prog_opt.save_vtk = true;
    mdl_out.prog_opt.simulation_type = 40;


    //Border object setup.. Rectangular boundaries
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{2, 3};
    auto sources_x_coord = vector<double>{0.3};
    auto sources_id = vector<int>{1};
    
    
    mdl_out.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    //Tree object setup

    mdl_out.tree.Initialize(
        mdl_out.border.GetSourcesPoint(), 
        mdl_out.border.GetSourcesNormalAngle(), 
        mdl_out.border.GetSourcesId());
    
    mdl_out.geometry_difference.branches_bifuraction_info[1] = {{2, 3}, {3, 4}, {4, 5}, {5, 6}};
    mdl_out.geometry_difference.branches_series_params_and_geom_diff[1] = {{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};

    BranchNew br1left({0, 1}, 2), br1right({3, 4}, 5), br2left({6, 7}, 8), br2right({9, 10}, 11);
    br1left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br1right.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2right.AddAbsolutePoint(Polar{1, 0}).AddAbsolutePoint(Polar{1, 0});
    mdl_out.tree.AddSubBranches(1, br1left, br1right);
    mdl_out.tree.AddSubBranches(3, br2left, br2right);

    Save(mdl_out, "iotest");
    
    bool q = false;
    Open(mdl_in, "iotest.json", q);

    BOOST_TEST(q == true);
    //Model TEST

    BOOST_TEST(mdl_in.dx == 1);
    BOOST_TEST(mdl_in.width == 2);
    BOOST_TEST(mdl_in.height == 3);
    BOOST_TEST(mdl_in.river_boundary_id == 4);
    BOOST_TEST(mdl_in.boundary_ids == vec);
    BOOST_TEST(mdl_in.boundary_condition == 6);
    BOOST_TEST(mdl_in.field_value == 7);
    BOOST_TEST(mdl_in.eta == 8);
    BOOST_TEST(mdl_in.bifurcation_type == 9);
    BOOST_TEST(mdl_in.bifurcation_threshold == 10);
    BOOST_TEST(mdl_in.bifurcation_threshold_2 == 11);
    BOOST_TEST(mdl_in.bifurcation_min_dist == 12);
    BOOST_TEST(mdl_in.bifurcation_angle == 13);
    BOOST_TEST(mdl_in.growth_type == 14);
    BOOST_TEST(mdl_in.growth_threshold == 15);
    BOOST_TEST(mdl_in.growth_min_distance == 16);
    BOOST_TEST(mdl_in.ds == 17);
    //output values only
    //BOOST_TEST(mdl_in.mesh.number_of_quadrangles == 18);
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
    BOOST_TEST(mdl_in.mesh.static_refinment_steps == 30);
    BOOST_TEST(mdl_in.solver_params.quadrature_degree == 31);
    BOOST_TEST(mdl_in.solver_params.adaptive_refinment_steps == 32);
    BOOST_TEST(mdl_in.solver_params.tollerance == 33);
    BOOST_TEST(mdl_in.solver_params.num_of_iterrations == 34);
    BOOST_TEST(mdl_in.prog_opt.verbose == true);
    BOOST_TEST(mdl_in.prog_opt.number_of_steps == 36);
    BOOST_TEST(mdl_in.prog_opt.maximal_river_height == 37);
    BOOST_TEST(mdl_in.prog_opt.number_of_backward_steps == 38);
    BOOST_TEST(mdl_in.prog_opt.save_vtk == true);
    BOOST_TEST(mdl_in.prog_opt.simulation_type == 40);


    //Tree Test
    BOOST_TEST(mdl_in.tree.source_branches_id == sources_id);
    map<int, pair<int, int>> t = {{1, {2, 3}}, {3, {4, 5}}};
    BOOST_TEST(mdl_in.tree.branches_relation == t);
    BOOST_TEST(mdl_in.tree.GetBranch(5)->TipPoint().x == 11);
    BOOST_TEST(mdl_in.tree.GetBranch(5)->TipPoint().y == 10);
    //TODO

    //Border Test
    BOOST_TEST((mdl_in.border.vertices.front() == Point{2, 0}));
    BOOST_TEST((mdl_in.border.vertices.back() == Point{0.3, 0}));

    //GeometryDifference
    auto a1 = vector<vector<double>>{{2, 3}, {3, 4}, {4, 5}, {5, 6}},
        a2 = vector<vector<double>>{{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};

    BOOST_TEST(mdl_in.geometry_difference.branches_bifuraction_info[1] == a1);
    BOOST_TEST(mdl_in.geometry_difference.branches_series_params_and_geom_diff[1] == a2);
}