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

        //implementation with json
        json j = {
            {"Description", "This is RiverSim simulation data"},

            {"RuntimeInfo", {
                {"StartDate",  time.CreationtDate()},
                {"EndDate",  time.CurrentDate()},
                {"TotalTime",  time.Total()},
                {"EachCycleTime",  time.records}}},
                //TODO add input files names

            {"Model", {
                {"eps", mdl.eps},
                {"biff_angle", mdl.biff_angle},
                {"method", (int)mdl.method},
                {"ds", mdl.ds},
                {"width", mdl.width},
                {"height", mdl.height},
                {"biffurcation_threshold", mdl.biffurcation_threshold},
                {"Rmax", mdl.Rmax},
                {"exponant", mdl.exponant},
                {"field_value", mdl.field_value},
                {"Mesh", {
                    {"r0", mdl.ac.r0},
                    {"exponant", mdl.ac.exponant},
                    {"min_area", mdl.ac.min_area}}}}},
            
            {"Border", "Implement Me!!!"},

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
            jmdl.at("eps").get_to(mdl.eps);
            jmdl.at("biff_angle").get_to(mdl.biff_angle);
            jmdl.at("method").get_to(mdl.method);
            jmdl.at("ds").get_to(mdl.ds);
            jmdl.at("width").get_to(mdl.width);
            jmdl.at("height").get_to(mdl.height);
            jmdl.at("biffurcation_threshold").get_to(mdl.biffurcation_threshold);
            jmdl.at("Rmax").get_to(mdl.Rmax);
            jmdl.at("exponant").get_to(mdl.exponant);
            jmdl.at("field_value").get_to(mdl.field_value);
            
            if(jmdl.count("Mesh"))
            {
                auto jmesh = jmdl["Mesh"];
                jmesh.at("r0").get_to(mdl.ac.r0);
                jmesh.at("exponant").get_to(mdl.ac.exponant);
                jmesh.at("min_area").get_to(mdl.ac.min_area);
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
            //TODO
            cout << j["Border"] << endl;
        }
    }

}//namespace River