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

    BoundaryCondition a, b;
    a = b;

    class_<map<int, string> >("testMap")
        .def(map_indexing_suite<map<int, string>, true>())
    ;

    class_<t_BoundaryConditions>("t_BoundaryConditions")
        .def(map_indexing_suite<t_BoundaryConditions, true>())
    ;

    class_<BoundaryConditions, bases<t_BoundaryConditions>>("BoundaryConditions")
        .def("Get", static_cast< t_BoundaryConditions (BoundaryConditions::*)(t_boundary)>(&BoundaryConditions::Get))
    ;

    class_<River::t_Sources >("t_Sources")
        .def(map_indexing_suite<River::t_Sources, true>() )
    ;

    class_<River::Sources, bases<River::t_Sources>>("Sources")
        .def("Get", &River::Sources::GetSourcesIds)
    ;

    class_<River::Line>("Line")
        .def(init<>())
        .def(init<River::Line&>(args("line")))
        .def(init<t_vert_pos, t_vert_pos, t_boundary_id>(args( "p1v", "p2v", "boundary_id")))
        .def_readwrite("p1", &River::Line::p1)
        .def_readwrite("p2", &River::Line::p2)
        .def_readwrite("boundary_id", &River::Line::boundary_id)
        .def(self == self)
    ;

    class_<River::t_PointList>("t_PointList")
        .def(vector_indexing_suite<River::t_PointList, true>())
    ;

    class_<River::t_LineList>("t_LineList")
        .def(vector_indexing_suite<River::t_LineList, true>())
    ;

    class_<River::SimpleBoundary >("SimpleBoundary")
        .def("Append", &River::SimpleBoundary::Append)
        .def("ReplaceElement", &River::SimpleBoundary::ReplaceElement)
        .def(self == self)
        .def_readwrite("name", &River::SimpleBoundary::name)
        .def_readwrite("vertices", &River::SimpleBoundary::vertices)
        .def_readwrite("lines", &River::SimpleBoundary::lines)
        .def_readwrite("inner_boundary", &River::SimpleBoundary::inner_boundary)
        .def_readwrite("holes", &River::SimpleBoundary::holes)
    ;

    class_<River::t_Boundaries >("t_Boundaries")
        .def(map_indexing_suite<River::t_Boundaries, true>())
    ;

    class_<River::Boundaries::trees_interface_t >("trees_interface_t")
        .def(map_indexing_suite<River::Boundaries::trees_interface_t, true>())
    ;

    class_<River::Boundaries, bases<River::t_Boundaries> >("Boundaries")
        .def(init<River::t_Boundaries>(args("simple_boundaries")))
        .def("MakeRectangular", &River::Boundaries::MakeRectangular)
        .def("MakeRectangularWithHole", &River::Boundaries::MakeRectangularWithHole)
        .def("Check", &River::Boundaries::Check)
        .def("GetOuterBoundary", &River::Boundaries::GetOuterBoundary, return_internal_reference<>())
        .def("GetHolesList", &River::Boundaries::GetHolesList)
        .def("GetSourcesIdsPointsAndAngles", &River::Boundaries::GetSourcesIdsPointsAndAngles)
    ;
    
    //tree
    class_<River::BranchNew>("Branch")
        .def(init<River::Point&, double>(args("source_point_val", "angle")))
        .def("AddAbsolutePoint", static_cast< BranchNew& (River::BranchNew::*)(const River::Point&)>( &River::BranchNew::AddAbsolutePoint), return_internal_reference<>())
        .def("AddAbsolutePoint", static_cast< BranchNew& (River::BranchNew::*)(const River::Polar&)>( &River::BranchNew::AddAbsolutePoint), return_internal_reference<>())
        .def("AddPoint", static_cast< BranchNew& (River::BranchNew::*)(const River::Point&)>( &River::BranchNew::AddPoint), return_internal_reference<>())
        .def("AddPoint", static_cast< BranchNew& (River::BranchNew::*)(const River::Polar&)>( &River::BranchNew::AddPoint), return_internal_reference<>())
        .def("Shrink", &River::BranchNew::Shrink, return_internal_reference<>())
        .def("RemoveTipPoint", &River::BranchNew::RemoveTipPoint, return_internal_reference<>())
        .def("TipPoint", &River::BranchNew::TipPoint)
        .def("TipVector", &River::BranchNew::TipVector)
        .def("Vector", &River::BranchNew::Vector)
        .def("TipAngle", &River::BranchNew::TipAngle)
        .def("SourcePoint", &River::BranchNew::SourcePoint)
        .def("SourceAngle", &River::BranchNew::SourceAngle)
        .def("Lenght", &River::BranchNew::Lenght)
        .def(self == self)
    ;

    class_<River::t_Tree >("t_Tree")
        .def(map_indexing_suite<River::t_Tree>())
    ;

    class_<River::Tree, bases<River::t_Tree> >("Tree")
        .def(init<>())
        .def(init<River::Tree&>())
        .def(self == self)
        .def("Initialize", &River::Tree::Initialize)
        .def("AddBranch", &River::Tree::AddBranch)
        .def("AddSubBranches", &River::Tree::AddSubBranches)
        .def("DeleteBranch", &River::Tree::DeleteBranch)
        .def("DeleteSubBranches", &River::Tree::DeleteSubBranches)
        .def("Clear", &River::Tree::Clear)
        .def("GetParentBranchId", &River::Tree::GetParentBranchId)
        .def("GetParentBranch", &River::Tree::GetParentBranch, return_internal_reference<>())
        //.def("GetSubBranchesIds", &River::Tree::GetSubBranchesIds, return_internal_reference<>())
        .def("GetAdjacentBranchId", &River::Tree::GetAdjacentBranchId)
        .def("GetAdjacentBranch", &River::Tree::GetAdjacentBranch, return_internal_reference<>())
        .def("AddPoints", &River::Tree::AddPoints)
        .def("AddPolars", &River::Tree::AddPolars)
        .def("AddAbsolutePolars", &River::Tree::AddAbsolutePolars)
        //.def("GrowTestTree", &River::Tree::GrowTestTree)
        .def("TipBranchesIds", &River::Tree::TipBranchesIds)
        .def("zero_lenght_tip_branches_ids", &River::Tree::zero_lenght_tip_branches_ids)
        .def("TipPoints", &River::Tree::TipPoints)
        .def("TipIdsAndPoints", &River::Tree::TipIdsAndPoints)
        .def("IsSourceBranch", &River::Tree::IsSourceBranch)
        .def("maximal_tip_curvature_distance", &River::Tree::maximal_tip_curvature_distance)
        .def("flatten_tip_curvature", &River::Tree::flatten_tip_curvature)
        .def("remove_tip_points", &River::Tree::remove_tip_points)
        .def("HasSubBranches", &River::Tree::HasSubBranches)
        .def("HasParentBranch", &River::Tree::HasParentBranch)
        .def("IsValidBranchId", &River::Tree::IsValidBranchId)
    ;

    //physmodel
    class_<River::t_SeriesParameters >("t_SeriesParameters")
        .def(map_indexing_suite<River::t_SeriesParameters>())
    ;

    class_<River::SimulationData >("SimulationData")
        .def(map_indexing_suite<River::SimulationData>())
    ;

    class_<River::BackwardData >("BackwardData")
        .def(self == self)
        .def_readwrite("a1", &River::BackwardData::a1)
        .def_readwrite("a2", &River::BackwardData::a2)
        .def_readwrite("a3", &River::BackwardData::a3)
        .def_readwrite("init", &River::BackwardData::init)
        .def_readwrite("backward", &River::BackwardData::backward)
        .def_readwrite("backward_forward", &River::BackwardData::backward_forward)
        .def_readwrite("branch_lenght_diff", &River::BackwardData::branch_lenght_diff)
    ;

    class_<River::t_GeometryDiffernceNew >("t_GeometryDiffernceNew")
        .def(map_indexing_suite<River::t_GeometryDiffernceNew>())
    ;

    class_<River::ProgramOptions >("ProgramOptions")
        .def(self == self)
        .def_readwrite("simulation_type", &River::ProgramOptions::simulation_type)
        .def_readwrite("number_of_steps", &River::ProgramOptions::number_of_steps)
        .def_readwrite("maximal_river_height", &River::ProgramOptions::maximal_river_height)
        .def_readwrite("number_of_backward_steps", &River::ProgramOptions::number_of_backward_steps)
        .def_readwrite("save_vtk", &River::ProgramOptions::save_vtk)
        .def_readwrite("save_each_step", &River::ProgramOptions::save_each_step)
        .def_readwrite("verbose", &River::ProgramOptions::verbose)
        .def_readwrite("debug", &River::ProgramOptions::debug)
        .def_readwrite("output_file_name", &River::ProgramOptions::output_file_name)
        .def_readwrite("input_file_name", &River::ProgramOptions::input_file_name)
    ;

    class_<River::MeshParams >("MeshParams")
        .def(self == self)
        .def_readwrite("tip_points", &River::MeshParams::tip_points)
        .def_readwrite("refinment_radius", &River::MeshParams::refinment_radius)
        .def_readwrite("exponant", &River::MeshParams::exponant)
        .def_readwrite("sigma", &River::MeshParams::sigma)
        .def_readwrite("static_refinment_steps", &River::MeshParams::static_refinment_steps)
        .def_readwrite("min_area", &River::MeshParams::min_area)
        .def_readwrite("max_area", &River::MeshParams::max_area)
        .def_readwrite("min_angle", &River::MeshParams::min_angle)
        .def_readwrite("max_edge", &River::MeshParams::max_edge)
        .def_readwrite("min_edge", &River::MeshParams::min_edge)
        .def_readwrite("ratio", &River::MeshParams::ratio)
        .def_readwrite("eps", &River::MeshParams::eps)
    ;

    class_<River::IntegrationParams >("IntegrationParams")
        .def(self == self)
        .def_readwrite("weigth_func_radius", &River::IntegrationParams::weigth_func_radius)
        .def_readwrite("integration_radius", &River::IntegrationParams::integration_radius)
        .def_readwrite("exponant", &River::IntegrationParams::exponant)
    ;

    class_<River::SolverParams >("SolverParams")
        .def(self == self)
        .def_readwrite("tollerance", &River::SolverParams::tollerance)
        .def_readwrite("num_of_iterrations", &River::SolverParams::num_of_iterrations)
        .def_readwrite("adaptive_refinment_steps", &River::SolverParams::adaptive_refinment_steps)
        .def_readwrite("refinment_fraction", &River::SolverParams::refinment_fraction)
        .def_readwrite("quadrature_degree", &River::SolverParams::quadrature_degree)
        .def_readwrite("renumbering_type", &River::SolverParams::renumbering_type)
        .def_readwrite("max_distance", &River::SolverParams::max_distance)
    ;

    class_<River::Model >("Model")
        .def(self == self)
        .def_readwrite("prog_opt", &River::Model::prog_opt)
        .def_readwrite("border", &River::Model::border)
        .def_readwrite("sources", &River::Model::sources)
        .def_readwrite("tree", &River::Model::tree)
        .def_readwrite("boundary_conditions", &River::Model::boundary_conditions)
        .def_readwrite("mesh", &River::Model::mesh)
        .def_readwrite("integr", &River::Model::integr)
        .def_readwrite("solver_params", &River::Model::solver_params)
        .def_readwrite("series_parameters", &River::Model::series_parameters)
        .def_readwrite("sim_data", &River::Model::sim_data)
        .def_readwrite("backward_data", &River::Model::backward_data)
        .def("InitializeLaplace", &River::Model::InitializeLaplace)
        .def("InitializePoisson", &River::Model::InitializePoisson)
        .def("InitializeDirichlet", &River::Model::InitializeDirichlet)
        .def("InitializeDirichletWithHole", &River::Model::InitializeDirichletWithHole)
        .def("Clear", &River::Model::Clear)
        .def("RevertLastSimulationStep", &River::Model::RevertLastSimulationStep)
        .def("SaveCurrentTree", &River::Model::SaveCurrentTree)
        .def("RestoreTree", &River::Model::RestoreTree)
        .def_readwrite("dx", &River::Model::dx)
        .def_readwrite("width", &River::Model::width)
        .def_readwrite("height", &River::Model::height)
        .def_readwrite("river_boundary_id", &River::Model::river_boundary_id)
        .def_readwrite("field_value", &River::Model::field_value)
        .def_readwrite("ds", &River::Model::ds)
        .def_readwrite("eta", &River::Model::eta)
        .def_readwrite("bifurcation_type", &River::Model::bifurcation_type)
        .def_readwrite("bifurcation_threshold", &River::Model::bifurcation_threshold)
        .def_readwrite("bifurcation_min_dist", &River::Model::bifurcation_min_dist)
        .def_readwrite("bifurcation_angle", &River::Model::bifurcation_angle)
        .def_readwrite("growth_type", &River::Model::growth_type)
        .def_readwrite("growth_threshold", &River::Model::growth_threshold)
        .def_readwrite("growth_min_distance", &River::Model::growth_min_distance)
        .def("q_growth", &River::Model::q_growth)
        .def("CheckParametersConsistency", &River::Model::CheckParametersConsistency)   
    ;

    //io
    def("ProgramTitle", River::ProgramTitle);
    def("print_ascii_signature", River::print_ascii_signature);
    def("version_string", River::version_string);
    def("print_version", River::print_version);

    //class_<cxxopts::ParseResult >("ParseResult")
    //    .def(init<>())
    //;

    //def("process_program_options", River::process_program_options);
    //def("SetupModelParamsFromProgramOptions", River::SetupModelParamsFromProgramOptions);
    def("Save", River::Save);
    def("Open", River::Open);
    //def("InitializeModelObject", River::InitializeModelObject);

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
        .def_readwrite("holelist", &triangulateio::holelist)
        .def_readwrite("regionlist", &triangulateio::regionlist)
        .def_readwrite("numberofregions", &triangulateio::numberofregions)
        .def_readwrite("edgelist", &triangulateio::edgelist)
        .def_readwrite("edgemarkerlist", &triangulateio::edgemarkerlist)
        .def_readwrite("numberofedges", &triangulateio::numberofedges)
    ;


    //boundary_generator
    //def("TreeVertices", River::TreeVertices);
    //def("TreeBoundary", River::TreeBoundary);
    //def("SimpleBoundaryGenerator", River::SimpleBoundaryGenerator);

    //mesh
    //class_<River::Triangle >("Triangle")
    //    .def("Triangle", &River::Triangle::Triangle)
    //    .def("generate", &River::Triangle::generate)
    //    .def_readwrite("Refine", &River::Triangle::Refine)
    //    .def_readwrite("ConstrainAngle", &River::Triangle::ConstrainAngle)
    //    .def_readwrite("MinAngle", &River::Triangle::MinAngle)
    //    .def_readwrite("MaxTriaArea", &River::Triangle::MaxTriaArea)
    //    .def_readwrite("MaxEdgeLenght", &River::Triangle::MaxEdgeLenght)
    //    .def_readwrite("MinEdgeLenght", &River::Triangle::MinEdgeLenght)
    //    .def_readwrite("MaxTriangleRatio", &River::Triangle::MaxTriangleRatio)
    //    .def_readwrite("AreaConstrain", &River::Triangle::AreaConstrain)
    //    .def_readwrite("CustomConstraint", &River::Triangle::CustomConstraint)
    //    .def_readwrite("DelaunayTriangles", &River::Triangle::DelaunayTriangles)
    //    .def_readwrite("EncloseConvexHull", &River::Triangle::EncloseConvexHull)
    //    .def_readwrite("CheckFinalMesh", &River::Triangle::CheckFinalMesh)
    //    .def_readwrite("AssignRegionalAttributes", &River::Triangle::AssignRegionalAttributes)
    //    .def_readwrite("Algorithm", &River::Triangle::Algorithm)
    //    .def_readwrite("Quite", &River::Triangle::Quite)
    //    .def_readwrite("Verbose", &River::Triangle::Verbose)
    //    .def_readwrite("MeshParams", &River::Triangle::mesh_params)
    //;

    //solver
    //class_<River::Solver>("Solver")
    //    .def(init<River::Model*>(args("model")))
    //    .def_readwrite("tollerance", &River::Solver::tollerance)
    //    .def_readwrite("number_of_iterations", &River::Solver::number_of_iterations)
    //    .def_readwrite("verbose", &River::Solver::verbose)
    //    .def_readwrite("num_of_adaptive_refinments", &River::Solver::num_of_adaptive_refinments)
    //    .def_readwrite("num_of_static_refinments", &River::Solver::num_of_static_refinments)
    //    .def_readwrite("field_value", &River::Solver::field_value)
    //    .def_readwrite("refinment_fraction", &River::Solver::refinment_fraction)
    //    .def_readwrite("coarsening_fraction", &River::Solver::coarsening_fraction)
    //    .def("OpenMesh", &River::Solver::clear)
    //    .def("run", &River::Solver::run)
    //    .def("clear", &River::Solver::clear)
    //    .def("output_results", &River::Solver::output_results)
    //    .def("integrate", &River::Solver::integrate)
    //;


    //riversim
    //class_<River::ForwardRiverSimulation>("Solver")
    //    .def(init<River::Model*, River::Triangle*, River::Solver*>(args("model", "triangle", "solver")))
    //    .def("linear_solver", &River::ForwardRiverSimulation::linear_solver)
    //    .def("non_linear_solver", &River::ForwardRiverSimulation::non_linear_solver)
    //    .def("backward_solver", &River::ForwardRiverSimulation::backward_solver)
    //;

    //app
    class_<App>("App")
        .def("Run", &App::Run)
    ;
}