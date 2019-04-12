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

using namespace std;
using json = nlohmann::json;

namespace River
{

    void Save(Model& mdl, Timing& time, Border& border, Tree& tr, string file_name)
    {
        if(file_name.length() == 0)
            return;

        ofstream out(file_name);
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
                {"source_point", {branch.SourcePoint().x , branch.SourcePoint().y}},
                {"source_angle", branch.SourceAngle()},
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
                {"SourceIndexes", border.sources}, 
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
                {"river-boundary-id", mdl.river_boundary_id},
                {"boundary-ids", mdl.boundary_ids}, 

                {"boundary-condition", mdl.boundary_condition},
                {"field-value", mdl.field_value},
                {"eta", mdl.eta},
                {"biffurcation-type", mdl.biffurcation_type},
                {"biffurcation-threshold", mdl.biffurcation_threshold},
                {"biffurcation-angle", mdl.biffurcation_angle},
                {"growth-type", mdl.growth_type},
                {"growth-threshold", mdl.growth_threshold},
                {"ds", mdl.ds},

                {"Integration",{
                    {"radius", mdl.integr.integration_radius},
                    {"exponant", mdl.integr.exponant}}},

                {"Mesh", {
                    {"eps", mdl.mesh.eps},
                    {"exponant", mdl.mesh.exponant},
                    {"refinment-radius", mdl.mesh.refinment_radius},
                    {"min-area", mdl.mesh.min_area},
                    {"max-area", mdl.mesh.max_area},
                    {"min-angle", mdl.mesh.min_angle}}}}},
            
            {"Border", jborder},

            {"Trees", {
                {"SourceIds", tr.source_branches_id},
                {"Relations", tr.branches_relation},
                {"Branches", branches}}}};

        out << setw(4) << j;
        out.close();
    }


    void Open(Model& mdl, Border& border, Tree& tr, string file_name)
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
            jmdl.at("river-boundary-id").get_to(mdl.river_boundary_id);
            jmdl.at("boundary-ids").get_to(mdl.boundary_ids);
            
            jmdl.at("boundary-condition").get_to(mdl.boundary_condition);
            jmdl.at("field-value").get_to(mdl.field_value);
            jmdl.at("eta").get_to(mdl.eta);
            jmdl.at("biffurcation-type").get_to(mdl.biffurcation_type);
            jmdl.at("biffurcation-threshold").get_to(mdl.biffurcation_threshold);
            jmdl.at("biffurcation-angle").get_to(mdl.biffurcation_angle);
            jmdl.at("biffurcation-threshold").get_to(mdl.biffurcation_threshold);
            jmdl.at("growth-type").get_to(mdl.growth_type);
            jmdl.at("growth-threshold").get_to(mdl.growth_threshold);
            jmdl.at("ds").get_to(mdl.ds);
            
            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl["Mesh"];

                jmesh.at("eps").get_to(mdl.mesh.eps);
                jmesh.at("exponant").get_to(mdl.mesh.exponant);
                jmesh.at("max-area").get_to(mdl.mesh.max_area);
                jmesh.at("min-area").get_to(mdl.mesh.min_area);
                jmesh.at("min-angle").get_to(mdl.mesh.min_angle);
                jmesh.at("refinment-radius").get_to(mdl.mesh.refinment_radius);
            }
            if(jmdl.count("Integration"))
            {
                auto jinteg = jmdl["Integration"];

                jinteg.at("radius").get_to(mdl.integr.integration_radius);
                jinteg.at("exponant").get_to(mdl.integr.exponant);
            }
        }
        if(j.count("Trees"))
        {
            auto jtrees = j["Trees"];
            jtrees.at("SourceIds").get_to(tr.source_branches_id);
            jtrees.at("Relations").get_to(tr.branches_relation);

            
            for (auto& [key, value] : jtrees["Branches"].items()) 
            {   
                vector<double> s_point;
                vector<pair<double, double>> coords;
                double source_angle;
                int id;
                value.at("source_point").get_to(s_point);
                value.at("source_angle").get_to(source_angle);
                value.at("coords").get_to(coords);
                value.at("id").get_to(id);
                
                BranchNew branch(River::Point{s_point.at(0), s_point.at(1)}, source_angle);
                branch.points.resize(coords.size());
                for(unsigned i = 0; i < coords.size(); ++i)
                {
                    branch.points[i] = River::Point{coords.at(i).first, coords.at(i).second};
                }
                tr.AddBranch(branch, id);
            }
            
        }
        if(j.count("Border"))
        {
            auto jborder = j["Border"];
            vector<pair<double, double>> coords;
            vector<vector<int>> lines;

            jborder.at("SourceIndexes").get_to(border.sources);
            jborder.at("coords").get_to(coords);
            jborder.at("lines").get_to(lines);

            border.vertices.reserve(coords.size());
            border.lines.reserve(lines.size());
            for(auto &p: coords)
                border.vertices.push_back({p.first, p.second});

            for(auto &l: lines)
                border.lines.push_back({(long unsigned)l.at(0)/*p1*/, (long unsigned)l.at(1)/*p2*/, l.at(2)/*id*/});
        }
    }

}//namespace River