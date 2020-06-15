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

    class_<Polar>("Polar")
        .def(init<>())
        .def(init<double, double>(args( "r", "phi" )))
        .def_readwrite("r", &Polar::r)
        .def_readwrite("phi", &Polar::phi)
        .def(self == self)
        .def(self != self)
    ;

    class_<River::Point>("Point")
        .def(init<>())
        .def(init<double, double>(args( "x", "y" )))
        .def(init<const River::Point&>())
        .def(init<const River::Polar&>())
        .def_readwrite("x", &River::Point::x)
        .def_readwrite("y", &River::Point::y)
        .def("norm", static_cast< double (River::Point::*)() const>( &River::Point::norm ) )
        //.def("norm", static_cast< double (Point::*)(const double, const double) >( &Point::norm ), args("x", "y") )
        //.staticmethod("norm")
        .def("rotate", &River::Point::rotate, args("phi"), return_value_policy<reference_existing_object>())
        .def("normalize", &River::Point::normalize, return_value_policy<reference_existing_object>())//return value policy
        .def("angle", static_cast< double (River::Point::*)() const>( &River::Point::angle ))
        //.def("angle", static_cast<double (Point::*)(double, double) >( &Point::angle ), args("x", "y") )
        //.staticmethod( "angle" )
        .def("angle", static_cast< double (River::Point::*)(const River::Point &) const>( &River::Point::angle ), args("p") )
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
    enum_<t_boundary>("t_boundary")
        .value("DIRICHLET", DIRICHLET)
        .value("NEUMAN", NEUMAN)
        .export_values()
        ;

    class_<BoundaryCondition>("BoundaryCondition")
        .def_readwrite("value", &BoundaryCondition::value)
        .def_readwrite("type", &BoundaryCondition::type)
        .def(self == self) 
    ;

    class_<t_BoundaryConditions>("t_BoundaryConditions")
        .def(map_indexing_suite<t_BoundaryConditions>())
    ;

    class_<BoundaryConditions, bases<t_BoundaryConditions>>("BoundaryConditions")
        .def("Get", static_cast< t_BoundaryConditions (BoundaryConditions::*)(t_boundary)>(&BoundaryConditions::Get))
    ;

    class_<t_source_coord >("t_source_coord")
        .def_readwrite("boundary_id", &t_source_coord::first)
        .def_readwrite("vert_pos", &t_source_coord::second)
    ;

    class_<t_sources_ids>("t_sources_ids")
        .def(vector_indexing_suite<t_sources_ids>())
    ;

    class_<t_Sources >("t_Sources")
        .def(map_indexing_suite<t_Sources>() )
    ;

    class_<Sources, bases<t_Sources>>("Sources")
        .def("getSourcesIds", &Sources::GetSourcesIds)
    ;

    class_<Line>("Line")
        .def(init<>())
        .def(init<Line&>(args("line")))
        .def(init<t_vert_pos, t_vert_pos, t_boundary_id>(args( "p1v", "p2v", "boundary_id")))
        .def_readwrite("p1", &Line::p1)
        .def_readwrite("p2", &Line::p2)
        .def_readwrite("boundary_id", &Line::boundary_id)
        .def(self == self)
    ;

    class_<t_PointList>("t_PointList")
        .def(vector_indexing_suite<t_PointList>())
    ;

    class_<t_LineList>("t_LineList")
        .def(vector_indexing_suite<t_LineList>())
    ;

    class_<SimpleBoundary >("SimpleBoundary")
        .def("Append", &SimpleBoundary::Append)
        .def("ReplaceElement", &SimpleBoundary::ReplaceElement)
        .def(self == self)
        .def_readwrite("name", &SimpleBoundary::name)
        .def_readwrite("vertices", &SimpleBoundary::vertices)
        .def_readwrite("lines", &SimpleBoundary::lines)
        .def_readwrite("inner_boundary", &SimpleBoundary::inner_boundary)
        .def_readwrite("holes", &SimpleBoundary::holes)
    ;

    class_<t_Boundaries>("t_Boundaries")
        .def(map_indexing_suite<t_Boundaries>())
    ;

    class_<t_branch_source >("t_branch_source")
        .def_readwrite("point", &t_branch_source::first)
        .def_readwrite("angle", &t_branch_source::second)
    ;

    class_<Boundaries::trees_interface_t >("trees_interface_t")
        .def(map_indexing_suite<Boundaries::trees_interface_t>())
    ;

    class_<Boundaries, bases<t_Boundaries> >("Boundaries")
        .def(init<t_Boundaries>(args("simple_boundaries")))
        .def("MakeRectangular", &Boundaries::MakeRectangular)
        .def("MakeRectangularWithHole", &Boundaries::MakeRectangularWithHole)
        .def("Check", &Boundaries::Check)
        .def("GetOuterBoundary", &Boundaries::GetOuterBoundary, return_internal_reference<>())
        .def("GetHolesList", &Boundaries::GetHolesList)
        .def("GetSourcesIdsPointsAndAngles", &Boundaries::GetSourcesIdsPointsAndAngles)
    ;
    
    //tree
    class_<BranchNew, bases<t_PointList>>("Branch")
        .def(init<River::Point&, double>(args("source_point", "angle")))
        .def("AddAbsolutePoint", static_cast< BranchNew& (BranchNew::*)(const River::Point&)>( &BranchNew::AddAbsolutePoint), return_internal_reference<>())
        .def("AddAbsolutePoint", static_cast< BranchNew& (BranchNew::*)(const Polar&)>( &BranchNew::AddAbsolutePoint), return_internal_reference<>())
        .def("AddPoint", static_cast< BranchNew& (BranchNew::*)(const River::Point&)>( &BranchNew::AddPoint), return_internal_reference<>())
        .def("AddPoint", static_cast< BranchNew& (BranchNew::*)(const Polar&)>( &BranchNew::AddPoint), return_internal_reference<>())
        .def("Shrink", &BranchNew::Shrink, return_internal_reference<>())
        .def("RemoveTipPoint", &BranchNew::RemoveTipPoint, return_internal_reference<>())
        .def("TipPoint", &BranchNew::TipPoint)
        .def("TipVector", &BranchNew::TipVector)
        .def("Vector", &BranchNew::Vector)
        .def("TipAngle", &BranchNew::TipAngle)
        .def("SourcePoint", &BranchNew::SourcePoint)
        .def("SourceAngle", &BranchNew::SourceAngle)
        .def("Lenght", &BranchNew::Lenght)
        .def(self == self)
    ;

    class_<t_Tree >("t_Tree")
        .def(map_indexing_suite<t_Tree>())
    ;

    class_<t_branch_id_pair >("t_branch_id_pair")
        .def_readwrite("left", &t_branch_id_pair::first)
        .def_readwrite("right", &t_branch_id_pair::second)
    ;

    class_<Tree, bases<t_Tree> >("Tree")
        .def(init<>())
        .def(init<Tree&>())
        .def(self == self)
        .def("Initialize", &Tree::Initialize)
        .def("AddBranch", &Tree::AddBranch)
        .def("AddSubBranches", &Tree::AddSubBranches)
        .def("DeleteBranch", &Tree::DeleteBranch)
        .def("DeleteSubBranches", &Tree::DeleteSubBranches)
        .def("Clear", &Tree::Clear)
        .def("GetParentBranchId", &Tree::GetParentBranchId)
        .def("GetParentBranch", &Tree::GetParentBranch, return_internal_reference<>())
        //.def("GetSubBranchesIds", &Tree::GetSubBranchesIds, return_internal_reference<>())
        .def("GetAdjacentBranchId", &Tree::GetAdjacentBranchId)
        .def("GetAdjacentBranch", &Tree::GetAdjacentBranch, return_internal_reference<>())
        .def("AddPoints", &Tree::AddPoints)
        .def("AddPolars", &Tree::AddPolars)
        .def("AddAbsolutePolars", &Tree::AddAbsolutePolars)
        .def("GrowTestTree", &Tree::GrowTestTree)
        .def("TipBranchesIds", &Tree::TipBranchesIds)
        .def("zero_lenght_tip_branches_ids", &Tree::zero_lenght_tip_branches_ids)
        .def("TipPoints", &Tree::TipPoints)
        .def("TipIdsAndPoints", &Tree::TipIdsAndPoints)
        .def("IsSourceBranch", &Tree::IsSourceBranch)
        .def("maximal_tip_curvature_distance", &Tree::maximal_tip_curvature_distance)
        .def("flatten_tip_curvature", &Tree::flatten_tip_curvature)
        .def("remove_tip_points", &Tree::remove_tip_points)
        .def("HasSubBranches", &Tree::HasSubBranches)
        .def("HasParentBranch", &Tree::HasParentBranch)
        .def("IsValidBranchId", &Tree::IsValidBranchId)
    ;

    //physmodel
    class_<vector<vector<double>>> ("t_v2_double")
        .def(vector_indexing_suite<vector<vector<double>>>());

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
        .def_readwrite("a1", &BackwardData::a1)
        .def_readwrite("a2", &BackwardData::a2)
        .def_readwrite("a3", &BackwardData::a3)
        .def_readwrite("init", &BackwardData::init)
        .def_readwrite("backward", &BackwardData::backward)
        .def_readwrite("backward_forward", &BackwardData::backward_forward)
        .def_readwrite("branch_lenght_diff", &BackwardData::branch_lenght_diff)
    ;

    class_<t_GeometryDiffernceNew >("t_GeometryDiffernceNew")
        .def(map_indexing_suite<t_GeometryDiffernceNew>())
    ;

    class_<ProgramOptions >("ProgramOptions")
        .def(self == self)
        .def_readwrite("simulation_type", &ProgramOptions::simulation_type)
        .def_readwrite("number_of_steps", &ProgramOptions::number_of_steps)
        .def_readwrite("maximal_river_height", &ProgramOptions::maximal_river_height)
        .def_readwrite("number_of_backward_steps", &ProgramOptions::number_of_backward_steps)
        .def_readwrite("save_vtk", &ProgramOptions::save_vtk)
        .def_readwrite("save_each_step", &ProgramOptions::save_each_step)
        .def_readwrite("verbose", &ProgramOptions::verbose)
        .def_readwrite("debug", &ProgramOptions::debug)
        .def_readwrite("output_file_name", &ProgramOptions::output_file_name)
        .def_readwrite("input_file_name", &ProgramOptions::input_file_name)
    ;

    class_<MeshParams >("MeshParams")
        .def(self == self)
        .def_readwrite("tip_points", &MeshParams::tip_points)
        .def_readwrite("refinment_radius", &MeshParams::refinment_radius)
        .def_readwrite("exponant", &MeshParams::exponant)
        .def_readwrite("sigma", &MeshParams::sigma)
        .def_readwrite("static_refinment_steps", &MeshParams::static_refinment_steps)
        .def_readwrite("min_area", &MeshParams::min_area)
        .def_readwrite("max_area", &MeshParams::max_area)
        .def_readwrite("min_angle", &MeshParams::min_angle)
        .def_readwrite("max_edge", &MeshParams::max_edge)
        .def_readwrite("min_edge", &MeshParams::min_edge)
        .def_readwrite("ratio", &MeshParams::ratio)
        .def_readwrite("eps", &MeshParams::eps)
        .def("areaConstrain", &MeshParams::operator())
    ;

    class_<IntegrationParams>("IntegrationParams")
        .def(self == self)
        .def_readwrite("weigth_func_radius", &IntegrationParams::weigth_func_radius)
        .def_readwrite("integration_radius", &IntegrationParams::integration_radius)
        .def_readwrite("exponant", &IntegrationParams::exponant)
        .def("weightFunction", &IntegrationParams::WeightFunction)
        .def("baseVectorFinal", &IntegrationParams::BaseVectorFinal)
    ;

    class_<SolverParams>("SolverParams")
        .def(self == self)
        .def_readwrite("tollerance", &SolverParams::tollerance)
        .def_readwrite("num_of_iterrations", &SolverParams::num_of_iterrations)
        .def_readwrite("adaptive_refinment_steps", &SolverParams::adaptive_refinment_steps)
        .def_readwrite("refinment_fraction", &SolverParams::refinment_fraction)
        .def_readwrite("quadrature_degree", &SolverParams::quadrature_degree)
        .def_readwrite("renumbering_type", &SolverParams::renumbering_type)
        .def_readwrite("max_distance", &SolverParams::max_distance)
    ;

    class_<Model>("Model")
        .def(self == self)
        .def_readwrite("prog_opt", &Model::prog_opt)
        .def_readwrite("border", &Model::border)
        .def_readwrite("sources", &Model::sources)
        .def_readwrite("tree", &Model::tree)
        .def_readwrite("boundary_conditions", &Model::boundary_conditions)
        .def_readwrite("mesh", &Model::mesh)
        .def_readwrite("integr", &Model::integr)
        .def_readwrite("solver_params", &Model::solver_params)
        .def_readwrite("series_parameters", &Model::series_parameters)
        .def_readwrite("sim_data", &Model::sim_data)
        .def_readwrite("backward_data", &Model::backward_data)
        .def("InitializeLaplace", &Model::InitializeLaplace)
        .def("InitializePoisson", &Model::InitializePoisson)
        .def("InitializeDirichlet", &Model::InitializeDirichlet)
        .def("InitializeDirichletWithHole", &Model::InitializeDirichletWithHole)
        .def("Clear", &Model::Clear)
        .def("RevertLastSimulationStep", &Model::RevertLastSimulationStep)
        .def("SaveCurrentTree", &Model::SaveCurrentTree)
        .def("RestoreTree", &Model::RestoreTree)
        .def_readwrite("dx", &Model::dx)
        .def_readwrite("width", &Model::width)
        .def_readwrite("height", &Model::height)
        .def_readwrite("river_boundary_id", &Model::river_boundary_id)
        .def_readwrite("field_value", &Model::field_value)
        .def_readwrite("ds", &Model::ds)
        .def_readwrite("eta", &Model::eta)
        .def_readwrite("bifurcation_type", &Model::bifurcation_type)
        .def_readwrite("bifurcation_threshold", &Model::bifurcation_threshold)
        .def_readwrite("bifurcation_min_dist", &Model::bifurcation_min_dist)
        .def_readwrite("bifurcation_angle", &Model::bifurcation_angle)
        .def_readwrite("growth_type", &Model::growth_type)
        .def_readwrite("growth_threshold", &Model::growth_threshold)
        .def_readwrite("growth_min_distance", &Model::growth_min_distance)
        .def("q_bifurcate", static_cast< bool (Model::*)(const vector<double>&) const>( &Model::q_bifurcate))
        .def("q_bifurcate", static_cast< bool (Model::*)(const vector<double>&, double branch_lenght) const>( &Model::q_bifurcate))
        .def("q_growth", &Model::q_growth)
        .def("next_point", static_cast< Polar (Model::*)(const vector<double>&, double branch_lenght, double max_a)>(&Model::next_point))
        .def("next_point", static_cast< Polar (Model::*)(const vector<double>&) const>(&Model::next_point))
        .def("CheckParametersConsistency", &Model::CheckParametersConsistency)   
    ;

    //io
    def("ProgramTitle", ProgramTitle);
    def("print_ascii_signature", print_ascii_signature);
    def("version_string", version_string);
    def("print_version", print_version);

    //class_<cxxopts::ParseResult >("ParseResult")
    //    .def(init<>())
    //;

    //def("process_program_options", River::process_program_options);
    //def("SetupModelParamsFromProgramOptions", River::SetupModelParamsFromProgramOptions);
    def("Save", Save);
    def("Open", Open);
    //def("InitializeModelObject", River::InitializeModelObject);

    //boundary_generator
    def("TreeVertices", TreeVertices);
    def("TreeBoundary", TreeBoundary);
    def("SimpleBoundaryGenerator", SimpleBoundaryGenerator);

    //tethex
    class_<tethex::Point>("TethexPoint")
        .def(init<>())
        .def(init<const double, const double, const double, const int>(args( "x", "y", "z", "region_tag")))
        .def(init<const tethex::Point&>())
        .def(init<const River::Point&>())
        .def("get_coord", &tethex::Point::get_coord)
        .def("set_coord", &tethex::Point::set_coord, args("number", "value"))
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
        .def(init<const River::SimpleBoundary&>(args("boundaries")))

        .def("read", &tethex::Mesh::read)
        .def("write", &tethex::Mesh::write)
        .def("clean", &tethex::Mesh::clean)
        .def("convert", &tethex::Mesh::convert)

        .def("get_n_vertices", &tethex::Mesh::get_n_vertices)
        .def("get_n_holes", &tethex::Mesh::get_n_holes)
        .def("get_n_points", &tethex::Mesh::get_n_points)
        .def("get_n_lines", &tethex::Mesh::get_n_lines)
        .def("get_n_triangles", &tethex::Mesh::get_n_triangles)
        .def("get_n_quadrangles", &tethex::Mesh::get_n_quadrangles)

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

    class_<Triangle >("Triangle")
        .def(init<>())
        .def(init<MeshParams*>(args("mesh_params")))

        .def("generate", &Triangle::generate)

        .def_readwrite("refine", &Triangle::Refine)
        .def_readwrite("constrain_angle", &Triangle::ConstrainAngle)
        .def_readwrite("min_angle", &Triangle::MinAngle)
        .def_readwrite("max_tria_area", &Triangle::MaxTriaArea)
        .def_readwrite("min_tria_area", &Triangle::MinTriaArea)
        .def_readwrite("max_edge_lenght", &Triangle::MaxEdgeLenght)
        .def_readwrite("min_edge_lenght", &Triangle::MinEdgeLenght)
        .def_readwrite("max_triangle_ratio", &Triangle::MaxTriangleRatio)
        .def_readwrite("area_constrain", &Triangle::AreaConstrain)
        .def_readwrite("custom_constraint", &Triangle::CustomConstraint)
        .def_readwrite("delaunay_triangles", &Triangle::DelaunayTriangles)
        .def_readwrite("enclose_convex_hull", &Triangle::EncloseConvexHull)
        .def_readwrite("check_final_mesh", &Triangle::CheckFinalMesh)
        .def_readwrite("assign_regional_attributes", &Triangle::AssignRegionalAttributes)
        //.def_readwrite("algorithm", &Triangle::Algorithm)
        .def_readwrite("quite", &Triangle::Quite)
        .def_readwrite("verbose", &Triangle::Verbose)
        .def_readwrite("mesh_params", &Triangle::mesh_params)
    ;

    //solver
    class_<River::Solver, boost::noncopyable>("Solver", init<River::Model*>(args("model")))
        .def_readwrite("tollerance", &River::Solver::tollerance)
        .def_readwrite("number_of_iterations", &River::Solver::number_of_iterations)
        .def_readwrite("verbose", &River::Solver::verbose)
        .def_readwrite("num_of_adaptive_refinments", &River::Solver::num_of_adaptive_refinments)
        .def_readwrite("num_of_static_refinments", &River::Solver::num_of_static_refinments)
        .def_readwrite("field_value", &River::Solver::field_value)
        .def_readwrite("refinment_fraction", &River::Solver::refinment_fraction)
        .def_readwrite("coarsening_fraction", &River::Solver::coarsening_fraction)
        .def("OpenMesh", &River::Solver::OpenMesh)
        .def("static_refine_grid", &River::Solver::static_refine_grid)
        .def("numberOfDOFs", &River::Solver::NumberOfDOFs)
        .def("run", &River::Solver::run)
        .def("output_results", &River::Solver::output_results)
        .def("integrate", &River::Solver::integrate)
        .def("solved", &River::Solver::solved)
        .def("clear", &River::Solver::clear)
    ;

    //riversim
    class_<ForwardRiverSimulation>("ForwardRiverSimulation", init<Model*, Triangle*, River::Solver*>(args("model", "triangle", "solver")))
        .def("linear_solver", &ForwardRiverSimulation::linear_solver)
        .def("non_linear_solver", &ForwardRiverSimulation::non_linear_solver)
        .def("backward_solver", &ForwardRiverSimulation::backward_solver)
    ;

    //app
    class_<App>("App")
    //    .def("Run", &App::Run)
    ;
}