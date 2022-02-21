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
    BOOST_TEST(model.prog_opt.debug == model_po.prog_opt.debug );
    BOOST_TEST(model.prog_opt.output_file_name == model_po.prog_opt.output_file_name );
    BOOST_TEST(model.prog_opt.input_file_name == model_po.prog_opt.input_file_name );
    BOOST_TEST(model.prog_opt.save_each_step == model_po.prog_opt.save_each_step );

    //mesh options
    BOOST_TEST(model.mesh.refinment_radius == model_po.mesh.refinment_radius);
    BOOST_TEST(model.mesh.exponant == model_po.mesh.exponant);
    BOOST_TEST(model.mesh.sigma == model_po.mesh.sigma);
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
    BOOST_TEST(model.solver_params.static_refinment_steps == model_po.solver_params.static_refinment_steps);
    BOOST_TEST(model.solver_params.refinment_fraction == model_po.solver_params.refinment_fraction);
    BOOST_TEST(model.solver_params.quadrature_degree == model_po.solver_params.quadrature_degree);
    BOOST_TEST(model.solver_params.max_distance == model_po.solver_params.max_distance);
    BOOST_TEST(model.solver_params.field_value == model_po.solver_params.field_value);

    //model parameters
    BOOST_TEST(model.dx == model_po.dx);
    BOOST_TEST(model.width == model_po.width);
    BOOST_TEST(model.height == model_po.height);

    //these options are not set from program options interface
    BOOST_TEST(model.river_boundary_id == model_po.river_boundary_id);
    BOOST_TEST(model.ds == model_po.ds);
    BOOST_TEST(model.eta == model_po.eta);
    BOOST_TEST(model.bifurcation_type == model_po.bifurcation_type);
    BOOST_TEST(model.bifurcation_threshold == model_po.bifurcation_threshold);
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
        "--debug",
        "--output", "38",
        "--input", "39", 
        "--save-each-step",

        //mesh options
        "--refinment-radius", "4",
        "--mesh-exp", "5",
        "--mesh-sigma", "6",
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
        "--static-refinment-steps", "7",
        "--refinment-fraction", "21", 
        "--quadrature-degree", "22", 
        "--max-dist", "99",
        "--field-value", "27",

        //model options
        "--dx", "23",
        "--width", "24",
        "--height", "25",
        "--ds", "28",
        "--eta", "29",
        "--bifurcation-type", "30",
        "--bifurcation-threshold", "31",
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
    BOOST_TEST(model.prog_opt.debug == true);
    //BOOST_TEST(model.prog_opt.output_file_name == "38"); 
    BOOST_TEST(model.prog_opt.input_file_name == "39");
    BOOST_TEST(model.prog_opt.save_each_step == true);

    //mesh options
    BOOST_TEST(model.mesh.refinment_radius == 4);
    BOOST_TEST(model.mesh.exponant == 5);
    BOOST_TEST(model.mesh.sigma == 6);
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
    BOOST_TEST(model.solver_params.static_refinment_steps == 7);
    BOOST_TEST(model.solver_params.refinment_fraction == 21);
    BOOST_TEST(model.solver_params.quadrature_degree == 22);
    BOOST_TEST(model.solver_params.max_distance == 99);
    BOOST_TEST(model.solver_params.field_value == 27);

    //model parameters
    BOOST_TEST(model.dx == 23);
    BOOST_TEST(model.width == 24);
    BOOST_TEST(model.height == 25);
    BOOST_TEST(model.river_boundary_id == 90);
    BOOST_TEST(model.ds == 28);
    BOOST_TEST(model.eta == 29);
    BOOST_TEST(model.bifurcation_type == 30);
    BOOST_TEST(model.bifurcation_threshold == 31);
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
    mdl_out.prog_opt.debug = true;
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
    mdl_out.solver_params.max_distance = 99.;
    mdl_out.solver_params.field_value = 7;

    //model options
    mdl_out.dx = 1;
    mdl_out.width = 2;
    mdl_out.height = 3;
    mdl_out.river_boundary_id = 10;
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
    
    mdl_out.sources = mdl_out.border.MakeRectangular();
    
    //Rivers object setup
    mdl_out.tree.Initialize(mdl_out.border.GetSourcesIdsPointsAndAngles(mdl_out.sources));

    Branch 
        br1left({0, 1}, 2), br1right({3, 4}, 5), 
        br2left({6, 7}, 8), br2right({9, 10}, 11);

    br1left.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br1right.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br2left.AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddPoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    br2right.AddAbsolutePoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id).AddAbsolutePoint(Polar{1, 0}, (t_boundary_id)mdl_out.river_boundary_id);
    mdl_out.tree.AddSubBranches(1, br1left, br1right);
    mdl_out.tree.AddSubBranches(3, br2left, br2right);

    //ModelSimulationData
    mdl_out.series_parameters[1] = {{1.}, {2.}, {3.}};
    mdl_out.series_parameters[2] = {{2.}, {3.}, {4.}};

    //SimulationData
    mdl_out.sim_data["mesh_size"] = vector<double>{1, 2, 3, 4, 5, 6, 6, 7};
    mdl_out.sim_data["degree_of_freedom"] = vector<double>{1, 2, 3, 4, 5, 6, 6, 7, 100, 200};
    mdl_out.sim_data["mesh"] = vector<double>{0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.6, 0.7, 0.100, 0.200};

    //Geometry difference object
    mdl_out.backward_data[1].a1 = {1};
    mdl_out.backward_data[1].a2 = {2};
    mdl_out.backward_data[1].a3 = {3};
    mdl_out.backward_data[1].init = {{1, 1}};
    mdl_out.backward_data[1].backward = {{2, 2}};
    mdl_out.backward_data[1].backward_forward = {{3, 3}};
    mdl_out.backward_data[1].branch_lenght_diff = 1.;

    BOOST_TEST_CHECKPOINT("Output");
    Save(mdl_out, "iotest");
    
    BOOST_TEST_CHECKPOINT("Input");

    Model mdl_in;
    mdl_in.prog_opt.input_file_name = "iotest.json";
    Open(mdl_in);

    BOOST_TEST_CHECKPOINT("After Input");

    //program options
    BOOST_TEST(mdl_in.prog_opt.verbose == true);
    BOOST_TEST(mdl_in.prog_opt.debug == true);
    BOOST_TEST(mdl_in.prog_opt.number_of_steps == 36);
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

    //ModelSimulationData
    BOOST_TEST(mdl_out.series_parameters == mdl_in.series_parameters);

    //SimulationData
    BOOST_TEST(mdl_out.sim_data == mdl_in.sim_data);

    //GeometryDifference
    auto a1 = vector<vector<double>>{{2, 3}, {3, 4}, {4, 5}, {5, 6}},
        a2 = vector<vector<double>>{{2, 7}, {5, 8}, {6, 9}, {7, 10}, {8, 11}};

    BOOST_TEST((mdl_in.backward_data[1].a1 == vector<double>{1}));
    BOOST_TEST((mdl_in.backward_data[1].a2 == vector<double>{2}));
    BOOST_TEST((mdl_in.backward_data[1].a3 == vector<double>{3}));
    BOOST_TEST((mdl_in.backward_data[1].init == vector<River::Point>{{1, 1}}));
    BOOST_TEST((mdl_in.backward_data[1].backward == vector<River::Point>{{2, 2}}));
    BOOST_TEST((mdl_in.backward_data[1].backward_forward == vector<River::Point>{{3, 3}}));
    BOOST_TEST(mdl_in.backward_data[1].branch_lenght_diff == 1.);
    

    BOOST_TEST(mdl_out.border == mdl_in.border);
    BOOST_TEST(mdl_out.tree == mdl_in.tree);
    BOOST_TEST(mdl_out.sources == mdl_in.sources);
    BOOST_TEST(mdl_out.boundary_conditions == mdl_in.boundary_conditions);
}

