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
                {"biff_angle", mdl.biff_angle},
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

    }

}//namespace River