#include "io.hpp"
#include "json.hpp"
///\cond
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <tuple>
///\endcond



using namespace std;
using json = nlohmann::json;

namespace River
{
    //Point
    void to_json(json& j, const Point& p) 
    {
        j = json{
            {"x", p.x}, 
            {"y", p.y}};
    }
    void from_json(const json& j, Point& p) 
    {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    //Polar
    void to_json(json& j, const Polar& p) 
    {
        j = json{
            {"r", p.r}, 
            {"phi", p.phi}};
    }
    void from_json(const json& j, Polar& p) 
    {
        j.at("r").get_to(p.r);
        j.at("phi").get_to(p.phi);
    }

    //Boundary
    //BoundaryCondition
    void to_json(json& j, const BoundaryCondition& bc) 
    {
        string type_str = "Dirichlet";
        if (bc.type == NEUMAN)
            type_str = "Neuman";

        j = json{
            {"type", type_str}, 
            {"value", bc.value}};
    }
    void from_json(const json& j, BoundaryCondition& bc) 
    {
        string type_str;

        j.at("type").get_to(type_str);

        if(type_str == "Dirichlet")
            bc.type = DIRICHLET;
        else if(type_str == "Neuman")
            bc.type = NEUMAN;
        else
            throw Exception("Unknown boundary type: " + type_str);

        j.at("value").get_to(bc.value);
    }

    //boundary conditions
    void to_json(json& j, const BoundaryConditions& bc)
    {
        j = json{{"boundary_conditions", t_BoundaryConditions(bc)}};
    }

    void from_json(const json& j, BoundaryConditions& bc)
    {
        t_BoundaryConditions bc_simple;
        j.at("boundary_conditions").get_to(bc_simple);
        for(const auto&[key, value]: bc_simple)
            bc[key] = value;
    }

    //Line
    void to_json(json& j, const Line& line) 
    {
        j = json{
            {"p1", line.p1}, 
            {"p2", line.p2},
            {"boundary_id", line.boundary_id}};
    }

    void from_json(const json& j, Line& line) 
    {
        j.at("p1").get_to(line.p1);
        j.at("p2").get_to(line.p2);
        j.at("boundary_id").get_to(line.boundary_id);
    }

    //Boundary
    void to_json(json& j, const Boundary& boundary) 
    {
        j = json{
            {"vertices", boundary.vertices}, 
            {"lines", boundary.lines}};
    }
    void from_json(const json& j, Boundary& boundary) 
    {
        j.at("vertices").get_to(boundary.vertices);
        j.at("lines").get_to(boundary.lines);
    }

    //Region
    void to_json(json& j, const RegionParams& data) 
    {
        j = json{
            {"smoothness_degree", data.smoothness_degree},
            {"ignored_smoothness_length", data.ignored_smoothness_length},
            {"river_width", data.river_width},
            {"river_boundary_id", data.river_boundary_id}};
    }
    void from_json(const json& j, RegionParams& data) 
    {
        if(j.count("smoothness_degree")) j.at("smoothness_degree").get_to(data.smoothness_degree);
        if(j.count("ignored_smoothness_length")) j.at("ignored_smoothness_length").get_to(data.ignored_smoothness_length);
        if(j.count("river_width")) j.at("river_width").get_to(data.river_width);
        if(j.count("river_boundary_id")) j.at("river_boundary_id").get_to(data.river_boundary_id);
    }

    void to_json(json& j, const Region& region) 
    {
        j = json{{"region", (t_Region)region}};
        
    }
    void from_json(const json& j, Region& region) 
    {
        t_Region r;
        j.at("region").get_to(r);
        for(const auto&[key, value]: r)
            region[key] = value;
    }

    //Sources
    void to_json(json& j, const Sources& sources) 
    {
        j = json{{"sources", (t_Sources)sources}};
    }
    void from_json(const json& j, Sources& sources) 
    {
        t_Sources s;
        j.at("sources").get_to(s);
        for(const auto&[key, value]: s)
            sources[key] = value;
    }

    //Rivers
    //Branch
    void to_json(json& j, const Branch& branch) 
    {
        j = json{
            {"vertices", (t_PointList)branch.vertices}, 
            {"lines", (t_LineList)branch.lines},
            {"source_angle", branch.SourceAngle()}
        };
    }

    void from_json(const json& j, Branch& branch) 
    {
        j.at("vertices").get_to(branch.vertices);
        j.at("lines").get_to(branch.lines);
        double source_angle;
        j.at("source_angle").get_to(source_angle);
        branch.SetSourceAngle(source_angle);
    }

    //Rivers
    void to_json(json& j, const Rivers& rivers) 
    {
        json branches;
        for(const auto&[id, branch]: rivers)
        {
            branches.push_back({
                {"id", id},
                {"branch", branch}
            });
        }

        j = json{
            {"branches", branches}, 
            {"relations", rivers.branches_relation}};
    }
    void from_json(const json& j, Rivers& rivers) 
    {
        for(const auto&[key, value]: j.at("branches").items())
        {
            t_branch_id id;
            value.at("id").get_to(id);

            Branch branch;
            value.at("branch").get_to(branch);
            
            rivers.AddBranch(branch, id);
        }

        j.at("relations").get_to(rivers.branches_relation);
    }

    //Model

    //MeshParams
    void to_json(json& j, const MeshParams& data) 
    {
        j = json{
            {"refinment_radius", data.refinment_radius},
            {"exponant", data.exponant},
            {"sigma", data.sigma},
            {"min_area", data.min_area},
            {"max_area", data.max_area},
            {"min_angle", data.min_angle},
            {"max_edge", data.max_edge},
            {"min_edge", data.min_edge},
            {"ratio", data.ratio}};
    }

    void from_json(const json& j, MeshParams& data) 
    {
        if(j.count("refinment_radius")) j.at("refinment_radius").get_to(data.refinment_radius);
        if(j.count("exponant")) j.at("exponant").get_to(data.exponant);
        if(j.count("sigma")) j.at("sigma").get_to(data.sigma);
        if(j.count("min_area")) j.at("min_area").get_to(data.min_area);
        if(j.count("max_area")) j.at("max_area").get_to(data.max_area);
        if(j.count("min_angle")) j.at("min_angle").get_to(data.min_angle);
        if(j.count("max_edge")) j.at("max_edge").get_to(data.max_edge);
        if(j.count("min_edge")) j.at("min_edge").get_to(data.min_edge);
        if(j.count("ratio")) j.at("ratio").get_to(data.ratio);
    }

    //IntegrationParams
    void to_json(json& j, const IntegrationParams& data) 
    {
        j = json{
            {"weigth_func_radius", data.weigth_func_radius},
            {"integration_radius", data.integration_radius},
            {"exponant", data.exponant},
            {"eps", data.eps},
            {"n_rho", data.n_rho}};
    }
    void from_json(const json& j, IntegrationParams& data) 
    {
        if(j.count("weigth_func_radius")) j.at("weigth_func_radius").get_to(data.weigth_func_radius);
        if(j.count("integration_radius")) j.at("integration_radius").get_to(data.integration_radius);
        if(j.count("exponant")) j.at("exponant").get_to(data.exponant);
        if(j.count("eps")) j.at("eps").get_to(data.eps);
        if(j.count("n_rho")) j.at("n_rho").get_to(data.n_rho);
    }

    //SolverParams
    void to_json(json& j, const SolverParams& data) 
    {
        j = json{
            {"field_value", data.field_value},
            {"tollerance", data.tollerance},
            {"num_of_iterrations", data.num_of_iterrations},
            {"adaptive_refinment_steps", data.adaptive_refinment_steps},
            {"static_refinment_steps", data.static_refinment_steps},
            {"refinment_fraction", data.refinment_fraction},
            {"quadrature_degree", data.quadrature_degree},
            {"renumbering_type", data.renumbering_type}
            };
    }
    
    void from_json(const json& j, SolverParams& data) 
    {
        if(j.count("field_value")) j.at("field_value").get_to(data.field_value);
        if(j.count("tollerance")) j.at("tollerance").get_to(data.tollerance);
        if(j.count("num_of_iterrations")) j.at("num_of_iterrations").get_to(data.num_of_iterrations);
        if(j.count("adaptive_refinment_steps")) j.at("adaptive_refinment_steps").get_to(data.adaptive_refinment_steps);
        if(j.count("static_refinment_steps")) j.at("static_refinment_steps").get_to(data.static_refinment_steps);
        if(j.count("refinment_fraction")) j.at("refinment_fraction").get_to(data.refinment_fraction);
        if(j.count("quadrature_degree")) j.at("quadrature_degree").get_to(data.quadrature_degree);
        if(j.count("renumbering_type")) j.at("renumbering_type").get_to(data.renumbering_type);
    }

    //Model
    void to_json(json& j, const Model& data) 
    {
        j = json{
            {"region", data.region},
            {"sources", data.sources},
            {"rivers", data.rivers},
            {"region_params", data.region_params},
            {"mesh_parameters", data.mesh_params},
            {"boundary_conditions", data.boundary_conditions},
            {"solver_parameters", data.solver_params},
            {"integration_parameters", data.integr},
            
            {"number_of_steps", data.number_of_steps},
            {"dx", data.dx},
            {"width", data.width},
            {"height", data.height},
            {"ds", data.ds},
            {"eta", data.eta},
            {"bifurcation_type", data.bifurcation_type},
            {"bifurcation_threshold", data.bifurcation_threshold},
            {"bifurcation_min_dist", data.bifurcation_min_dist},
            {"bifurcation_angle", data.bifurcation_angle},
            {"growth_type", data.growth_type},
            {"growth_threshold", data.growth_threshold},
            {"growth_min_distance", data.growth_min_distance}};
    }
    void from_json(const json& j, Model& data) 
    {   
        if(j.count("region")) j.at("region").get_to(data.region);
        if(j.count("sources")) j.at("sources").get_to(data.sources);
        if(j.count("rivers")) j.at("rivers").get_to(data.rivers);
        if(j.count("region_params")) j.at("region_params").get_to(data.region_params);
        if(j.count("mesh_parameters")) j.at("mesh_parameters").get_to(data.mesh_params);
        if(j.count("boundary_conditions")) j.at("boundary_conditions").get_to(data.boundary_conditions);
        if(j.count("solver_parameters")) j.at("solver_parameters").get_to(data.solver_params);
        if(j.count("integration_parameters")) j.at("integration_parameters").get_to(data.integr);
        
        if(j.count("number_of_steps")) j.at("number_of_steps").get_to(data.number_of_steps);
        if(j.count("dx")) j.at("dx").get_to(data.dx);
        if(j.count("width")) j.at("width").get_to(data.width);
        if(j.count("height")) j.at("height").get_to(data.height);
        if(j.count("ds")) j.at("ds").get_to(data.ds);
        if(j.count("eta")) j.at("eta").get_to(data.eta);
        if(j.count("bifurcation_type")) j.at("bifurcation_type").get_to(data.bifurcation_type);
        if(j.count("bifurcation_threshold")) j.at("bifurcation_threshold").get_to(data.bifurcation_threshold);
        if(j.count("bifurcation_min_dist")) j.at("bifurcation_min_dist").get_to(data.bifurcation_min_dist);
        if(j.count("bifurcation_angle")) j.at("bifurcation_angle").get_to(data.bifurcation_angle);
        if(j.count("growth_type")) j.at("growth_type").get_to(data.growth_type);
        if(j.count("growth_threshold")) j.at("growth_threshold").get_to(data.growth_threshold);
        if(j.count("growth_min_distance")) j.at("growth_min_distance").get_to(data.growth_min_distance);
    }

    void Save(const Model& model, const string file_name)
    {
        if(file_name.length() == 0)
            throw Exception("Save: File name is not set.");

        ofstream out(file_name);
        if(!out) throw Exception("Save: Can't create file for write");

        out.precision(16);

        json j = {{"model", model}};

        out << setw(4) << j;

        out.close();        
    }

    void Open(Model& model, const string file_name)
    {
        ifstream in(file_name);
        if(!in) throw Exception("Open. Can't open file for read. : " + file_name);

        json j;
        in >> j;

        j.at("model").get_to(model);

        if (model.sources.empty())
            throw Exception("Open: sources from input file are empty");

        if (model.boundary_conditions.empty())
            throw Exception("Open: boundary_conditions from input file are empty");

        if (model.region.empty())
            throw Exception("Open: boundaries from input file are empty");

        if (model.rivers.empty())
            model.rivers.Initialize(model.region.GetSourcesIdsPointsAndAngles(model.sources));
    }
}//namespace River