BOOST_AUTO_TEST_CASE( point_to_json, 
    *utf::tolerance(eps))
{
    River::Point p {1, 2};

    json j = p;

    auto p2 = j.get<River::Point>();

    BOOST_TEST(p == p2);
}

BOOST_AUTO_TEST_CASE( polar_to_json, 
    *utf::tolerance(eps))
{
    Polar p {1, 2};

    json j = p;

    auto p2 = j.get<Polar>();

    BOOST_TEST(p == p2);
}

BOOST_AUTO_TEST_CASE( BoundaryCondition_to_json, 
    *utf::tolerance(eps))
{
    BoundaryCondition bc;

    bc.type = NEUMAN;
    bc.value = -90;

    json j = bc;

    auto bcj = j.get<BoundaryCondition>();

    BOOST_TEST(bc == bcj);
}

BOOST_AUTO_TEST_CASE( BoundaryConditions_to_json, 
    *utf::tolerance(eps))
{
    BoundaryConditions bc;

    bc[1].type = NEUMAN;
    bc[1].value = -90;

    json j = bc;

    auto bcj = j.get<BoundaryConditions>();

    BOOST_TEST(bc == bcj);
}

BOOST_AUTO_TEST_CASE( Line_to_json, 
    *utf::tolerance(eps))
{
    Line line(1, 2, 10);

    json j = line;

    auto linej = j.get<Line>();

    BOOST_TEST(line == linej);
}

BOOST_AUTO_TEST_CASE( SimpleBoundary_to_json, 
    *utf::tolerance(eps))
{
    Boundary boundary;

    boundary.vertices = {{1, 1}, {2, 2}};
    boundary.lines = {{1, 2, 3}, {2, 3, 4}};
    boundary.holes = {{2, 3}, {3, 4}};
    boundary.name = "lalal";
    boundary.inner_boundary = false;

    json j = boundary;

    auto boundaryj = j.get<Boundary>();

    BOOST_TEST(boundary == boundaryj);
}

