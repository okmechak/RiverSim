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
    Border border_out, border_in;
    Tree tree_out, tree_in;
    GeometryDifference gd_out, gd_in;
    Timing time_out, time_in;

    //program options
    mdl_out.prog_opt.verbose = true;
    mdl_out.prog_opt.number_of_steps = 36;
    mdl_out.prog_opt.maximal_river_height = 37;
    mdl_out.prog_opt.number_of_backward_steps = 38;
    mdl_out.prog_opt.save_vtk = true;
    mdl_out.prog_opt.simulation_type = 40;
    mdl_out.prog_opt.output_file_name = "lalala";
    mdl_out.prog_opt.input_file_name = "kakaka";

    //mesh options
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

    //integration options
    mdl_out.integr.weigth_func_radius = 10;
    mdl_out.integr.integration_radius = 9;
    mdl_out.integr.exponant = 3;

    //solver options
    mdl_out.solver_params.quadrature_degree = 31;
    mdl_out.solver_params.refinment_fraction = 10;
    mdl_out.solver_params.adaptive_refinment_steps = 32;
    mdl_out.solver_params.tollerance = 33;
    mdl_out.solver_params.num_of_iterrations = 34;

    //model options
    mdl_out.dx = 1;
    mdl_out.width = 2;
    mdl_out.height = 3;
    mdl_out.river_boundary_id = 10;
    auto vec = vector<int>{3, 4, 5, mdl_out.river_boundary_id};
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

    //Border object setup.. Rectangular boundaries
    auto boundary_ids = vec;
    auto region_size = vector<double>{2, 3};
    auto sources_x_coord = vector<double>{0.3};
    auto sources_id = vector<int>{1};
    
    border_out.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    
    //Tree object setup
    tree_out.Initialize(
        border_out.GetSourcesPoint(), 
        border_out.GetSourcesNormalAngle(), 
        border_out.GetSourcesId());

    BranchNew 
        br1left({0, 1}, 2), br1right({3, 4}, 5), 
        br2left({6, 7}, 8), br2right({9, 10}, 11);

    br1left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br1right.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2right.AddAbsolutePoint(Polar{1, 0}).AddAbsolutePoint(Polar{1, 0});
    tree_out.AddSubBranches(1, br1left, br1right);
    tree_out.AddSubBranches(3, br2left, br2right);

    //Geometry difference object
    gd_out.branches_bifuraction_info[1] = {{2, 3}, {3, 4}, {4, 5}, {5, 6}};
    gd_out.branches_series_params_and_geom_diff[1] = {{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};


    Save(mdl_out, time_out, border_out, tree_out, gd_out, "iotest");
    
    bool q = false;
    Open(mdl_in, border_in, tree_in, gd_in, "iotest.json", q);


    BOOST_TEST(q == true);

    //program options
    BOOST_TEST(mdl_in.prog_opt.verbose == true);
    BOOST_TEST(mdl_in.prog_opt.number_of_steps == 36);
    BOOST_TEST(mdl_in.prog_opt.maximal_river_height == 37);
    BOOST_TEST(mdl_in.prog_opt.number_of_backward_steps == 38);
    BOOST_TEST(mdl_in.prog_opt.save_vtk == true);
    BOOST_TEST(mdl_in.prog_opt.simulation_type == 40);
    BOOST_TEST(mdl_in.prog_opt.output_file_name == "lalala");
    BOOST_TEST(mdl_in.prog_opt.input_file_name == "kakaka");

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
    BOOST_TEST(mdl_in.mesh.static_refinment_steps == 30);

    //integration options
    BOOST_TEST(mdl_in.integr.weigth_func_radius == 10);
    BOOST_TEST(mdl_in.integr.integration_radius == 9);
    BOOST_TEST(mdl_in.integr.exponant == 3);

    //solver parameters
    BOOST_TEST(mdl_in.solver_params.quadrature_degree = 31);
    BOOST_TEST(mdl_in.solver_params.refinment_fraction = 10);
    BOOST_TEST(mdl_in.solver_params.adaptive_refinment_steps = 32);
    BOOST_TEST(mdl_in.solver_params.tollerance = 33);
    BOOST_TEST(mdl_in.solver_params.num_of_iterrations = 34);

    //model parameters
    BOOST_TEST(mdl_out.dx == 1);
    BOOST_TEST(mdl_out.width == 2);
    BOOST_TEST(mdl_out.height == 3);
    BOOST_TEST(mdl_out.river_boundary_id == 10);
    BOOST_TEST(mdl_out.boundary_ids == vec);
    BOOST_TEST(mdl_out.boundary_condition == 6);
    BOOST_TEST(mdl_out.field_value == 7);
    BOOST_TEST(mdl_out.eta == 8);
    BOOST_TEST(mdl_out.bifurcation_type == 9);
    BOOST_TEST(mdl_out.bifurcation_threshold == 10);
    BOOST_TEST(mdl_out.bifurcation_threshold_2 == 11);
    BOOST_TEST(mdl_out.bifurcation_min_dist == 12);
    BOOST_TEST(mdl_out.bifurcation_angle == 13);
    BOOST_TEST(mdl_out.growth_type == 14);
    BOOST_TEST(mdl_out.growth_threshold == 15);
    BOOST_TEST(mdl_out.growth_min_distance == 16);
    BOOST_TEST(mdl_out.ds == 17);

    //Border Test
    BOOST_TEST((border_in.vertices.front() == Point{2, 0}));
    BOOST_TEST((border_in.vertices.back() == Point{0.3, 0}));

    //Tree Test
    BOOST_TEST(tree_in.source_branches_id == sources_id);
    map<int, pair<int, int>> t = {{1, {2, 3}}, {3, {4, 5}}};
    BOOST_TEST(tree_in.branches_relation == t);
    BOOST_TEST(tree_in.GetBranch(5)->TipPoint().x == 11);
    BOOST_TEST(tree_in.GetBranch(5)->TipPoint().y == 10);
    BOOST_TEST(tree_in.GetBranch(1)->TipPoint().x == 0.3);
    BOOST_TEST(tree_in.GetBranch(1)->TipPoint().y == 0);

    //GeometryDifference
    auto a1 = vector<vector<double>>{{2, 3}, {3, 4}, {4, 5}, {5, 6}},
        a2 = vector<vector<double>>{{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};

    BOOST_TEST(gd_in.branches_bifuraction_info[1] == a1);
    BOOST_TEST(gd_in.branches_series_params_and_geom_diff[1] == a2);
}