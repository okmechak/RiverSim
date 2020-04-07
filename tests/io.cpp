//Link to Boost
#define BOOST_TEST_DYN_LINK

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

const double eps = 1e-12;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( default_program_options, 
    *utf::tolerance(eps))
{
    vector<string> std_argv = {"./riversim"};
    int argc = std_argv.size();
    std::vector<std::vector<char>> vstrings;
    std::vector<char*> cstrings;
    vstrings.reserve(argc);
    cstrings.reserve(argc);
    for(int i = 0; i < argc; ++i)
    {
        vstrings.emplace_back(std_argv[i].begin(), std_argv[i].end());
        vstrings.back().push_back('\0');
        cstrings.push_back(vstrings.back().data());
    }

    char **argv = cstrings.data();
    auto po = process_program_options(argc, argv);
    Model model, model_po;
    SetupModelParamsFromProgramOptions(po, model);
    //program options
    BOOST_TEST(model.prog_opt.simulation_type == model_po.prog_opt.simulation_type );
    BOOST_TEST(model.prog_opt.number_of_steps  == model_po.prog_opt.number_of_steps  );
    BOOST_TEST(model.prog_opt.maximal_river_height  == model_po.prog_opt.maximal_river_height  );
    BOOST_TEST(model.prog_opt.number_of_backward_steps  == model_po.prog_opt.number_of_backward_steps  );
    BOOST_TEST(model.prog_opt.save_vtk == model_po.prog_opt.save_vtk );
    BOOST_TEST(model.prog_opt.verbose == model_po.prog_opt.verbose );
    BOOST_TEST(model.prog_opt.output_file_name == model_po.prog_opt.output_file_name );
    BOOST_TEST(model.prog_opt.input_file_name == model_po.prog_opt.input_file_name );
    BOOST_TEST(model.prog_opt.save_each_step == model_po.prog_opt.save_each_step );

    //mesh options
    BOOST_TEST(model.mesh.refinment_radius == model_po.mesh.refinment_radius);
    BOOST_TEST(model.mesh.exponant == model_po.mesh.exponant);
    BOOST_TEST(model.mesh.sigma == model_po.mesh.sigma);
    BOOST_TEST(model.mesh.static_refinment_steps == model_po.mesh.static_refinment_steps);
    BOOST_TEST(model.mesh.min_area == model_po.mesh.min_area);
    BOOST_TEST(model.mesh.max_area == model_po.mesh.max_area);
    BOOST_TEST(model.mesh.min_angle ==  model_po.mesh.min_angle );
    BOOST_TEST(model.mesh.max_edge ==  model_po.mesh.max_edge );
    BOOST_TEST(model.mesh.min_edge ==  model_po.mesh.min_edge );
    BOOST_TEST(model.mesh.ratio ==  model_po.mesh.ratio );
    BOOST_TEST(model.mesh.eps ==  model_po.mesh.eps );

    //integration options
    BOOST_TEST(model.integr.weigth_func_radius == model_po.integr.weigth_func_radius);
    BOOST_TEST(model.integr.integration_radius == model_po.integr.integration_radius);
    BOOST_TEST(model.integr.exponant == model_po.integr.exponant);

    //solver parameters
    BOOST_TEST(model.solver_params.tollerance == model_po.solver_params.tollerance);
    BOOST_TEST(model.solver_params.num_of_iterrations == model_po.solver_params.num_of_iterrations);
    BOOST_TEST(model.solver_params.adaptive_refinment_steps == model_po.solver_params.adaptive_refinment_steps);
    BOOST_TEST(model.solver_params.refinment_fraction == model_po.solver_params.refinment_fraction);
    BOOST_TEST(model.solver_params.quadrature_degree == model_po.solver_params.quadrature_degree);

    //model parameters
    BOOST_TEST(model.dx == model_po.dx);
    BOOST_TEST(model.width == model_po.width);
    BOOST_TEST(model.height == model_po.height);
    //these options are not set from program options interface
    BOOST_TEST(model.river_boundary_id == 5);
    BOOST_TEST(model.field_value == model_po.field_value);
    BOOST_TEST(model.ds == model_po.ds);
    BOOST_TEST(model.eta == model_po.eta);
    BOOST_TEST(model.bifurcation_type == model_po.bifurcation_type);
    BOOST_TEST(model.bifurcation_threshold == model_po.bifurcation_threshold);
    BOOST_TEST(model.bifurcation_threshold_2 == model_po.bifurcation_threshold_2);
    BOOST_TEST(model.bifurcation_min_dist == model_po.bifurcation_min_dist);
    BOOST_TEST(model.bifurcation_angle == model_po.bifurcation_angle);
    BOOST_TEST(model.growth_type == model_po.growth_type);
    BOOST_TEST(model.growth_threshold == model_po.growth_threshold);
    BOOST_TEST(model.growth_min_distance == model_po.growth_min_distance);
}

