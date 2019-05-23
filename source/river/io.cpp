/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "io.hpp"
#include "json.hpp"
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>

using namespace std;
using json = nlohmann::json;

namespace River
{
    Model& SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm, Model& mdl)
    {

        //geometry
        if (vm.count("width")) mdl.width = vm["width"].as<double>();
        if (vm.count("height")) mdl.height = vm["height"].as<double>();
        if (vm.count("dx")) mdl.dx = vm["dx"].as<double>();

        //model parameters
        if (vm.count("boundary-condition")) mdl.boundary_condition = vm["boundary-condition"].as<int>();
        if (vm.count("field-value")) mdl.field_value = vm["field-value"].as<double>();
        if (vm.count("eta")) mdl.eta = vm["eta"].as<double>();
        if (vm.count("biffurcation-type")) mdl.biffurcation_type = vm["biffurcation-type"].as<int>();
        if (vm.count("biffurcation-threshold")) mdl.biffurcation_threshold = vm["biffurcation-threshold"].as<double>();
        if (vm.count("biffurcation-threshold-2")) mdl.biffurcation_threshold_2 = vm["biffurcation-threshold-2"].as<double>();
        if (vm.count("biffurcation-angle")) mdl.biffurcation_angle = vm["biffurcation-angle"].as<double>();
        if (vm.count("biffurcation-min-distance")) mdl.biffurcation_min_dist = vm["biffurcation-min-distance"].as<double>();
        if (vm.count("growth-type")) mdl.growth_type = vm["growth-type"].as<int>();
        if (vm.count("growth-threshold")) mdl.growth_threshold = vm["growth-threshold"].as<double>();
        if (vm.count("growth-min-distance")) mdl.growth_min_distance = vm["growth-min-distance"].as<double>();
        if (vm.count("ds")) mdl.ds = vm["ds"].as<double>();

        //mesh options
        if (vm.count("eps")) mdl.mesh.eps = vm["eps"].as<double>();
        if (vm.count("mesh-exp")) mdl.mesh.exponant = vm["mesh-exp"].as<double>();
        if (vm.count("mesh-max-area")) mdl.mesh.max_area = vm["mesh-max-area"].as<double>();
        if (vm.count("mesh-min-area")) mdl.mesh.min_area = vm["mesh-min-area"].as<double>();
        if (vm.count("mesh-min-angle")) mdl.mesh.min_angle = vm["mesh-min-angle"].as<double>();
        if (vm.count("refinment-radius")) mdl.mesh.refinment_radius = vm["refinment-radius"].as<double>();
        if (vm.count("mesh-sigma")) mdl.mesh.sigma = vm["mesh-sigma"].as<double>();
        if (vm.count("static-refinment-steps")) mdl.mesh.static_refinment_steps = vm["static-refinment-steps"].as<unsigned>();

        //integration options
        if (vm.count("integration-radius")) mdl.integr.integration_radius = vm["integration-radius"].as<double>();
        if (vm.count("weight-radius")) mdl.integr.weigth_func_radius = vm["weight-radius"].as<double>();
        if (vm.count("weight-exp")) mdl.integr.exponant = vm["weight-exp"].as<double>();

        //solver options
        if (vm.count("quadrature-degree")) mdl.solver_params.quadrature_degree = vm["quadrature-degree"].as<int>();
        if (vm.count("refinment-fraction")) mdl.solver_params.refinment_fraction = vm["refinment-fraction"].as<double>();
        if (vm.count("adaptive-refinment-steps")) mdl.solver_params.adaptive_refinment_steps = vm["adaptive-refinment-steps"].as<unsigned>();

        return mdl;
    }

    void Save(const Model& mdl, const Timing& time, const Border& border, const Tree& tr, const GeometryDifference &gd, const string file_name, string const input_file)
    {
        if(file_name.length() == 0)
            throw invalid_argument("Save: File name is not set.");

        ofstream out(file_name + ".json");
        if(!out) throw invalid_argument("Save: Can't create file for write");

        out.precision(16);

        //Branches
        json branches;
        for(auto id: tr.branches_index)
        {
            auto branch_id = id.first;
            auto branch = tr.GetBranch(branch_id);
            vector<pair<double, double>> coords(branch->Size());
            for(unsigned i = 0; i < branch->Size(); ++i)
                coords[i] = {branch->GetPoint(i).x, branch->GetPoint(i).y};


            branches.push_back({
                {"sourcePoint", {branch->SourcePoint().x , branch->SourcePoint().y}},
                {"sourceAngle", branch->SourceAngle()},
                {"Desciption", "Order of elements should be from source point to tip. Source point should be the same as first point of array. Source angle - represents branch growth dirrection when it consist only from one(source) point. For example perpendiculary to border line. Id should be unique(and >= 1) to each branch and is referenced in Trees->Relations structure and Border->SourcesId"},
                {"coords", coords},
                {"id", branch_id}});
        }
        
        //Border
        json jborder;
        {
            vector<pair<double, double>> coords;
            vector<vector<int>> lines;
            coords.reserve(border.GetVertices().size());
            coords.reserve(border.GetLines().size());

            for(auto& p: border.GetVertices())
                coords.push_back({p.x, p.y});

            for(auto& l: border.GetLines())
                lines.push_back({(int)l.p1, (int)l.p2, l.id});

            jborder = {
                {"SourceIds", border.GetSourceMap()}, 
                {"SomeDetails", "Points and lines should be in counterclockwise order. SourcesIDs is array of pairs - where first number - is related branch id(source branche), and second is index of related point in coords array(after initialization it will be source point of related branch). Lines consist of three numbers: first and second - point index in coords array, third - configures boundary condition(See --boundary-condition option in program: ./riversim -h)."},
                {"coords", coords},
                {"lines", lines}};
        }


        //implementation with json
        json j = {
            {"Description", "RiverSim simulation data and state of program. All coordinates are in normal cartesian coordinate system and by default are x > 0 and y > 0. Default values of simulation assumes that coordinates values will be of order 0 - 200. Greater values demands a lot of time to run, small are not tested(Problem of scalling isn't resolved yet TODO)."},
            {"Version", version_string()},

            {"RuntimeInfo", {
                {"Description", "Units are in seconds."},
                {"StartDate",  time.CreationtDate()},
                {"EndDate",  time.CurrentDate()},
                {"TotalTime",  time.Total()},
                {"EachCycleTime",  time.records},
                {"InputFile", input_file}}},

            {"Model", {
                {"Description", "All model parameters. Almost all options are described in program options: ./riversim -h. riverBoundaryId - value of boundary id of river(solution equals zero on river boundary) "},
                {"dx", mdl.dx},
                {"width", mdl.width},
                {"height", mdl.height},
                {"riverBoundaryId", mdl.river_boundary_id},
                {"boundaryIds", mdl.boundary_ids}, 

                {"boundaryCondition", mdl.boundary_condition},
                {"fieldValue", mdl.field_value},
                {"eta", mdl.eta},
                {"biffurcationType", mdl.biffurcation_type},
                {"biffurcationThreshold", mdl.biffurcation_threshold},
                {"biffurcationThreshold2", mdl.biffurcation_threshold_2},
                {"biffurcationMinDistance", mdl.biffurcation_min_dist},
                {"biffurcationAngle", mdl.biffurcation_angle},
                {"growthType", mdl.growth_type},
                {"growthThreshold", mdl.growth_threshold},
                {"growthMinDistance", mdl.growth_min_distance},
                {"ds", mdl.ds},

                {"Integration",{
                    {"radius", mdl.integr.integration_radius},
                    {"exponant", mdl.integr.exponant},
                    {"weightRadius", mdl.integr.weigth_func_radius}}},

                {"Mesh", {
                    {"eps", mdl.mesh.eps},
                    {"exponant", mdl.mesh.exponant},
                    {"refinmentRadius", mdl.mesh.refinment_radius},
                    {"minArea", mdl.mesh.min_area},
                    {"maxArea", mdl.mesh.max_area},
                    {"minAngle", mdl.mesh.min_angle},
                    {"sigma", mdl.mesh.sigma},
                    {"staticRefinmentSteps", mdl.mesh.static_refinment_steps}}},
                    
                {"Solver", {
                    {"quadratureDegree", mdl.solver_params.quadrature_degree},
                    {"refinmentFraction", mdl.solver_params.refinment_fraction},
                    {"adaptiveRefinmentSteps", mdl.solver_params.adaptive_refinment_steps}}}}
            },
            
            {"Border", jborder},

            {"Trees", {
                {"Description", "SourcesIds represents sources(or root) branches of each rivers(yes you can setup several rivers in one run). Relations is array{...} of next elements {source_branch_id, {left_child_branch_id, right_child_branch_id} it holds structure of river divided into separate branches. Order of left and right id is important."},
                {"SourceIds", tr.source_branches_id},
                {"Relations", tr.branches_relation},
                {"Branches", branches}}},
                
            {"GeometryDifference", {
                {"Description", "This structure holds info about backward river simulation. AlongBranches consist of five arrays for each branch: {branch_id: {1..}, {2..}, {3..}, {4..}, {5..}}, Where first consist of angles values allong branch(from tip to source), second - distance between tips, third - a(1) elements, forth - a(2) elements, fifth - a(3) elements. In case of --simulation-type=2, first item - integral value over whole region, second - disk integral over tip with r = 0.1, and rest are series params. BiffuractionPoints - is similar to previous object. It has same parameters but in biffurcation point. {source_branch_id: {lenght of non zero branch, which doesnt reached biffurcation point as its adjacent branch},{a(1)},{a(2)},{a(3)}}."},
                {"AlongBranches", gd.branches_series_params_and_geom_diff},
                {"BiffuractionPoints", gd.branches_biffuraction_info}}}
        };

        out << setw(4) << j;
        out.close();
    }


    void Open(Model& mdl, Border& border, Tree& tree, GeometryDifference &gd, string file_name)
    {
        ifstream in(file_name);
        if(!in) throw invalid_argument("Open. Can't create file for read.");

        json j;
        in >> j;
        if(j.count("Model"))
        {
            json jmdl = j["Model"];
            
            if (jmdl.count("width")) jmdl.at("width").get_to(mdl.width);
            if (jmdl.count("height")) jmdl.at("height").get_to(mdl.height);
            if (jmdl.count("dx")) jmdl.at("dx").get_to(mdl.dx);
            if (jmdl.count("riverBoundaryId")) jmdl.at("riverBoundaryId").get_to(mdl.river_boundary_id);
            if (jmdl.count("boundaryIds")) jmdl.at("boundaryIds").get_to(mdl.boundary_ids);
            
            if (jmdl.count("boundaryCondition")) jmdl.at("boundaryCondition").get_to(mdl.boundary_condition);
            if (jmdl.count("fieldValue")) jmdl.at("fieldValue").get_to(mdl.field_value);
            if (jmdl.count("eta")) jmdl.at("eta").get_to(mdl.eta);
            if (jmdl.count("biffurcationType")) jmdl.at("biffurcationType").get_to(mdl.biffurcation_type);
            if (jmdl.count("biffurcationThreshold")) jmdl.at("biffurcationThreshold").get_to(mdl.biffurcation_threshold);
            if (jmdl.count("biffurcationThreshold2")) jmdl.at("biffurcationThreshold2").get_to(mdl.biffurcation_threshold_2);
            if (jmdl.count("biffurcationMinDistance")) jmdl.at("biffurcationMinDistance").get_to(mdl.biffurcation_min_dist);
            if (jmdl.count("biffurcationAngle")) jmdl.at("biffurcationAngle").get_to(mdl.biffurcation_angle);
            if (jmdl.count("biffurcationThreshold")) jmdl.at("biffurcationThreshold").get_to(mdl.biffurcation_threshold);
            if (jmdl.count("growthType")) jmdl.at("growthType").get_to(mdl.growth_type);
            if (jmdl.count("growthThreshold")) jmdl.at("growthThreshold").get_to(mdl.growth_threshold);
            if (jmdl.count("growthMinDistance")) jmdl.at("growthMinDistance").get_to(mdl.growth_min_distance);
            if (jmdl.count("ds")) jmdl.at("ds").get_to(mdl.ds);
            
            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl["Mesh"];

                if (jmesh.count("eps")) jmesh.at("eps").get_to(mdl.mesh.eps);
                if (jmesh.count("exponant")) jmesh.at("exponant").get_to(mdl.mesh.exponant);
                if (jmesh.count("maxArea")) jmesh.at("maxArea").get_to(mdl.mesh.max_area);
                if (jmesh.count("minArea")) jmesh.at("minArea").get_to(mdl.mesh.min_area);
                if (jmesh.count("minAngle")) jmesh.at("minAngle").get_to(mdl.mesh.min_angle);
                if (jmesh.count("refinmentRadius")) jmesh.at("refinmentRadius").get_to(mdl.mesh.refinment_radius);
                if (jmesh.count("staticRefinmentSteps")) jmesh.at("staticRefinmentSteps").get_to(mdl.mesh.static_refinment_steps);
            }
            if(jmdl.count("Integration"))
            {
                auto jinteg = jmdl["Integration"];

                if (jinteg.count("radius")) jinteg.at("radius").get_to(mdl.integr.integration_radius);
                if (jinteg.count("exponant")) jinteg.at("exponant").get_to(mdl.integr.exponant);
                if (jinteg.count("weightRadius")) jinteg.at("weightRadius").get_to(mdl.integr.weigth_func_radius);
            }
            if(jmdl.count("Solver"))
            {
                auto jsolver = jmdl["Solver"];
                
                if (jsolver.count("quadratureDegree")) jsolver.at("quadratureDegree").get_to(mdl.solver_params.quadrature_degree);
                if (jsolver.count("refinmentFraction")) jsolver.at("refinmentFraction").get_to(mdl.solver_params.refinment_fraction);
                if (jsolver.count("adaptiveRefinmentSteps")) jsolver.at("adaptiveRefinmentSteps").get_to(mdl.solver_params.adaptive_refinment_steps);
            }
        }


        if(j.count("Border"))
        {
            auto jborder = j["Border"];
            vector<pair<double, double>> coords;
            vector<Point> points;
            vector<vector<int>> lines_raw;
            vector<Line> lines;
            map<int, long unsigned> sources;

            jborder.at("SourceIds").get_to(sources);

            jborder.at("coords").get_to(coords);
            points.reserve(coords.size());
            for(auto& c : coords)
                points.push_back(Point{c.first, c.second});

            jborder.at("lines").get_to(lines_raw);
            lines.reserve(lines_raw.size());
            for(auto& l : lines_raw)
                lines.push_back(Line{(long unsigned)l.at(0), (long unsigned)l.at(1), l.at(2)});

            border = Border{points, lines, sources};
        }
        

        if(j.count("Trees"))
        {
            if(!j.count("Border"))
                throw invalid_argument("Input json file contains Trees and do not contain Border. Make sure that you created corresponding Border object(Trees and Border should contain same source/branches ids - its values and number)");
            
            tree.Clear();

            auto jtrees = j["Trees"];
            jtrees.at("SourceIds").get_to(tree.source_branches_id);
            jtrees.at("Relations").get_to(tree.branches_relation);

            
            for(auto& [key, value] : jtrees["Branches"].items()) 
            {   
                vector<double> s_point;
                vector<pair<double, double>> coords;
                double source_angle;
                int id;

                value.at("sourcePoint").get_to(s_point);
                value.at("sourceAngle").get_to(source_angle);
                value.at("coords").get_to(coords);
                value.at("id").get_to(id);
                
                BranchNew branch(River::Point{s_point.at(0), s_point.at(1)}, source_angle);
                branch.points.resize(coords.size());

                
                for(unsigned i = 0; i < coords.size(); ++i)
                {
                    branch.points[i] = River::Point{coords.at(i).first, coords.at(i).second};
                }
                try
                {
                    tree.AddBranch(branch, id);
                }
                catch (invalid_argument& e)
                {   
                    cout << e.what() << endl;
                    cout << "tree io..ivalid inser" << endl;
                }
            }
        }
        else if(j.count("Border"))
            //If no tree provided but border is, than we reinitialize tree.. to current border.
            tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());

        if(j.count("GeometryDifference"))
        { 
            json jgd = j["GeometryDifference"];

            jgd.at("AlongBranches").get_to(gd.branches_series_params_and_geom_diff);
            jgd.at("BiffuractionPoints").get_to(gd.branches_biffuraction_info);
        }
    }
}//namespace River