BOOST_AUTO_TEST_CASE( Boundaries_to_json, 
    *utf::tolerance(eps))
{
    Region boundary;

    boundary.MakeRectangularWithHole();

    json j = boundary;
    cout << j << endl;
    auto boundaryj = j.get<Region>();

    BOOST_TEST(boundary == boundaryj);
}

BOOST_AUTO_TEST_CASE( Sources_to_json, 
    *utf::tolerance(eps))
{
    Region boundary;

    auto sources = boundary.MakeRectangularWithHole();

    json j = sources;
    auto sourcesj = j.get<Sources>();

    BOOST_TEST(sources == sourcesj);
}

//Rivers
//Branch
BOOST_AUTO_TEST_CASE( Branch_to_json, 
    *utf::tolerance(eps))
{
    Branch branch({0, 1}, M_PI/3.);
    branch.AddPoint(Polar{1, 1}, (t_boundary_id)0);

    json j = branch;

    auto branchj = j.get<Branch>();
    cout << branchj << endl;
    cout << branch << endl;

    BOOST_TEST(branchj == branch);
}

//Rivers
BOOST_AUTO_TEST_CASE( Tree_to_json, 
    *utf::tolerance(eps))
{
    Branch branch({0, 1}, M_PI/3.);
    Rivers tree;
    auto branch_id = tree.AddBranch(branch);
    t_boundary_id boundary_id = 1;
    tree.GrowTestTree(boundary_id, branch_id);

    json j = tree;

    auto treej = j.get<Rivers>();
    cout << treej << endl;
    cout << tree << endl;

    BOOST_TEST(tree == treej);
}

//PhysModel
//SeriesParameters
BOOST_AUTO_TEST_CASE( SeriesParameters_to_json, 
    *utf::tolerance(eps))
{
    SeriesParameters params;

    params[1] = {{1, 2, 3}, {4, 5 ,6}, {7, 8, 9}};
    params[3] = {{10, 20, 30}, {40, 50, 60}, {70, 80, 90}};

    json j = params;

    auto paramsj = j.get<SeriesParameters>();

    BOOST_TEST(paramsj == params);
}

//SimulationData
    
//BackwardData
BOOST_AUTO_TEST_CASE( BackwardData_to_json, 
    *utf::tolerance(eps))
{
    BackwardData data;

    data.a1 = {1};
    data.a2 = {3};
    data.a3 = {7};

    data.init = {{1, 2}};
    data.backward = {{2, 3}};
    data.backward_forward = {{4, 5}};

    data.branch_lenght_diff = 9;

    json j = data;

    auto dataj = j.get<BackwardData>();

    BOOST_TEST(dataj == data);
}

//t_GeometryDiffernceNew

//ProgramOptions
BOOST_AUTO_TEST_CASE( ProgramOptions_to_json, 
    *utf::tolerance(eps))
{
    ProgramOptions data;
    data.simulation_type = 10;
    data.number_of_steps = 11;
    data.maximal_river_height = 12;
    data.number_of_backward_steps = 13;
    data.save_vtk = true;
    data.save_each_step = true;
    data.verbose = true;
    data.debug = true;
    data.output_file_name = "lalalala";
    data.input_file_name = "kakakaka";
    
    json j = data;

    auto dataj = j.get<ProgramOptions>();

    BOOST_TEST(dataj == data);
}

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
    data.eps = 19;
    
    json j = data;

    auto dataj = j.get<MeshParams>();

    BOOST_TEST(dataj == data);
}

//IntegrationParams
BOOST_AUTO_TEST_CASE( IntegrationParams_to_json, 
    *utf::tolerance(eps))
{
    IntegrationParams data;
    data.weigth_func_radius = 10;
    data.integration_radius = 11;
    data.exponant = 12;

    json j = data;

    auto dataj = j.get<IntegrationParams>();

    BOOST_TEST(dataj == data);
}

//SolverParams
BOOST_AUTO_TEST_CASE( SolverParams_to_json, 
    *utf::tolerance(eps))
{
    SolverParams data;

    data.tollerance = 10;
    data.num_of_iterrations = 11;
    data.adaptive_refinment_steps = 12;
    data.static_refinment_steps = 49;
    data.refinment_fraction = 13;
    data.quadrature_degree = 14;
    data.renumbering_type = 15;
    data.max_distance = 16;
    data.field_value = 5;
    
    json j = data;

    auto dataj = j.get<SolverParams>();

    BOOST_TEST(dataj == data);
}

//Model
BOOST_AUTO_TEST_CASE( Model_to_json, 
    *utf::tolerance(eps))
{
    Model data;

    data.InitializeDirichletWithHole();
    data.dx = 1;
    data.width = 2;
    data.height = 3;
    data.river_boundary_id = 4;
    data.eta = 6;
    data.bifurcation_type = 7;
    data.bifurcation_threshold = 8;
    data.bifurcation_min_dist = 10;
    data.bifurcation_angle = 11;
    data.growth_type = 12;
    data.growth_threshold = 13;
    data.growth_min_distance = 14;
    data.ds = 15;
    
    json j = data;

    auto dataj = j.get<Model>();

    BOOST_TEST(dataj == data);
}