BOOST_AUTO_TEST_CASE( program_options, 
    *utf::tolerance(eps))
{
    
    vector<string> std_argv = {
        "./riversim",
        "-h", 
        "-v", 
        "--suppress-signature", 

        //Program Options
        "--simulation-type", "1",
        "--number-of-steps", "2",
        "--maximal-river-height", "2",
        "--number-of-backward-steps", "3",
        "--vtk",
        "--verbose",
        "--output", "38",
        "--input", "39", 
        "--save-each-step",

        //mesh options
        "--refinment-radius", "4",
        "--mesh-exp", "5",
        "--mesh-sigma", "6",
        "--static-refinment-steps", "7",
        "--mesh-min-area", "8",
        "--mesh-max-area", "9",
        "--mesh-min-angle", "10",
        "--mesh-max-edge", "11",
        "--mesh-min-edge", "12", 
        "--mesh-ratio", "13",
        "--eps", "14",

        //integration options
        "--weight-radius", "15",
        "--integration-radius", "16",
        "--weight-exp", "17",
        
        //solver options
        "--tol", "18", 
        "--iteration-steps", "19", 
        "--adaptive-refinment-steps", "20",
        "--refinment-fraction", "21", 
        "--quadrature-degree", "22", 

        //model options
        "--dx", "23",
        "--width", "24",
        "--height", "25",
        "--field-value", "27",
        "--ds", "28",
        "--eta", "29",
        "--bifurcation-type", "30",
        "--bifurcation-threshold", "31",
        "--bifurcation-threshold-2", "32",
        "--bifurcation-min-distance", "33",
        "--bifurcation-angle", "34",
        "--growth-type", "35",
        "--growth-threshold", "36",
        "--growth-min-distance", "37",
        "--river-boundary-id", "90"
    };
    int argc = std_argv.size();

    // guarantee contiguous, null terminated strings
    std::vector<std::vector<char>> vstrings;

    // pointers to rhose strings
    std::vector<char*> cstrings;

    vstrings.reserve(argc);
    cstrings.reserve(argc);

    for(int i = 0; i < argc; ++i)
    {
        vstrings.emplace_back(std_argv[i].begin(), std_argv[i].end());
        vstrings.back().push_back('\0');
        cstrings.push_back(vstrings.back().data());
    }

    char **argv = cstrings.data();

    auto po = process_program_options(argc, argv);

    Model model;
    SetupModelParamsFromProgramOptions(po, model);

    //program options
    BOOST_TEST(model.prog_opt.simulation_type == 1);
    BOOST_TEST(model.prog_opt.number_of_steps == 2);
    BOOST_TEST(model.prog_opt.maximal_river_height == 2);
    BOOST_TEST(model.prog_opt.number_of_backward_steps == 3);
    BOOST_TEST(model.prog_opt.save_vtk == true);
    BOOST_TEST(model.prog_opt.verbose == true);
    //BOOST_TEST(model.prog_opt.output_file_name == "38"); 
    BOOST_TEST(model.prog_opt.input_file_name == "39");
    BOOST_TEST(model.prog_opt.save_each_step == true);

    //mesh options
    BOOST_TEST(model.mesh.refinment_radius == 4);
    BOOST_TEST(model.mesh.exponant == 5);
    BOOST_TEST(model.mesh.sigma == 6);
    BOOST_TEST(model.mesh.static_refinment_steps == 7);
    BOOST_TEST(model.mesh.min_area == 8);
    BOOST_TEST(model.mesh.max_area == 9);
    BOOST_TEST(model.mesh.min_angle == 10);
    BOOST_TEST(model.mesh.max_edge == 11);
    BOOST_TEST(model.mesh.min_edge == 12);
    BOOST_TEST(model.mesh.ratio == 13);
    BOOST_TEST(model.mesh.eps == 14);

    //integration options
    BOOST_TEST(model.integr.weigth_func_radius == 15);
    BOOST_TEST(model.integr.integration_radius == 16);
    BOOST_TEST(model.integr.exponant == 17);

    //solver parameters
    BOOST_TEST(model.solver_params.tollerance == 18);
    BOOST_TEST(model.solver_params.num_of_iterrations == 19);
    BOOST_TEST(model.solver_params.adaptive_refinment_steps == 20);
    BOOST_TEST(model.solver_params.refinment_fraction == 21);
    BOOST_TEST(model.solver_params.quadrature_degree == 22);

    //model parameters
    BOOST_TEST(model.dx == 23);
    BOOST_TEST(model.width == 24);
    BOOST_TEST(model.height == 25);
    BOOST_TEST(model.river_boundary_id == 90);
    BOOST_TEST(model.field_value == 27);
    BOOST_TEST(model.ds == 28);
    BOOST_TEST(model.eta == 29);
    BOOST_TEST(model.bifurcation_type == 30);
    BOOST_TEST(model.bifurcation_threshold == 31);
    BOOST_TEST(model.bifurcation_threshold_2 == 32);
    BOOST_TEST(model.bifurcation_min_dist == 33);
    BOOST_TEST(model.bifurcation_angle == 34);
    BOOST_TEST(model.growth_type == 35);
    BOOST_TEST(model.growth_threshold == 36);
    BOOST_TEST(model.growth_min_distance == 37);
}

