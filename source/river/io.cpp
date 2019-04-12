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
    Model SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm)
    {
        Model mdl;

        //geometry
        mdl.width = vm["width"].as<double>();
        mdl.height = vm["height"].as<double>();
        mdl.dx = vm["dx"].as<double>();

        //model parameters
        mdl.boundary_condition = vm["boundary-condition"].as<int>();
        mdl.field_value = vm["field-value"].as<double>();
        mdl.eta = vm["eta"].as<double>();
        mdl.biffurcation_type = vm["biffurcation-type"].as<int>();
        mdl.biffurcation_threshold = vm["biffurcation-threshold"].as<double>();
        mdl.biffurcation_angle = vm["biffurcation-angle"].as<double>();
        mdl.growth_type = vm["growth-type"].as<int>();
        mdl.growth_threshold = vm["growth-threshold"].as<double>();
        mdl.ds = vm["ds"].as<double>();

        //mesh options
        mdl.mesh.eps = vm["eps"].as<double>();
        mdl.mesh.exponant = vm["mesh-exp"].as<double>();
        mdl.mesh.max_area = vm["mesh-max-area"].as<double>();
        mdl.mesh.min_area = vm["mesh-min-area"].as<double>();
        mdl.mesh.min_angle = vm["mesh-min-angle"].as<double>();
        mdl.mesh.refinment_radius = vm["refinment-radius"].as<double>();

        //integration options
        mdl.integr.integration_radius = vm["integration-radius"].as<double>();
        mdl.integr.exponant = vm["weight-exp"].as<double>();
        return mdl;
    }

    void Save(Model& mdl, Timing& time, Border& border, Tree& tr, string file_name)
    {
        if(file_name.length() == 0)
            throw invalid_argument("Save. File name is not set.");

        ofstream out(file_name + ".json");
        if(!out) throw invalid_argument("Save. Can't create file for write");


        //Branches
        json branches;
        for(auto id: tr.branches_index)
        {
            auto branch_id = id.first;
            auto& branch = tr.GetBranch(branch_id);
            vector<pair<double, double>> coords(branch.Size());
            for(unsigned i = 0; i < branch.Size(); ++i)
                coords[i] = {branch.GetPoint(i).x, branch.GetPoint(i).y};


            branches.push_back({
                {"sourcePoint", {branch.SourcePoint().x , branch.SourcePoint().y}},
                {"sourceAngle", branch.SourceAngle()},
                {"coords", coords},
                {"id", branch_id}});
        }
        
        //Border
        json jborder;
        {
            vector<pair<double, double>> coords;
            vector<vector<int>> lines;
            coords.reserve(border.vertices.size());
            coords.reserve(border.lines.size());
            for(auto& p: border.vertices)
                coords.push_back({p.x, p.y});
            for(auto& l: border.lines)
                lines.push_back({(int)l.p1, (int)l.p2, l.id});
            jborder = {
                {"SourceIds", border.sources}, 
                {"SomeDetails", "points and lines should be in counterclockwise order!"},
                {"coords", coords},
                {"lines", lines}};
        }


        //implementation with json
        json j = {
            {"Description", "This is RiverSim simulation data"},

            {"RuntimeInfo", {
                {"StartDate",  time.CreationtDate()},
                {"EndDate",  time.CurrentDate()},
                {"TotalTime",  time.Total()},
                {"EachCycleTime",  time.records},
                {"InputFile", "TODO"}}},

            {"Model", {
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
                {"biffurcationAngle", mdl.biffurcation_angle},
                {"growthType", mdl.growth_type},
                {"growthThreshold", mdl.growth_threshold},
                {"ds", mdl.ds},

                {"Integration",{
                    {"radius", mdl.integr.integration_radius},
                    {"exponant", mdl.integr.exponant}}},

                {"Mesh", {
                    {"eps", mdl.mesh.eps},
                    {"exponant", mdl.mesh.exponant},
                    {"refinmentRadius", mdl.mesh.refinment_radius},
                    {"minArea", mdl.mesh.min_area},
                    {"maxArea", mdl.mesh.max_area},
                    {"minAngle", mdl.mesh.min_angle}}}}},
            
            {"Border", jborder},

            {"Trees", {
                {"SourceIds", tr.source_branches_id},
                {"Relations", tr.branches_relation},
                {"Branches", branches}}}};

        out << setw(4) << j;
        out.close();
    }


    void Open(Model& mdl, Border& border, Tree& tree, string file_name)
    {
        ifstream in(file_name);
        if(!in) throw invalid_argument("Open. Can't create file for read.");

        json j;
        in >> j;
        if(j.count("Model"))
        {
            json jmdl = j["Model"];
            
            jmdl.at("width").get_to(mdl.width);
            jmdl.at("height").get_to(mdl.height);
            jmdl.at("dx").get_to(mdl.dx);
            jmdl.at("riverBoundaryId").get_to(mdl.river_boundary_id);
            jmdl.at("boundaryIds").get_to(mdl.boundary_ids);
            
            jmdl.at("boundaryCondition").get_to(mdl.boundary_condition);
            jmdl.at("fieldValue").get_to(mdl.field_value);
            jmdl.at("eta").get_to(mdl.eta);
            jmdl.at("biffurcationType").get_to(mdl.biffurcation_type);
            jmdl.at("biffurcationThreshold").get_to(mdl.biffurcation_threshold);
            jmdl.at("biffurcationAngle").get_to(mdl.biffurcation_angle);
            jmdl.at("biffurcationThreshold").get_to(mdl.biffurcation_threshold);
            jmdl.at("growthType").get_to(mdl.growth_type);
            jmdl.at("growthThreshold").get_to(mdl.growth_threshold);
            jmdl.at("ds").get_to(mdl.ds);
            
            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl["Mesh"];

                jmesh.at("eps").get_to(mdl.mesh.eps);
                jmesh.at("exponant").get_to(mdl.mesh.exponant);
                jmesh.at("maxArea").get_to(mdl.mesh.max_area);
                jmesh.at("minArea").get_to(mdl.mesh.min_area);
                jmesh.at("minAngle").get_to(mdl.mesh.min_angle);
                jmesh.at("refinmentRadius").get_to(mdl.mesh.refinment_radius);
            }
            if(jmdl.count("Integration"))
            {
                auto jinteg = jmdl["Integration"];

                jinteg.at("radius").get_to(mdl.integr.integration_radius);
                jinteg.at("exponant").get_to(mdl.integr.exponant);
            }
        }
        else
            throw invalid_argument("No <Model> key in JSON.");


        if(j.count("Border"))
        {
            auto jborder = j["Border"];
            vector<pair<double, double>> coords;
            vector<vector<int>> lines;

            jborder.at("SourceIds").get_to(border.sources);
            jborder.at("coords").get_to(coords);
            jborder.at("lines").get_to(lines);

            border.vertices.reserve(coords.size());
            border.lines.reserve(lines.size());
            for(auto &p: coords)
                border.vertices.push_back({p.first, p.second});

            for(auto &l: lines)
                border.lines.push_back({(long unsigned)l.at(0)/*p1*/, (long unsigned)l.at(1)/*p2*/, l.at(2)/*id*/});
        }
        else
            //if no border provided in input data
            border.MakeRectangular(
                {mdl.width, mdl.height}, 
                mdl.boundary_ids,
                {mdl.dx},
                {1});
        

        if(j.count("Trees"))
        {
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
                tree.AddBranch(branch, id);
            }
            
        }
        else
            //If no tree provided in input data
            tree.Initialize(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
        
    }

}//namespace River