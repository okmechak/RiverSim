#include <boost/python.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "io.hpp"

using namespace boost::python;
using namespace River;

BOOST_PYTHON_MODULE(riversim)
{
    //POINT
    def("getNormalizedPoint", GetNormalizedPoint, args("p"), "Returns normalized vector of current Point");

    def("toPolar", ToPolar, args("p"), "Returns Polar representation of vector");

    class_<Polar>("Polar", "Point in polar coordinates")
        .def(init<>())
        .def(init<double, double>(args( "r", "phi" ), "Point with r and phi coords."))
        .def_readwrite("r", &Polar::r, "r coordinate.")
        .def_readwrite("phi", &Polar::phi, "phi coordinate.")
        .def("__str__", &River::print<River::Polar>)
        .def("__repr__", &River::print<River::Polar>)
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
        .def("__str__", &River::print<River::Point>)
        .def("__repr__", &River::print<River::Point>)
    ;

    
    //BOUNDARY
    enum_<t_boundary>("t_boundary", "Enumeration of different boundary conditions.")
        .value("DIRICHLET", DIRICHLET)
        .value("NEUMAN", NEUMAN)
        .export_values()
        ;

    class_<BoundaryCondition>("BoundaryCondition", "Describes boudary conditions types for different boundary lines.")
        .def(init<>())
        .def(init<t_boundary, double>(args("type", "value")))
        .def_readwrite("type", &BoundaryCondition::type)
        .def_readwrite("value", &BoundaryCondition::value)
        .def("__str__", &River::print<BoundaryCondition>)
        .def("__repr__", &River::print<BoundaryCondition>)
        .def(self == self) 
    ;

    class_<t_BoundaryConditions>("t_BoundaryConditions", "Map structure with boundary condition types.")
        .def(map_indexing_suite<t_BoundaryConditions>())
        .def("__str__", &River::print<t_BoundaryConditions>)
        .def("__repr__", &River::print<t_BoundaryConditions>)
    ;

    class_<BoundaryConditions, bases<t_BoundaryConditions>>("BoundaryConditions", "Map structure with boundary condition types.")
        .def("Get", static_cast< t_BoundaryConditions (BoundaryConditions::*)(t_boundary) const>(&BoundaryConditions::Get), args("boundary_type"), "Returns map structure with boundary conditions of specific type(Neuman or Dirichlet)")
        .def("__str__", &River::print<BoundaryConditions>)
        .def("__repr__", &River::print<BoundaryConditions>)
    ;

    class_<t_source_coord>("t_source_coord", "Source point coordinate data type. Pair which holds boundary id and vertice position on this id.")
        .def_readwrite("boundary_id", &t_source_coord::first, "boundary id")
        .def_readwrite("vert_pos", &t_source_coord::second, "vertice position")
        .def("__str__", &River::print<t_source_coord>)
        .def("__repr__", &River::print<t_source_coord>)
    ;

    class_<t_sources_ids>("t_sources_ids", "Vector of source point ids data type.")
        .def(vector_indexing_suite<t_sources_ids>())
        .def("__str__", &River::print<t_sources_ids>)
        .def("__repr__", &River::print<t_sources_ids>)
    ;

    class_<t_Sources>("t_Sources", "Map structure which holds source id and its source point coordinates.")
        .def(map_indexing_suite<t_Sources>() )
        .def("__str__", &River::print<t_Sources>)
        .def("__repr__", &River::print<t_Sources>)
    ;

    class_<Sources, bases<t_Sources>>("Sources", "Map structure which holds source id and its source point coordinates.")
        .def("getSourcesIds", &Sources::GetSourcesIds, "Returns all sources ids.")
        .def("__str__", &River::print<Sources>)
        .def("__repr__", &River::print<Sources>)
        .def("__str__", &River::print<Sources>)
        .def("__repr__", &River::print<Sources>)
    ;

    class_<Line>("Line", "Connects two point of boundaries and holds boundary condition id.", init<>())
        .def(init<Line&>(args("line"), "Copies input line."))
        .def(init<t_vert_pos, t_vert_pos, t_boundary_id>(args( "p1v", "p2v", "boundary_id"), "Defines new line with p1v and p2v points and boudanry_id."))
        .def_readwrite("p1", &Line::p1, "First point index.")
        .def_readwrite("p2", &Line::p2, "Second point index.")
        .def_readwrite("boundary_id", &Line::boundary_id, "Boundary id.")
        .def("__str__", &River::print<Line>)
        .def("__repr__", &River::print<Line>)
        .def(self == self)
    ;

    class_<t_PointList>("t_PointList", "Vector of points.")
        .def(vector_indexing_suite<t_PointList>())
        .def("__str__", &River::print<t_PointList>)
        .def("__repr__", &River::print<t_PointList>)
    ;

    class_<t_LineList>("t_LineList", "Vector of lines.")
        .def(vector_indexing_suite<t_LineList>())
        .def("__str__", &River::print<t_LineList>)
        .def("__repr__", &River::print<t_LineList>)
    ;

    class_<Boundary>("Boundary", "Structure which defines simple boundary data structure.")
        .def(init<t_PointList, t_LineList>(args("vertices", "lines")))
        .def("append", &Boundary::Append, args("boundary"), "Appends another simple boundary at the end of current boundary.")
        .def("replaceElement", &Boundary::ReplaceElement, args("vertice_pos", "boundary"), "Replace on element of boundary with whole simple boundary structure.")
        .def("fixLinesIndices", &Boundary::FixLinesIndices, args("is_closed_boundary"), "Fixes line indices with correct one, like 0 1, 1 2, 2 3 etc.")
        .def("generateSmoothBoundary", &Boundary::generateSmoothBoundary, args("min_degree", "ignored_distance"), "Generate smooth boundaries with removed some vertices. Coarsening is controlled by min_degree and also by ignored_distance. Smoothing starts from end.")
        .def("__str__", &River::print<Boundary>)
        .def("__repr__", &River::print<Boundary>)
        .def(self == self)
        .def_readwrite("vertices", &Boundary::vertices, "Vertices of boundary vector.")
        .def_readwrite("lines", &Boundary::lines, "Connvections between boundaries.")
    ;

    class_<t_branch_source>("t_branch_source")
        .def_readwrite("point", &t_branch_source::first, "Source point of branche.")
        .def_readwrite("angle", &t_branch_source::second, "Angle of growth.")
        .def("__str__", &River::print<t_branch_source>)
        .def("__repr__", &River::print<t_branch_source>)
    ;

    class_<t_rivers_interface>("t_rivers_interface", "Interface between boundary sources and source points of Rivers.")
        .def(map_indexing_suite<t_rivers_interface>())
        .def("__str__", &River::print<t_rivers_interface>)
        .def("__repr__", &River::print<t_rivers_interface>)
    ;
    
    //RIVERS.hpp
    class_<Branch, bases<Boundary>>("Branch", "Holds all functionality that you need to work with single branch.")
        .def(init<River::Point&, double>(args("source_point", "angle")))
        .def("addAbsolutePoint", static_cast< Branch& (Branch::*)(const River::Point&, const t_boundary_id boundary_id)>( &Branch::AddAbsolutePoint), return_internal_reference<>(), args("point", "boundary_id"), "Adds point p to branch with absolute coords.")
        .def("addAbsolutePoint", static_cast< Branch& (Branch::*)(const Polar&, const t_boundary_id boundary_id)>( &Branch::AddAbsolutePoint), return_internal_reference<>(), args("polar", "boundary_id"), "Adds polar p coords to branch with absolute angle, but position is relative to tip.")
        .def("addPoint", static_cast< Branch& (Branch::*)(const River::Point&, const t_boundary_id boundary_id)>( &Branch::AddPoint), return_internal_reference<>(), args("point", "boundary_id"), "Adds point p to branch in tip relative coord system.")
        .def("addPoint", static_cast< Branch& (Branch::*)(const Polar&, const t_boundary_id boundary_id)>( &Branch::AddPoint), return_internal_reference<>(), args("polar", "boundary_id"), "Adds polar p to branch in tip relative coord and angle system.")
        .def("shrink", &Branch::Shrink, return_internal_reference<>(), args("length"), "Reduces lenght of branch by p lenght.")
        .def("removeTipPoint", &Branch::RemoveTipPoint, return_internal_reference<>(), "Remove tip point from branch(simply pops element from vector).")
        .def("tipPoint", &Branch::TipPoint, "Return TipPoint of branch(last point in branch).")
        .def("tipVector", &Branch::TipVector, "Returns vector of tip - difference between two adjacent points.")
        .def("vector", &Branch::Vector, args("index"), "Returns vector of p i-th segment of branch.")
        .def("tipAngle", &Branch::TipAngle, "Returns angle of branch tip.")
        .def("sourcePoint", &Branch::SourcePoint, "Returns source point of branch(the first one).")
        .def("getSourceAngle", &Branch::SourceAngle, "Returns source angle of branch.")
        .def("setSourceAngle", &Branch::SetSourceAngle, args("source_angle"), "Set source angle of branch.")
        .def("lenght", &Branch::Lenght, "Returns lenght of whole branch.")
        .def("generateSmoothBoundary", &Branch::generateSmoothBoundary, args("min_degree", "ignored_distance"), "Generate smooth branch with removed some vertices. Coarsening is controlled by min_degree and also by ignored_distance. Smoothing starts from end.")
        .def("__str__", &River::print<Branch>)
        .def("__repr__", &River::print<Branch>)
        .def(self == self)
    ;

    class_<t_branch_id_pair>("t_branch_id_pair")
        .def_readwrite("left", &t_branch_id_pair::first, "Left branch id.")
        .def_readwrite("right", &t_branch_id_pair::second, "Right branch id.")
        .def("__str__", &River::print<t_branch_id_pair>)
        .def("__repr__", &River::print<t_branch_id_pair>)
    ;
    
    class_<t_Rivers>("t_Rivers", "Combines branches into tree like structure.")
        .def(map_indexing_suite<t_Rivers>())
        .def("__str__", &River::print<t_Rivers>)
        .def("__repr__", &River::print<t_Rivers>)
    ;

    //Problem with reference...
    //class_<t_sub_branches_ref >("t_sub_branches_ref")
    //    .def_readwrite("left", static_cast< Branch& >(&t_sub_branches_ref::first),  "Left branch reference.")
    //    .def_readwrite("right", static_cast< Branch& >(&t_sub_branches_ref::second), "Right branch reference.")
    //;

    class_<Rivers, bases<t_Rivers> >("Rivers", "Combines branches into tree like structure.", init<>())
        .def(init<Rivers&>())
        .def(self == self)
        .def("initialize", &Rivers::Initialize, args("ids_points_angles"), "Initialize tree with source points vector and source angle vector.")
        .def("addBranch", &Rivers::AddBranch, args("branch", "branch_id"), "Adds new branch with id (id should be unique).")
        .def("addSubBranches", &Rivers::AddSubBranches, args("root_branch_id", "left_branch", "right_branch"), "Returns root(or source) branch of branch (if there is no such - throw exception).")
        .def("deleteBranch", &Rivers::DeleteBranch, args("branch_id"), "Delete branch.")
        .def("deleteSubBranches", &Rivers::DeleteSubBranches, args("root_branch_id"), "Delete sub branch of current branch")
        .def("clear", &Rivers::Clear, "Clear whole data from rivers.")
        .def("getParentBranchId", &Rivers::GetParentBranchId, args("branch_id"), "Returns link to parent branch.")
        .def("getParentBranch", &Rivers::GetParentBranch, return_internal_reference<>(), args("branch_id"), "Returns link to parent branch.")
        .def("getSubBranchesIds", &Rivers::GetSubBranchesIds, args("branch_id"), "Returns pair of ids of subranches.")
        //Problem with references
        //.def("getSubBranches", &Rivers::GetSubBranches, return_internal_reference<>())
        .def("getAdjacentBranchId", &Rivers::GetAdjacentBranchId, args("branch_id"), "Returns id of adjacent branch to current branch_id branch.")
        .def("getAdjacentBranch", &Rivers::GetAdjacentBranch, return_internal_reference<>(), args("branch_id"), "Returns link to adjacent branch with id.")
        .def("addPoints", &Rivers::AddPoints, args("tips_id", "points", "boundary_ids"), "Adds  relatively vector of points to Branches tips_id.")
        .def("addPolars", &Rivers::AddPolars, args("tips_id", "polars", "boundary_ids"), "Adds  relatively points to Branches tips_id.")
        .def("addAbsolutePolars", &Rivers::AddAbsolutePolars, args("tips_id", "polars", "boundary_ids"), "Adds  absolute points to Branches tips_id.")
        .def("growTestTree", &Rivers::GrowTestTree, args("boundary_id", "branch_id", "ds", "n", "dalpha"))
        .def("tipBranchesIds", &Rivers::TipBranchesIds, "Returns vector of tip branches ids.")
        .def("zeroLenghtTipBranchesIds", &Rivers::zero_lenght_tip_branches_ids, args("zero_lenght"))
        .def("tipPoints", &Rivers::TipPoints, "Returns vector of tip branches Points.")
        .def("tipIdsAndPoints", &Rivers::TipIdsAndPoints, "Returns vector of tip branches Points and its ids.")
        .def("isSourceBranch", &Rivers::IsSourceBranch, args("branch_id"), "Checks if current id of branch is source or not.")
        .def("maximalTipCurvatureDistance", &Rivers::maximal_tip_curvature_distance, "Evaluates curvature of tips. Used in non-euler growth.")
        .def("flattenTipCurvature", &Rivers::flatten_tip_curvature)
        .def("removeTipPoints", &Rivers::remove_tip_points, "Removes tips points, or in other words reverts one step of simulation.")
        .def("hasSubBranches", &Rivers::HasSubBranches, args("branch_id"), "Checks if Branch branch_id has subbranches.")
        .def("hasParentBranch", &Rivers::HasParentBranch, args("sub_branch_id"))
        .def("isValidBranchId", &Rivers::IsValidBranchId, args("id"), "Checks for validity of id.")
        .def("__str__", &River::print<Rivers>)
        .def("__repr__", &River::print<Rivers>)
        .def_readwrite("branches_relation", &Rivers::branches_relation, "Holds realations between root branhces and its subbranches.")
    ;

    
    //REGION.hpp
    class_<RegionParams>("RegionParams")
        .def_readwrite("smoothness_degree", &RegionParams::smoothness_degree, "Smoothnes minimal degree. This value sets threshold for degree between adjacent points below which it should be ignored. This creates smaller mesh.")
        .def_readwrite("ignored_smoothness_length", &RegionParams::ignored_smoothness_length, "Smoothnes minimal length. This value sets threshold for length where smoothnest near tip will be ignored. Ideally it should be bigger then integration radius.")
        .def_readwrite("river_width", &RegionParams::river_width)
        .def_readwrite("river_boundary_id", &RegionParams::river_boundary_id, "Sigma is used in exponence formula of mesh refinment.")
        .def(self == self)
        .def("__str__", &River::print<MeshParams>)
        .def("__repr__", &River::print<MeshParams>)
    ;

    class_<t_Region>("t_Region", "Structure which defines Region of region.")
        .def(map_indexing_suite<t_Region>())
        .def("__str__", &River::print<t_Region>)
        .def("__repr__", &River::print<t_Region>)
    ;

    class_<Region, bases<t_Region> >("Region", "Structure which defines Region of region.")
        .def(init<t_Region>(args("boundaries"), "Constructor"))
        .def("makeRectangular", &Region::MakeRectangular, args("width", "height", "source_x_position"), "Initialize rectangular Region.")
        .def("makeRectangularWithHole", &Region::MakeRectangularWithHole, args("width", "height", "source_x_position"), "Initialize rectangular with hole Region.")
        .def("check", &Region::Check, "Some basic checks of data structure.")
        .def("getHolesList", &Region::GetHolesList, "Returns vector of all holes.")
        .def("getSourcesIdsPointsAndAngles", &Region::GetSourcesIdsPointsAndAngles, args("sources"), "Returns map of source points ids and coresponding source point and angle of tree Branch.")
        .def("normalAngle", &Region::NormalAngle, args("left_point", "center_point", "right_point"), "Returns normal angle based on three points.")
        .def("getAdjacentVerticesPositions", &Region::GetAdjacentVerticesPositions, args("vertices_size", "vertice_pos"), "Returns andjancent vertice indexes.")
        .def("getVerticeNormalAngle", &Region::GetVerticeNormalAngle, args("vertices", "vertice_pos"), "Returns normal angle at point with vertice_pos index.")
        .def("__str__", &River::print<Region>)
        .def("__repr__", &River::print<Region>)
        .def_readwrite("holes", &River::Region::holes, "Array of holes. Which will be eliminated by mesh generator.")
    ;

    def("RiversBoundary", River::RiversBoundary, args("rivers_boundary", "rivers", "river_id", "rp"), "Generates rivers boundary.");
    def("BoundaryGenerator", River::BoundaryGenerator, args("sources", "region", "rivers", "rp"), "Generates boundary from region and rivers.");


    //TETHEX.hpp
    class_<tethex::Point>("TethexPoint",  init<>())
        .def(init<const double, const double, const double, const int>(args( "x", "y", "z", "region_tag")))
        .def(init<const tethex::Point&>())
        .def(init<const River::Point&>())
        .def("getCoord", &tethex::Point::get_coord, args("number"))
        .def("setCoord", &tethex::Point::set_coord, args("number", "value"))
        .def(self == self)
        //.def("__str__", &River::print<tethex::Point>)
        //.def("__repr__", &River::print<tethex::Point>)
    ;

    class_<vector<tethex::Point>> ("t_Vector_of_TethexPoint")
        .def(vector_indexing_suite<vector<tethex::Point>>())
        //.def("__str__", &River::print<vector<tethex::Point>>)
        //.def("__repr__", &River::print<vector<tethex::Point>>)
    ;

    class_<tethex::PhysPoint>("PhysPoint")
        .def(init<>())
        .def(init<const vector<int> &, int>(args("ver", "mat_id")))
        .def(init<const int, int>(args("ver", "mat_id")))
        .def("get_n_vertices", &tethex::MeshElement::get_n_vertices)
        .def("get_n_edges", &tethex::MeshElement::get_n_edges)
        .def("get_vertex", &tethex::MeshElement::get_vertex, args("number"))
        .def("set_vertex", &tethex::MeshElement::set_vertex, args("local_number", "global_number"))
        .def("get_edge", &tethex::MeshElement::get_edge, args("number"))
        .def("get_gmsh_el_type", &tethex::MeshElement::get_gmsh_el_type)
        .def("get_material_id", &tethex::MeshElement::get_material_id)
        .def("set_edge", &tethex::MeshElement::set_edge, args("local_number", "global_number"))
        .def("contains", &tethex::MeshElement::contains, args("vertex"))
        .def("measure", &tethex::MeshElement::measure, args("points"))
        .def("min_angle", &tethex::MeshElement::min_angle, args("points"))
        .def("max_angle", &tethex::MeshElement::max_angle, args("points"))
        .def("min_edge", &tethex::MeshElement::min_edge, args("points"))
        .def("max_edge", &tethex::MeshElement::max_edge, args("points"))
        .def("quality", &tethex::MeshElement::quality, args("points"))
    ;

    class_<tethex::Line>("PhysLine")
        .def(init<>())
        .def(init<const vector<int> &, int>(args("ver", "mat_id")))
        .def(init<const int, int>(args("ver", "mat_id")))
        .def(init<int, int, int>(args("v1", "v2", "mat_id")))
        .def(init<const River::Line&>(args("line")))
        .def("get_n_vertices", &tethex::MeshElement::get_n_vertices)
        .def("get_n_edges", &tethex::MeshElement::get_n_edges)
        .def("get_vertex", &tethex::MeshElement::get_vertex, args("number"))
        .def("set_vertex", &tethex::MeshElement::set_vertex, args("local_number", "global_number"))
        .def("get_edge", &tethex::MeshElement::get_edge, args("number"))
        .def("get_gmsh_el_type", &tethex::MeshElement::get_gmsh_el_type)
        .def("get_material_id", &tethex::MeshElement::get_material_id)
        .def("set_edge", &tethex::MeshElement::set_edge, args("local_number", "global_number"))
        .def("contains", &tethex::MeshElement::contains, args("vertex"))
        .def("measure", &tethex::MeshElement::measure, args("points"))
        .def("min_angle", &tethex::MeshElement::min_angle, args("points"))
        .def("max_angle", &tethex::MeshElement::max_angle, args("points"))
        .def("min_edge", &tethex::MeshElement::min_edge, args("points"))
        .def("max_edge", &tethex::MeshElement::max_edge, args("points"))
        .def("quality", &tethex::MeshElement::quality, args("points"))
    ;

    class_<tethex::Triangle>("PhysTriangle")
        .def(init<>())
        .def(init<const vector<int> &, int>(args("ver", "mat_id")))
        .def(init<int, int, int, int>(args("v1", "v2", "v3", "mat_id")))
        .def("get_n_vertices", &tethex::MeshElement::get_n_vertices)
        .def("get_n_edges", &tethex::MeshElement::get_n_edges)
        .def("get_vertex", &tethex::MeshElement::get_vertex, args("number"))
        .def("set_vertex", &tethex::MeshElement::set_vertex, args("local_number", "global_number"))
        .def("get_edge", &tethex::MeshElement::get_edge, args("number"))
        .def("get_gmsh_el_type", &tethex::MeshElement::get_gmsh_el_type)
        .def("get_material_id", &tethex::MeshElement::get_material_id)
        .def("set_edge", &tethex::MeshElement::set_edge, args("local_number", "global_number"))
        .def("contains", &tethex::MeshElement::contains, args("vertex"))
        .def("measure", &tethex::MeshElement::measure, args("points"))
        .def("min_angle", &tethex::MeshElement::min_angle, args("points"))
        .def("max_angle", &tethex::MeshElement::max_angle, args("points"))
        .def("min_edge", &tethex::MeshElement::min_edge, args("points"))
        .def("max_edge", &tethex::MeshElement::max_edge, args("points"))
        .def("quality", &tethex::MeshElement::quality, args("points"))
    ;

    class_<tethex::Mesh>("TethexMesh")
        .def(init<>())
        .def(init<const tethex::Mesh&>(args("mesh")))
        .def(init<const River::Boundary&, const River::t_PointList&>(args("boundary", "holes")))

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
        //.def("__str__", &River::print<vector<tethex::Mesh>>)
        //.def("__repr__", &River::print<vector<tethex::Mesh>>)

    //    .def("get_vertex", &tethex::Mesh::get_vertex, args("number"), return_internal_reference<>())
    //    .def("get_hole", &tethex::Mesh::get_hole, args("number"), return_internal_reference<>())
    //    .def("get_point", &tethex::Mesh::get_point, args("number"), return_internal_reference<>())
    //    .def("get_edge", &tethex::Mesh::get_edge, args("number"), return_internal_reference<>())
    //    .def("get_line", &tethex::Mesh::get_line, args("number"), return_internal_reference<>())
    //    .def("get_triangle", &tethex::Mesh::get_triangle, args("number"), return_internal_reference<>())
    //    .def("get_quadrangle", &tethex::Mesh::get_quadrangle, args("number"), return_internal_reference<>())
    ;


    //TRIANGLE_C.hpp
    class_<triangulateio>("triangulateio")
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

    class_<MeshParams>("MeshParams")
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
        .def("meshAreaConstraint", &MeshParams::meshAreaConstraint, args("x", "y"), "Evaluates mesh area constraint at {x, y} point.")
        .def("refinementFunction", &MeshParams::refinementFunction, args("p1", "p2", "p3", "area"), "Specifies a function to indicate whether mesh cells should be refined or not.")
        .def("__str__", &River::print<MeshParams>)
        .def("__repr__", &River::print<MeshParams>)
    ;


    //TRIANGLE.hpp
    class_<Triangle>("Triangle", init<>())
        .def(init<MeshParams>(args("mesh_params")))
        .def("generate", &Triangle::generate_quadrangular_mesh, args("boundary", "holes"), "Generate mesh.")
        .def("printOptions", &Triangle::print_options, args("qDetailedDescription"), "Outupts options with or without detailed description.")
        .def_readwrite("refine", &Triangle::Refine, "Refine previously generated mesh, with preserving of segments")
        .def_readwrite("constrain_angle", &Triangle::ConstrainAngle, "Sets minimum angle value.")
        .def_readwrite("min_angle", &Triangle::MinAngle, "Minimal angle of mesh element.")
        .def_readwrite("max_tria_area", &Triangle::MaxTriaArea, "Maximum triangle area constrain. a0.1")
        .def_readwrite("min_tria_area", &Triangle::MinTriaArea, "Minimum mesh element area.")
        .def_readwrite("max_edge_lenght", &Triangle::MaxEdgeLenght, "Maximal mesh element edge length.")
        .def_readwrite("min_edge_lenght", &Triangle::MinEdgeLenght, "Minimal mesh element edge length.")
        .def_readwrite("max_triangle_ratio", &Triangle::MaxTriangleRatio, "Maximum triangle ratio")
        .def_readwrite("area_constrain", &Triangle::AreaConstrain, "If true, then mesh element will be constrined to some maxiaml value.")
        //.def_readwrite("custom_constraint", &Triangle::CustomConstraint, "User defined function constraint.")
        .def_readwrite("delaunay_triangles", &Triangle::DelaunayTriangles, "D - all triangles will be delaunay. Not just constrained delaunay.")
        .def_readwrite("enclose_convex_hull", &Triangle::EncloseConvexHull, "Enclose convex hull with segments.")
        .def_readwrite("check_final_mesh", &Triangle::CheckFinalMesh, "Check final mesh if it was conf with X.")
        .def_readwrite("assign_regional_attributes", &Triangle::AssignRegionalAttributes, "Assign additional regional attribute to each triangle, and specifies it to each closed segment region it belongs. (has effect with -p and without -r).")
        //.def_readwrite("algorithm", &Triangle::Algorithm)
        .def_readwrite("quite", &Triangle::Quite, "Quiet - without prints.")
        .def_readwrite("verbose", &Triangle::Verbose)
        .def_readwrite("mesh_params", &Triangle::mesh_params, "Mesh refinment object.")
        //.def("__str__", &Triangle::print_options, "Outupts options with or without detailed description.")
        //.def("__repr__", &Triangle::print_options, "Outupts options with or without detailed description.")
    ;

    //SOLVER.hpp
    class_<SolverParams>("SolverParams")
        .def(self == self)
        .def_readwrite("tollerance", &SolverParams::tollerance, "Tollerarnce used by dealii Solver.")
        .def_readwrite("num_of_iterrations", &SolverParams::num_of_iterrations, "Number of solver iteration steps.")
        .def_readwrite("adaptive_refinment_steps", &SolverParams::adaptive_refinment_steps, "Number of adaptive refinment steps.")
        .def_readwrite("static_refinment_steps", &SolverParams::static_refinment_steps, "Number of mesh refinment steps used by Deal.II mesh functionality.")
        .def_readwrite("refinment_fraction", &SolverParams::refinment_fraction, "Fraction of refined mesh elements.")
        .def_readwrite("quadrature_degree", &SolverParams::quadrature_degree, "Polynom degree of quadrature integration.")
        .def_readwrite("renumbering_type", &SolverParams::renumbering_type, "Renumbering algorithm(0 - none, 1 - cuthill McKee, 2 - hierarchical, 3 - random, ...) for the degrees of freedom on a triangulation.")
        .def_readwrite("field_value", &SolverParams::field_value, "Field value used for Poisson conditions.")
        .def("__str__", &River::print<SolverParams>)
        .def("__repr__", &River::print<SolverParams>)
    ;

    class_<IntegrationParams>("IntegrationParams")
        .def(self == self)
        .def_readwrite("weigth_func_radius", &IntegrationParams::weigth_func_radius, "Circle radius with centrum in tip point.")
        .def_readwrite("integration_radius", &IntegrationParams::integration_radius, "Circle radius with centrum in tip point.")
        .def_readwrite("exponant", &IntegrationParams::exponant, "Controls slope.")
        .def_readwrite("eps", &IntegrationParams::eps, "Series params integral precision.")
        .def_readwrite("n_rho", &IntegrationParams::n_rho, "Rho integration step.")
        .def("weightFunction", &IntegrationParams::WeightFunction, "Weight function used in computation of series parameters.")
        .def("baseVectorFinal", &IntegrationParams::BaseVectorFinal, "Base Vector function used in computation of series parameters.")
        .def("__str__", &River::print<IntegrationParams>)
        .def("__repr__", &River::print<IntegrationParams>)
    ;

    class_<River::Solver, boost::noncopyable>("Solver", "Deal.II Solver Wrapper.", init<const River::SolverParams &>(args("solver_params")))
        .def_readwrite("tollerance", &River::Solver::tollerance, "Solver tollerance")
        .def_readwrite("number_of_iterations", &River::Solver::number_of_iterations, "Number of solver iterations.")
        .def_readwrite("num_of_adaptive_refinments", &River::Solver::num_of_adaptive_refinments, "Number of adaptive mesh refinments. Splits mesh elements and resolves.")
        .def_readwrite("num_of_static_refinments", &River::Solver::num_of_static_refinments, "Number of static mesh refinments. Splits elements without resolving.")
        .def_readwrite("field_value", &River::Solver::field_value, "Outer field value. See Puasson, Laplace equations.")
        .def_readwrite("refinment_fraction", &River::Solver::refinment_fraction, "Refinment fraction. Used static mesh elements refinment.")
        .def_readwrite("coarsening_fraction", &River::Solver::coarsening_fraction, "Coarsening fraction. Used static mesh elements refinment.")
        .def("openMesh", static_cast< void (River::Solver::*)(const tethex::Mesh&)>( &River::Solver::OpenMesh), args("mesh"), "Open mesh data from tethex::mesh object.")
        .def("openMeshFromFile", static_cast< void (River::Solver::*)(const string fileName)>( &River::Solver::OpenMesh), args("file_name"), "Open mesh data from file. Msh 2 format.")
        .def("staticRefineGrid", &River::Solver::static_refine_grid, "Static adaptive mesh refinment.")
        .def("setBoundaryConditions", &River::Solver::setBoundaryConditions, args("boundary_conditions"), "Set boundary conditions.")
        .def("numberOfDOFs", &River::Solver::NumberOfDOFs, "Number of degree of freedom.")
        .def("numberOfCells", &River::Solver::NumberOfRefinedCells, "Number of refined by Deal.II mesh cells.")
        .def("run", &River::Solver::run, "Run fem solution.")
        .def("outputResults", &River::Solver::output_results, args("file_name"), "Save results to VTK file.")
        .def("integrate", &River::Solver::integrate, args("integ", "point", "angle"), "Interation of series parameters around tips points.")
        .def("integrate_new", &River::Solver::integrate_new, args("integ", "point", "angle"), "Interation of series parameters around tips points.")
        .def("integrate_trap", &River::Solver::integrate_trap, args("integ", "point", "angle"), "Interation of series parameters around tips points using thrapezoidla method.")
        .def("integral_value_res", &River::Solver::integral_value_res, args("rho", "phi", "tip_coord", "angle", "integ"), "Function used in series parameters evaluation")
        .def("solved", &River::Solver::solved)
        .def("clear", &River::Solver::clear, "Clear Solver object.")
        .def("setupSystem", &River::Solver::setup_system, "setup_system.")
        .def("assembleSystem", &River::Solver::assemble_system, "assemble_system.")
        .def("solve", &River::Solver::solve, "solve.")
        .def("value", &River::Solver::value, args("p"), "Returns value of solution at specific coord p.")
        .def("valueSafe", &River::Solver::valueSafe, args("x", "y"), "Returns value of solution at specific coord {x, y} and handles exceptions.")
        .def("refineGrid", &River::Solver::refine_grid, "refine_grid.")
        .def("maxCellError", &River::Solver::max_cell_error, "Returns maximal error over all cells.")
        .def("averageCellError", &River::Solver::average_cell_error, "Returns maximal error over all cells.")
    ;

    //MODEL.hpp
    //class_<vector<double>> ("t_v_double")
    //    .def(vector_indexing_suite<vector<double>>())
    //    .def("__str__", &River::print<vector<double>>)
    //    .def("__repr__", &River::print<vector<double>>)
    //;

    //class_<vector<vector<double>>> ("t_v2_double")
    //    .def(vector_indexing_suite<vector<vector<double>>>())
    //    .def("__str__", &River::print<vector<vector<double>>>)
    //    .def("__repr__", &River::print<vector<vector<double>>>)
    //;

    class_<Model>("Model")
        .def("initializeLaplace", &Model::InitializeLaplace, "Initialize problem to Laplacea boundary conditions.")
        .def("initializePoisson", &Model::InitializePoisson, "Initialize problem to Poisson boundary conditions.")
        .def("initializeDirichlet", &Model::InitializeDirichlet, "Initialize proble to Dirichlet boundary conditions.")
        .def("initializeDirichletWithHole", &Model::InitializeDirichletWithHole, "Intialize problem to Dirirchlet boundary conditions with hole.")
        .def("clear", &Model::clear, "Clear all data")
        .def("qBifurcate", static_cast< bool (Model::*)(const vector<double>&) const>( &Model::q_bifurcate), "Checks by evaluating series params for bifuraction condition.")
        .def("qBifurcate", static_cast< bool (Model::*)(const vector<double>&, double branch_lenght) const>( &Model::q_bifurcate), "Checks by evaluating series params for bifuraction condition.")
        .def("qGrowth", &Model::q_growth, "Growth condition. Checks series parameters around river tip and evaluates if it is enough to grow.")
        .def("nextPoint", static_cast< Polar (Model::*)(const vector<double>&, double branch_lenght, double max_a)>(&Model::next_point), "Evaluate next point of simualtion based on series parameters around tip.")
        .def("nextPoint", static_cast< Polar (Model::*)(const vector<double>&) const>(&Model::next_point), "Evaluate next point of simualtion based on series parameters around tip.")
        .def(self == self)
        .def_readwrite("region_params", &Model::region_params, "Region and geometry parameters.")
        .def_readwrite("region", &Model::region, "Region of simulation.")
        .def_readwrite("boundary", &Model::boundary, "Region converted to boundary of simulation.")
        .def_readwrite("sources", &Model::sources, "Source points of growth.")
        .def_readwrite("rivers", &Model::rivers, "Rivers data structure.")
        .def_readwrite("mesh_params", &Model::mesh_params, "Mesh parameters.")
        .def_readwrite("boundary_conditions", &Model::boundary_conditions, "Boundary conditions data structure.")
        .def_readwrite("solver_params", &Model::solver_params, "Solver parameteres.")
        .def_readwrite("integr_params", &Model::integr, "Intergration around tip points parameters.")
        .def_readwrite("number_of_steps", &Model::number_of_steps, "Number of simulation steps.")
        .def_readwrite("dx", &Model::dx, "Initial x position of source.")
        .def_readwrite("width", &Model::width, "Width of region.")
        .def_readwrite("height", &Model::height, "Height of region.")
        .def_readwrite("ds", &Model::ds, "Maximal length of one step of growth.")
        .def_readwrite("eta", &Model::eta, "Eta. Growth power of a1^eta.")
        .def_readwrite("bifurcation_type", &Model::bifurcation_type, "Bifurcation method type, 0 - no bif, 1 - a(3)/a(1) > bifurcation_threshold, 2 - a1 > bifurcation_threshold, 3 - combines both conditions.")
        .def_readwrite("bifurcation_threshold", &Model::bifurcation_threshold, "Bifurcation threshold for 0 bifurcation type.")
        .def_readwrite("bifurcation_min_dist", &Model::bifurcation_min_dist, "Minimal distance between adjacent bifurcation points. Reduces numerical noise.")
        .def_readwrite("bifurcation_angle", &Model::bifurcation_angle, "Bifurcation angle.")
        .def_readwrite("growth_type", &Model::growth_type, "Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}")
        .def_readwrite("growth_threshold", &Model::growth_threshold, "Growth of branch will be done only if a1 > growth-threshold.")
        .def_readwrite("growth_min_distance", &Model::growth_min_distance, "Distance of constant tip growing after bifurcation point. Reduces numerical noise.")
        .def("__str__", &River::print<Model>)
        .def("__repr__", &River::print<Model>)
    ;

    //IO.hpp
    def("save", Save, args("model", "file_name"), "Saves current program state to file.");
    def("open", Open, args("model", "file_name"), "Opens program state from file.");
}