BOOST_AUTO_TEST_CASE( files_io_methods, 
    *utf::tolerance(eps))
{
    Model mdl_out;

    //program options
    mdl_out.prog_opt.verbose = true;
    mdl_out.prog_opt.number_of_steps = 36;
    mdl_out.prog_opt.maximal_river_height = 37;
    mdl_out.prog_opt.number_of_backward_steps = 38;
    mdl_out.prog_opt.save_vtk = true;
    mdl_out.prog_opt.simulation_type = 40;
    mdl_out.prog_opt.output_file_name = "lalala";
    mdl_out.prog_opt.input_file_name = "kakaka";
    mdl_out.prog_opt.save_each_step = true;

    //mesh options
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

    //Boundary conditions
    mdl_out.boundary_conditions[2] = {DIRICHLET, 10};
    mdl_out.boundary_conditions[3] = {NEUMAN, 11};

    //Boundary object setup.. Rectangular boundaries
    auto region_size = vector<double>{2, 3};
    auto sources_x_coord = vector<double>{0.3};
    auto sources_id = vector<t_source_id>{1};
    
    mdl_out.sources = mdl_out.border.MakeRectangular();
    
    //Tree object setup
    mdl_out.tree.Initialize(mdl_out.border.GetSourcesIdsPointsAndAngles(mdl_out.sources));

    BranchNew 
        br1left({0, 1}, 2), br1right({3, 4}, 5), 
        br2left({6, 7}, 8), br2right({9, 10}, 11);

    br1left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br1right.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2left.AddPoint(Polar{1, 0}).AddPoint(Polar{1, 0});
    br2right.AddAbsolutePoint(Polar{1, 0}).AddAbsolutePoint(Polar{1, 0});
    mdl_out.tree.AddSubBranches(1, br1left, br1right);
    mdl_out.tree.AddSubBranches(3, br2left, br2right);

    //Geometry difference object
    mdl_out.geometry_difference.branches_bifuraction_info[1] = {{2, 3}, {3, 4}, {4, 5}, {5, 6}};
    mdl_out.geometry_difference.branches_series_params_and_geom_diff[1] = {{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};


    BOOST_TEST_CHECKPOINT("Output");
    Save(mdl_out, "iotest");
    
    BOOST_TEST_CHECKPOINT("Input");

    Model mdl_in;
    mdl_in.prog_opt.input_file_name = "iotest.json";
    Open(mdl_in);

    //program options
    BOOST_TEST(mdl_in.prog_opt.verbose == true);
    BOOST_TEST(mdl_in.prog_opt.number_of_steps == 36);
    BOOST_TEST(mdl_in.prog_opt.maximal_river_height == 37);
    BOOST_TEST(mdl_in.prog_opt.number_of_backward_steps == 38);
    BOOST_TEST(mdl_in.prog_opt.save_vtk == true);
    BOOST_TEST(mdl_in.prog_opt.simulation_type == 40);
    //BOOST_TEST(mdl_in.prog_opt.output_file_name == "lalala");
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
    BOOST_TEST(mdl_in.dx == 1);
    BOOST_TEST(mdl_in.width == 2);
    BOOST_TEST(mdl_in.height == 3);
    BOOST_TEST(mdl_in.river_boundary_id == 10);
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

    //Boundary Test
    //BOOST_TEST((mdl_in.border.vertices.front() == Point{2, 0}));
    //BOOST_TEST((mdl_in.border.vertices.back() == Point{0.3, 0}));

    //Tree Test
    //BOOST_TEST(mdl_in.tree.source_branches_id == sources_id);
    map<t_branch_id, pair<t_branch_id, t_branch_id>> t = {{1, {2, 3}}, {3, {4, 5}}};
    BOOST_TEST(mdl_in.tree.branches_relation == t);
    BOOST_TEST(mdl_in.tree.GetBranch(5)->TipPoint().x == 11.);
    BOOST_TEST(mdl_in.tree.GetBranch(5)->TipPoint().y == 10.);
    BOOST_TEST(mdl_in.tree.GetBranch(1)->TipPoint().x == 0.25);
    BOOST_TEST(mdl_in.tree.GetBranch(1)->TipPoint().y == 0.);

    //GeometryDifference
    auto a1 = vector<vector<double>>{{2, 3}, {3, 4}, {4, 5}, {5, 6}},
        a2 = vector<vector<double>>{{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};

    BOOST_TEST(mdl_in.geometry_difference.branches_bifuraction_info[1] == a1);
    BOOST_TEST(mdl_in.geometry_difference.branches_series_params_and_geom_diff[1] == a2);

    BOOST_TEST(mdl_out.border == mdl_in.border);
    BOOST_TEST(mdl_out.tree == mdl_in.tree);
    BOOST_TEST(mdl_out.sources == mdl_in.sources);
    BOOST_TEST(mdl_out.boundary_conditions == mdl_in.boundary_conditions);
}