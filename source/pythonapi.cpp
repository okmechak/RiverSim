#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "app.hpp"

using namespace boost::python;
using namespace River;

BOOST_PYTHON_MODULE(riversim)
{
    //geometry primitives
    def("GetNormalizedPoint", GetNormalizedPoint, args("p"), "Returns normalized vector of current Point");

    def("ToPolar", ToPolar, args("p"), "Returns Polar representation of vector");

    class_<Polar>("Polar", "Point in polar coordinates")
        .def(init<>())
        .def(init<double, double>(args( "r", "phi" ), "Point with r and phi coords."))
        .def_readwrite("r", &Polar::r, "r coordinate.")
        .def_readwrite("phi", &Polar::phi, "phi coordinate.")
        .def(self == self)
        .def(self != self)
    ;

    class_<River::Point>("Point", "Point in cartesian coordinates.")
        .def(init<>())
        .def(init<double, double>(args( "x", "y" ), "Point with x and y coords."))
        .def(init<const River::Point&>(args("point"), "Point with same coords as input point."))
        .def(init<const River::Polar&>(args("polar"), "Converts Polar point into cartesian point."))
        .def_readwrite("x", &River::Point::x, "x coordinate.")
        .def_readwrite("y", &River::Point::y, "y coordinate.")
        .def("norm", static_cast< double (River::Point::*)() const>( &River::Point::norm ), "Returns norm of vector.")
        //.def("norm", static_cast< double (Point::*)(const double, const double) >( &Point::norm ), args("x", "y") )
        //.staticmethod("norm")
        .def("rotate", &River::Point::rotate, args("phi"), return_value_policy<reference_existing_object>(), "Rotates point around beginning of coordinate by phi.")
        .def("normalize", &River::Point::normalize, return_value_policy<reference_existing_object>(), "Normalizes point.")//return value policy
        .def("angle", static_cast< double (River::Point::*)() const>( &River::Point::angle ), "Returns angle of point.")
        //.def("angle", static_cast<double (Point::*)(double, double) >( &Point::angle ), args("x", "y") )
        //.staticmethod( "angle" )
        .def("angle", static_cast< double (River::Point::*)(const River::Point &) const>( &River::Point::angle ), args("p"), "Returns angle of point.")
        .def(self + self) 
        .def(self - self) 
        .def(self += self)
        .def(self -= self)
        .def(self == self) 
        .def(self * self)
        .def(self * double())
        .def(self *= double())
        .def(self / double())
        .def(self /= double())
        .def("__getitem__", &River::Point::operator[])
        //def("__setitem__", &vector_setitem)
    ;

    //boundary
    enum_<t_boundary>("t_boundary", "Enumeration of different boundary conditions.")
        .value("DIRICHLET", DIRICHLET)
        .value("NEUMAN", NEUMAN)
        .export_values()
        ;

    class_<BoundaryCondition>("BoundaryCondition", "Describes boudary condition type.")
        .def_readwrite("value", &BoundaryCondition::value)
        .def_readwrite("type", &BoundaryCondition::type)
        .def(self == self) 
    ;

    class_<t_BoundaryConditions>("t_BoundaryConditions", "Map structure with boundary condition types.")
        .def(map_indexing_suite<t_BoundaryConditions>())
    ;

    class_<BoundaryConditions, bases<t_BoundaryConditions>>("BoundaryConditions", "Map structure with boundary condition types.")
        .def("Get", static_cast< t_BoundaryConditions (BoundaryConditions::*)(t_boundary) const>(&BoundaryConditions::Get), "Returns map structure with boundary conditions of specific type(Neuman or Dirichlet)")
    ;

    class_<t_source_coord >("t_source_coord", "Source point coordinate data type. Pair which holds boundary id and vertice position on this id.")
        .def_readwrite("boundary_id", &t_source_coord::first, "boundary id")
        .def_readwrite("vert_pos", &t_source_coord::second, "vertice position")
    ;

    class_<t_sources_ids>("t_sources_ids", "Vector of source point ids data type.")
        .def(vector_indexing_suite<t_sources_ids>())
    ;

    class_<t_Sources >("t_Sources", "Map structure which holds source id and its source point coordinates.")
        .def(map_indexing_suite<t_Sources>() )
    ;

    class_<Sources, bases<t_Sources>>("Sources", "Map structure which holds source id and its source point coordinates.")
        .def("getSourcesIds", &Sources::GetSourcesIds, "Returns all sources ids.")
    ;

    class_<Line>("Line", "Connects two point of boundaries and holds boundary condition id.", init<>())
        .def(init<Line&>(args("line"), "Copies input line."))
        .def(init<t_vert_pos, t_vert_pos, t_boundary_id>(args( "p1v", "p2v", "boundary_id"), "Defines new line with p1v and p2v points and boudanry_id."))
        .def_readwrite("p1", &Line::p1, "First point index.")
        .def_readwrite("p2", &Line::p2, "Second point index.")
        .def_readwrite("boundary_id", &Line::boundary_id, "Boundary id.")
        .def(self == self)
    ;

    class_<t_PointList>("t_PointList", "Vector of points.")
        .def(vector_indexing_suite<t_PointList>())
    ;

    class_<t_LineList>("t_LineList", "Vector of lines.")
        .def(vector_indexing_suite<t_LineList>())
    ;

    class_<Boundary >("Boundary", "Structure which defines simple boundary data structure.")
        .def("append", &Boundary::Append, "Appends another simple boundary at the end of current boundary.")
        .def("replaceElement", &Boundary::ReplaceElement, "Replace on element of boundary with whole simple boundary structure.")
        .def(self == self)
        .def_readwrite("name", &Boundary::name, "Name with description of boundary.")
        .def_readwrite("vertices", &Boundary::vertices, "Vertices of boundary vector.")
        .def_readwrite("lines", &Boundary::lines, "Connvections between boundaries.")
        .def_readwrite("inner_boundary", &Boundary::inner_boundary, "Is inner boundary. It has consequences on holes evaluation")
        .def_readwrite("holes", &Boundary::holes, "Array of holes. Which will be eliminated by mesh generator.")
    ;

    class_<t_Boundaries>("t_Boundaries", "Structure which defines Region of region.")
        .def(map_indexing_suite<t_Boundaries>())
    ;

    class_<t_branch_source >("t_branch_source")
        .def_readwrite("point", &t_branch_source::first, "Source point of branche.")
        .def_readwrite("angle", &t_branch_source::second, "Angle of growth.")
    ;

    class_<Region::trees_interface_t >("trees_interface_t", "Interface between boundary sources and source points of Rivers.")
        .def(map_indexing_suite<Region::trees_interface_t>())
    ;

    class_<Region, bases<t_Boundaries> >("Region", "Structure which defines Region of region.")
        .def(init<t_Boundaries>(args("simple_boundaries"), "Constructor"))
        .def("makeRectangular", &Region::MakeRectangular, "Initialize rectangular Region.")
        .def("makeRectangularWithHole", &Region::MakeRectangularWithHole, "Initialize rectangular with hole Region.")
        .def("check", &Region::Check, "Some basic checks of data structure.")
        .def("getOuterBoundary", &Region::GetOuterBoundary, return_internal_reference<>(), "Return outer simple boundary(There should be only one such).")
        .def("getHolesList", &Region::GetHolesList, "Returns vector of all holes.")
        .def("getSourcesIdsPointsAndAngles", &Region::GetSourcesIdsPointsAndAngles, "Returns map of source points ids and coresponding source point and angle of tree Branch.")
    ;
    
    //tree
    class_<Branch, bases<Boundary>>("Branch", "Holds all functionality that you need to work with single branch.")
        .def(init<River::Point&, double>(args("source_point", "angle")))
        .def("addAbsolutePoint", static_cast< Branch& (Branch::*)(const River::Point&, const t_boundary_id boundary_id)>( &Branch::AddAbsolutePoint), return_internal_reference<>(), "Adds point p to branch with absolute coords.")
        .def("addAbsolutePoint", static_cast< Branch& (Branch::*)(const Polar&, const t_boundary_id boundary_id)>( &Branch::AddAbsolutePoint), return_internal_reference<>(), "Adds polar p coords to branch with absolute angle, but position is relative to tip.")
        .def("addPoint", static_cast< Branch& (Branch::*)(const River::Point&, const t_boundary_id boundary_id)>( &Branch::AddPoint), return_internal_reference<>(), "Adds point p to branch in tip relative coord system.")
        .def("addPoint", static_cast< Branch& (Branch::*)(const Polar&, const t_boundary_id boundary_id)>( &Branch::AddPoint), return_internal_reference<>(), "Adds polar p to branch in tip relative coord and angle system.")
        .def("shrink", &Branch::Shrink, return_internal_reference<>(), "Reduces lenght of branch by p lenght.")
        .def("removeTipPoint", &Branch::RemoveTipPoint, return_internal_reference<>(), "Remove tip point from branch(simply pops element from vector).")
        .def("tipPoint", &Branch::TipPoint, "Return TipPoint of branch(last point in branch).")
        .def("tipVector", &Branch::TipVector, "Returns vector of tip - difference between two adjacent points.")
        .def("vector", &Branch::Vector, "Returns vector of p i-th segment of branch.")
        .def("tipAngle", &Branch::TipAngle, "Returns angle of branch tip.")
        .def("sourcePoint", &Branch::SourcePoint, "Returns source point of branch(the first one).")
        .def("sourceAngle", &Branch::SourceAngle, "Returns source angle of branch.")
        .def("lenght", &Branch::Lenght, "Returns lenght of whole branch.")
        .def(self == self)
    ;

    class_<t_Tree >("t_Tree", "Combines branches into tree like structure.")
        .def(map_indexing_suite<t_Tree>())
    ;

    class_<t_branch_id_pair >("t_branch_id_pair")
        .def_readwrite("left", &t_branch_id_pair::first, "Left branch id.")
        .def_readwrite("right", &t_branch_id_pair::second, "Right branch id.")
    ;

    class_<Rivers, bases<t_Tree> >("Rivers", "Combines branches into tree like structure.", init<>())
        .def(init<Rivers&>())
        .def(self == self)
        .def("initialize", &Rivers::Initialize, "Initialize tree with source points vector and source angle vector.")
        .def("addBranch", &Rivers::AddBranch, "Adds new branch with id (id should be unique).")
        .def("addSubBranches", &Rivers::AddSubBranches, "Returns root(or source) branch of branch (if there is no such - throw exception).")
        .def("deleteBranch", &Rivers::DeleteBranch)
        .def("deleteSubBranches", &Rivers::DeleteSubBranches)
        .def("clear", &Rivers::Clear, "Clear whole data from tree.")
        .def("getParentBranchId", &Rivers::GetParentBranchId)
        .def("getParentBranch", &Rivers::GetParentBranch, return_internal_reference<>())
        //.def("GetSubBranchesIds", &Rivers::GetSubBranchesIds, return_internal_reference<>())
        .def("getAdjacentBranchId", &Rivers::GetAdjacentBranchId)
        .def("getAdjacentBranch", &Rivers::GetAdjacentBranch, return_internal_reference<>())
        .def("addPoints", &Rivers::AddPoints)
        .def("addPolars", &Rivers::AddPolars)
        .def("addAbsolutePolars", &Rivers::AddAbsolutePolars)
        .def("growTestTree", &Rivers::GrowTestTree)
        .def("tipBranchesIds", &Rivers::TipBranchesIds)
        .def("zeroLenghtTipBranchesIds", &Rivers::zero_lenght_tip_branches_ids)
        .def("tipPoints", &Rivers::TipPoints)
        .def("tipIdsAndPoints", &Rivers::TipIdsAndPoints)
        .def("isSourceBranch", &Rivers::IsSourceBranch)
        .def("maximalTipCurvatureDistance", &Rivers::maximal_tip_curvature_distance)
        .def("flattenTipCurvature", &Rivers::flatten_tip_curvature)
        .def("removeTipPoints", &Rivers::remove_tip_points)
        .def("hasSubBranches", &Rivers::HasSubBranches)
        .def("hasParentBranch", &Rivers::HasParentBranch)
        .def("isValidBranchId", &Rivers::IsValidBranchId)
    ;

    //physmodel
    class_<vector<vector<double>>> ("t_v2_double")
        .def(vector_indexing_suite<vector<vector<double>>>())
    ;

    class_<vector<double>> ("t_v_double")
        .def(vector_indexing_suite<vector<double>>())
    ;

    class_<t_SeriesParameters >("t_SeriesParameters")
        .def(map_indexing_suite<t_SeriesParameters>())
    ;

    class_<SimulationData >("SimulationData")
        .def(map_indexing_suite<SimulationData>())
    ;

    class_<BackwardData >("BackwardData")
        .def(self == self)
        .def_readwrite("a1", &BackwardData::a1, "First series parameters.")
        .def_readwrite("a2", &BackwardData::a2, "Second series parameters.")
        .def_readwrite("a3", &BackwardData::a3, "Third series parameters.")
        .def_readwrite("init", &BackwardData::init, "Initial points.")
        .def_readwrite("backward", &BackwardData::backward, "Backward points.")
        .def_readwrite("backward_forward", &BackwardData::backward_forward, "Backward forward points.")
        .def_readwrite("branch_lenght_diff", &BackwardData::branch_lenght_diff, "Branch length difference.")
    ;

    class_<t_GeometryDiffernceNew >("t_GeometryDiffernceNew")
        .def(map_indexing_suite<t_GeometryDiffernceNew>())
    ;

    class_<ProgramOptions >("ProgramOptions")
        .def(self == self)
        .def_readwrite("simulation_type", &ProgramOptions::simulation_type, "Simulation type: 0 - Forward, 1 - Backward, 2 - For test purposes.")
        .def_readwrite("number_of_steps", &ProgramOptions::number_of_steps, "Number of simulation steps.")
        .def_readwrite("maximal_river_height", &ProgramOptions::maximal_river_height, "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.")
        .def_readwrite("number_of_backward_steps", &ProgramOptions::number_of_backward_steps, "Number of backward steps simulations used in backward simulation type.")
        .def_readwrite("save_vtk", &ProgramOptions::save_vtk, "Outputs VTK file of Deal.II solution.")
        .def_readwrite("save_each_step", &ProgramOptions::save_each_step)
        .def_readwrite("verbose", &ProgramOptions::verbose, "If true - then program will print to standard output.")
        .def_readwrite("debug", &ProgramOptions::debug, "If true - then program will save additional output files for each stage of simulation.")
        .def_readwrite("output_file_name", &ProgramOptions::output_file_name)
        .def_readwrite("input_file_name", &ProgramOptions::input_file_name)
    ;

    class_<MeshParams >("MeshParams")
        .def(self == self)
        .def_readwrite("tip_points", &MeshParams::tip_points, "Vector of tip points.")
        .def_readwrite("refinment_radius", &MeshParams::refinment_radius)
        .def_readwrite("exponant", &MeshParams::exponant, "Radius of mesh refinment.")
        .def_readwrite("sigma", &MeshParams::sigma, "Sigma is used in exponence formula of mesh refinment.")
        .def_readwrite("min_area", &MeshParams::min_area, "Minimal area of mesh.")
        .def_readwrite("max_area", &MeshParams::max_area, "Maximal area of mesh element.")
        .def_readwrite("min_angle", &MeshParams::min_angle, "Minimal angle of mesh element.")
        .def_readwrite("max_edge", &MeshParams::max_edge, "Maximal edge size.")
        .def_readwrite("min_edge", &MeshParams::min_edge, "Minimal edge size.")
        .def_readwrite("ratio", &MeshParams::ratio, "Ratio of the triangles.")
        .def_readwrite("eps", &MeshParams::eps, "Width of branch.")
        .def("areaConstrain", &MeshParams::operator(), "Evaluates mesh area constraint at {x, y} point.")
    ;

    class_<IntegrationParams>("IntegrationParams")
        .def(self == self)
        .def_readwrite("weigth_func_radius", &IntegrationParams::weigth_func_radius, "Circle radius with centrum in tip point.")
        .def_readwrite("integration_radius", &IntegrationParams::integration_radius, "Circle radius with centrum in tip point.")
        .def_readwrite("exponant", &IntegrationParams::exponant, "Controls slope.")
        .def("weightFunction", &IntegrationParams::WeightFunction, "Weight function used in computation of series parameters.")
        .def("baseVectorFinal", &IntegrationParams::BaseVectorFinal, "Base Vector function used in computation of series parameters.")
    ;

    class_<SolverParams>("SolverParams")
        .def(self == self)
        .def_readwrite("tollerance", &SolverParams::tollerance, "Tollerarnce used by dealii Solver.")
        .def_readwrite("num_of_iterrations", &SolverParams::num_of_iterrations, "Number of solver iteration steps.")
        .def_readwrite("adaptive_refinment_steps", &SolverParams::adaptive_refinment_steps, "Number of adaptive refinment steps.")
        .def_readwrite("static_refinment_steps", &SolverParams::static_refinment_steps, "Number of mesh refinment steps used by Deal.II mesh functionality.")
        .def_readwrite("refinment_fraction", &SolverParams::refinment_fraction, "Fraction of refined mesh elements.")
        .def_readwrite("quadrature_degree", &SolverParams::quadrature_degree, "Polynom degree of quadrature integration.")
        .def_readwrite("renumbering_type", &SolverParams::renumbering_type, "Renumbering algorithm(0 - none, 1 - cuthill McKee, 2 - hierarchical, 3 - random, ...) for the degrees of freedom on a triangulation.")
        .def_readwrite("max_distance", &SolverParams::max_distance, "Maximal distance between middle point and first solved point, used in non euler growth.")
        .def_readwrite("field_value", &SolverParams::field_value, "Field value used for Poisson conditions.")
    ;

    class_<Model>("Model")
        .def(self == self)
        .def_readwrite("prog_opt", &Model::prog_opt, "Global program options.")
        .def_readwrite("border", &Model::border, "Border of simulation region.")
        .def_readwrite("sources", &Model::sources, "Source points of growth.")
        .def_readwrite("tree", &Model::tree, "Rivers data structure.")
        .def_readwrite("boundary_conditions", &Model::boundary_conditions, "Boundary conditions data structure.")
        .def_readwrite("mesh", &Model::mesh, "Mesh parameters.")
        .def_readwrite("integr", &Model::integr, "Intergration around tip points parameters.")
        .def_readwrite("solver_params", &Model::solver_params, "Solver parameteres.")
        .def_readwrite("series_parameters", &Model::series_parameters, "Series parameters data structure.")
        .def_readwrite("sim_data", &Model::sim_data, "Simulation data.")
        .def_readwrite("backward_data", &Model::backward_data, "Backward simulation data.")
        .def("initializeLaplace", &Model::InitializeLaplace, "Initialize problem to Laplacea boundary conditions.")
        .def("initializePoisson", &Model::InitializePoisson, "Initialize problem to Poisson boundary conditions.")
        .def("initializeDirichlet", &Model::InitializeDirichlet, "Initialize proble to Dirichlet boundary conditions.")
        .def("initializeDirichletWithHole", &Model::InitializeDirichletWithHole, "Intialize problem to Dirirchlet boundary conditions with hole.")
        .def("clear", &Model::Clear, "Clear all data")
        .def("revertLastSimulationStep", &Model::RevertLastSimulationStep, "Revert to last simulation step.")
        .def("saveCurrentTree", &Model::SaveCurrentTree, "Save current state of tree.")
        .def("restoreTree", &Model::RestoreTree, "Restore presaved state of tree.")
        .def_readwrite("dx", &Model::dx, "Initial x position of source.")
        .def_readwrite("width", &Model::width, "Width of region.")
        .def_readwrite("height", &Model::height, "Height of region.")
        .def_readwrite("river_boundary_id", &Model::river_boundary_id, "River boundary condition id.")
        .def_readwrite("ds", &Model::ds, "Maximal length of one step of growth.")
        .def_readwrite("eta", &Model::eta, "Eta. Growth power of a1^eta.")
        .def_readwrite("bifurcation_type", &Model::bifurcation_type, "Bifurcation method type, 0 - no bif, 1 - a(3)/a(1) > bifurcation_threshold, 2 - a1 > bifurcation_threshold, 3 - combines both conditions.")
        .def_readwrite("bifurcation_threshold", &Model::bifurcation_threshold, "Bifurcation threshold for 0 bifurcation type.")
        .def_readwrite("bifurcation_min_dist", &Model::bifurcation_min_dist, "Minimal distance between adjacent bifurcation points. Reduces numerical noise.")
        .def_readwrite("bifurcation_angle", &Model::bifurcation_angle, "Bifurcation angle.")
        .def_readwrite("growth_type", &Model::growth_type, "Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}")
        .def_readwrite("growth_threshold", &Model::growth_threshold, "Growth of branch will be done only if a1 > growth-threshold.")
        .def_readwrite("growth_min_distance", &Model::growth_min_distance, "Distance of constant tip growing after bifurcation point. Reduces numerical noise.")
        .def("qBifurcate", static_cast< bool (Model::*)(const vector<double>&) const>( &Model::q_bifurcate), "Checks by evaluating series params for bifuraction condition.")
        .def("qBifurcate", static_cast< bool (Model::*)(const vector<double>&, double branch_lenght) const>( &Model::q_bifurcate), "Checks by evaluating series params for bifuraction condition.")
        .def("qGrowth", &Model::q_growth, "Growth condition. Checks series parameters around river tip and evaluates if it is enough to grow.")
        .def("nextPoint", static_cast< Polar (Model::*)(const vector<double>&, double branch_lenght, double max_a)>(&Model::next_point), "Evaluate next point of simualtion based on series parameters around tip.")
        .def("nextPoint", static_cast< Polar (Model::*)(const vector<double>&) const>(&Model::next_point), "Evaluate next point of simualtion based on series parameters around tip.")
        .def("checkParametersConsistency", &Model::CheckParametersConsistency, "Checks if values of parameters are in normal ranges.")   
    ;

    //io
    def("programTitle", ProgramTitle, "Returns program title string.");
    def("print_ascii_signature", print_ascii_signature);
    def("version_string", version_string, "returns program version string");
    def("print_version", print_version);

    //class_<cxxopts::ParseResult >("ParseResult")
    //    .def(init<>())
    //;

    //def("process_program_options", River::process_program_options);
    //def("SetupModelParamsFromProgramOptions", River::SetupModelParamsFromProgramOptions);
    def("save", Save, "Saves current program state to file.");
    def("open", Open, "Opens program state from file.");
    //def("InitializeModelObject", River::InitializeModelObject);

    //boundary_generator
    def("treeVertices", TreeVertices);
    def("treeBoundary", TreeBoundary, "");
    def("simpleBoundaryGenerator", SimpleBoundaryGenerator, "Generates boundary of whole region used by mesh generator.");

    //tethex
    class_<tethex::Point>("TethexPoint",  init<>())
        .def(init<const double, const double, const double, const int>(args( "x", "y", "z", "region_tag")))
        .def(init<const tethex::Point&>())
        .def(init<const River::Point&>())
        .def("getCoord", &tethex::Point::get_coord)
        .def("setCoord", &tethex::Point::set_coord, args("number", "value"))
        .def(self == self)
    ;

    class_<vector<tethex::Point>> ("t_Vector_of_TethexPoint")
        .def(vector_indexing_suite<vector<tethex::Point>>())
        
    ;

    //class_<tethex::MeshElement>("MeshElement")
    //    .def("get_n_vertices", &tethex::MeshElement::get_n_vertices)
    //    .def("get_n_edges", &tethex::MeshElement::get_n_edges)
    //    .def("get_vertex", &tethex::MeshElement::get_vertex, args("number"))
    //    .def("set_vertex", &tethex::MeshElement::set_vertex, args("local_number", "global_number"))
    //    .def("get_edge", &tethex::MeshElement::get_edge, args("number"))
    //    .def("set_edge", &tethex::MeshElement::set_edge, args("local_number", "global_number"))
    //    .def("contains", &tethex::MeshElement::contains, args("vertex"))
    //    .def("measure", &tethex::MeshElement::measure, args("points"))
    //    .def("min_angle", &tethex::MeshElement::min_angle, args("points"))
    //    .def("max_angle", &tethex::MeshElement::max_angle, args("points"))
    //    .def("min_edge", &tethex::MeshElement::min_edge, args("points"))
    //    .def("max_edge", &tethex::MeshElement::max_edge, args("points"))
    //    .def("quality", &tethex::MeshElement::quality, args("points"))
    //;

    class_<tethex::Mesh>("TethexMesh")
        .def(init<>())
        .def(init<const tethex::Mesh&>(args("mesh")))
        .def(init<const River::Boundary&>(args("boundaries")))

        .def("read", &tethex::Mesh::read)
        .def("write", &tethex::Mesh::write)
        .def("clean", &tethex::Mesh::clean)
        .def("convert", &tethex::Mesh::convert)

        .def("getNVertices", &tethex::Mesh::get_n_vertices)
        .def("getNHoles", &tethex::Mesh::get_n_holes)
        .def("getNPoints", &tethex::Mesh::get_n_points)
        .def("getNLines", &tethex::Mesh::get_n_lines)
        .def("getNTriangles", &tethex::Mesh::get_n_triangles)
        .def("getNQuadrangles", &tethex::Mesh::get_n_quadrangles)

    //    .def("get_vertex", &tethex::Mesh::get_vertex, args("number"), return_internal_reference<>())
    //    .def("get_hole", &tethex::Mesh::get_hole, args("number"), return_internal_reference<>())
    //    .def("get_point", &tethex::Mesh::get_point, args("number"), return_internal_reference<>())
    //    .def("get_edge", &tethex::Mesh::get_edge, args("number"), return_internal_reference<>())
    //    .def("get_line", &tethex::Mesh::get_line, args("number"), return_internal_reference<>())
    //    .def("get_triangle", &tethex::Mesh::get_triangle, args("number"), return_internal_reference<>())
    //    .def("get_quadrangle", &tethex::Mesh::get_quadrangle, args("number"), return_internal_reference<>())
    ;

    //triangle
    class_<triangulateio >("triangulateio")
        .def_readwrite("pointlist", &triangulateio::pointlist)
        .def_readwrite("pointattributelist", &triangulateio::pointattributelist)
        .def_readwrite("pointmarkerlist", &triangulateio::pointmarkerlist)
        .def_readwrite("numberofpoints", &triangulateio::numberofpoints)
        .def_readwrite("numberofpointattributes", &triangulateio::numberofpointattributes)
        .def_readwrite("trianglelist", &triangulateio::trianglelist)
        .def_readwrite("triangleattributelist", &triangulateio::triangleattributelist)
        .def_readwrite("trianglearealist", &triangulateio::trianglearealist)
        .def_readwrite("neighborlist", &triangulateio::neighborlist)
        .def_readwrite("numberoftriangles", &triangulateio::numberoftriangles)
        .def_readwrite("numberofcorners", &triangulateio::numberofcorners)
        .def_readwrite("numberoftriangleattributes", &triangulateio::numberoftriangleattributes)
        .def_readwrite("segmentlist", &triangulateio::segmentlist)
        .def_readwrite("segmentmarkerlist", &triangulateio::segmentmarkerlist)
        .def_readwrite("numberofsegments", &triangulateio::numberofsegments)
        .def_readwrite("holelist", &triangulateio::holelist)
        .def_readwrite("numberofholes", &triangulateio::numberofholes)
        .def_readwrite("regionlist", &triangulateio::regionlist)
        .def_readwrite("numberofregions", &triangulateio::numberofregions)
        .def_readwrite("edgelist", &triangulateio::edgelist)
        .def_readwrite("edgemarkerlist", &triangulateio::edgemarkerlist)
        .def_readwrite("normlist", &triangulateio::normlist)
        .def_readwrite("numberofedges", &triangulateio::numberofedges)
    ;

    //mesh
    //enum_<Triangle::algorithm>("e_triangle_algorithm")
    //    .value("CONQUER", Triangle::algorithm::CONQUER)
    //    .value("FORTUNE", Triangle::algorithm::FORTUNE)
    //    .value("ITERATOR", Triangle::algorithm::ITERATOR)
    //    .export_values()
    //;

    class_<Triangle >("Triangle", init<>())
        .def(init<MeshParams*>(args("mesh_params")))

        .def("generate", &Triangle::generate)

        .def_readwrite("refine", &Triangle::Refine, "Refine previously generated mesh, with preserving of segments")
        .def_readwrite("constrain_angle", &Triangle::ConstrainAngle, "Sets minimum angle value.")
        .def_readwrite("min_angle", &Triangle::MinAngle, "Minimal angle of mesh element.")
        .def_readwrite("max_tria_area", &Triangle::MaxTriaArea, "Maximum triangle area constrain. a0.1")
        .def_readwrite("min_tria_area", &Triangle::MinTriaArea, "Minimum mesh element area.")
        .def_readwrite("max_edge_lenght", &Triangle::MaxEdgeLenght, "Maximal mesh element edge length.")
        .def_readwrite("min_edge_lenght", &Triangle::MinEdgeLenght, "Minimal mesh element edge length.")
        .def_readwrite("max_triangle_ratio", &Triangle::MaxTriangleRatio, "Maximum triangle ratio")
        .def_readwrite("area_constrain", &Triangle::AreaConstrain, "If true, then mesh element will be constrined to some maxiaml value.")
        .def_readwrite("custom_constraint", &Triangle::CustomConstraint, "User defined function constraint.")
        .def_readwrite("delaunay_triangles", &Triangle::DelaunayTriangles, "D - all triangles will be delaunay. Not just constrained delaunay.")
        .def_readwrite("enclose_convex_hull", &Triangle::EncloseConvexHull, "Enclose convex hull with segments.")
        .def_readwrite("check_final_mesh", &Triangle::CheckFinalMesh, "Check final mesh if it was conf with X.")
        .def_readwrite("assign_regional_attributes", &Triangle::AssignRegionalAttributes, "Assign additional regional attribute to each triangle, and specifies it to each closed segment region it belongs. (has effect with -p and without -r).")
        //.def_readwrite("algorithm", &Triangle::Algorithm)
        .def_readwrite("quite", &Triangle::Quite, "Quiet - without prints.")
        .def_readwrite("verbose", &Triangle::Verbose)
        .def_readwrite("mesh_params", &Triangle::mesh_params, "Mesh refinment object.")
    ;

    //solver mdl.solver_params, mdl.integr, mdl.boundary_conditions, false
    class_<River::Solver, boost::noncopyable>("Solver", "Deal.II Solver Wrapper.", init<const River::SolverParams &, const River::BoundaryConditions &, const bool>(args("solver_params", "boundary_conditions", "verbose")))
        .def_readwrite("tollerance", &River::Solver::tollerance, "Solver tollerance")
        .def_readwrite("number_of_iterations", &River::Solver::number_of_iterations, "Number of solver iterations.")
        .def_readwrite("verbose", &River::Solver::verbose, "If true, output will be produced to stadard output.")
        .def_readwrite("num_of_adaptive_refinments", &River::Solver::num_of_adaptive_refinments, "Number of adaptive mesh refinments. Splits mesh elements and resolves.")
        .def_readwrite("num_of_static_refinments", &River::Solver::num_of_static_refinments, "Number of static mesh refinments. Splits elements without resolving.")
        .def_readwrite("field_value", &River::Solver::field_value, "Outer field value. See Puasson, Laplace equations.")
        .def_readwrite("refinment_fraction", &River::Solver::refinment_fraction, "Refinment fraction. Used static mesh elements refinment.")
        .def_readwrite("coarsening_fraction", &River::Solver::coarsening_fraction, "Coarsening fraction. Used static mesh elements refinment.")
        .def("openMesh", &River::Solver::OpenMesh, "Open mesh data from file. Msh 2 format.")
        .def("staticRefineGrid", &River::Solver::static_refine_grid, "Static adaptive mesh refinment.")
        .def("numberOfDOFs", &River::Solver::NumberOfDOFs, "Number of refined by Deal.II mesh cells.")
        .def("run", &River::Solver::run, "Run fem solution.")
        .def("outputResults", &River::Solver::output_results, "Save results to VTK file.")
        .def("integrate", &River::Solver::integrate, "Interation of series parameters around tips points.")
        .def("solved", &River::Solver::solved)
        .def("clear", &River::Solver::clear, "Clear Solver object.")
    ;

    //riversim
    class_<ForwardRiverSimulation>("ForwardRiverSimulation", init<Model*, Triangle*, River::Solver*>(args("model", "triangle", "solver")))
        .def("linearSolver", &ForwardRiverSimulation::linear_solver)
        .def("nonLinearSolver", &ForwardRiverSimulation::non_linear_solver)
        .def("backwardSolver", &ForwardRiverSimulation::backward_solver)
    ;

    //app
    class_<App>("App")
    //    .def("Run", &App::Run)
    ;
}