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

#include "riversim.hpp"

#include <math.h>

namespace River
{
    //FIXME this will be walid only for quad region
    //FIXME handle case ymin too!
    bool StopConditionOfRiverGrowth(Border& border, Tree& tree)
    {
        auto tips = tree.TipPoints();

        auto border_init_point = border.vertices.back(),
            tip_init_point = tips.back();

        double xmin = tip_init_point.x, 
            xmax = xmin, 
            ymax = tip_init_point.y, 
            rxmin = border_init_point.x, 
            rxmax = rxmin, 
            rymax = border_init_point.y;

        for(auto &p :border.vertices)
        {
            if(p.x > rxmax)
                rxmax = p.x;
            if(p.x < rxmin)
                rxmin = p.x;
            if(p.y > rymax)
                rymax = p.y;
        }

        for(auto &p :tips)
        {
            if(p.x > xmax)
                xmax = p.x;
            if(p.x < xmin)
                xmin = p.x;
            if(p.y > ymax)
                ymax = p.y;
        }
        //FIXME eps should depend on elementary step size.
        double dl = 0.007;
        return xmax > rxmax - dl || ymax > rymax - dl || xmin < rxmin + dl;
    }




    void ForwardRiverEvolution(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, string file_name)
    {
        //initial boundaries of mesh
        auto mesh = BoundaryGenerator(mdl, tree, border);
        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);//triangulation
        mesh.convert();//convertaion from triangles to quadrangles
        mesh.write(file_name + ".msh");

        //Simulation
        //Deal.II library
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.run();
        sim.output_results(file_name);

        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id)->TipPoint();
            auto tip_angle = tree.GetBranch(id)->TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);

            if(mdl.q_growth(series_params))
            {
                if(mdl.q_biffurcate(series_params, tree.GetBranch(id)->Lenght()))
                {
                    auto br_left = BranchNew(tip_point, tip_angle + mdl.biffurcation_angle);
                    br_left.AddPoint(Polar{mdl.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - mdl.biffurcation_angle);
                    br_right.AddPoint(Polar{mdl.ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.GetBranch(id)->AddPoint(mdl.next_point(series_params, tree.GetBranch(id)->Lenght()));
            }
        }
        sim.clear();
    }




    bool BackwardRiverEvolution(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, GeometryDifference& gd, string file_name)
    {   
        bool stop_flag = false;
        auto mesh = BoundaryGenerator(mdl, tree, border);

        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);
        mesh.convert();
        mesh.write(file_name + ".msh");

        //Simulation
        //Deal.II library
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.run();
        sim.output_results(file_name);
        
        //tree_A is represent current river geometry,
        //tree_B will be representing simulated river geometry with new parameters.
        Tree tree_A = tree;

        //preevaluation series params to each tip point.
        map<int, vector<double>> ids_seriesparams_map;
        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id)->TipPoint();
            auto tip_angle = tree.GetBranch(id)->TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);
            ids_seriesparams_map[id] = series_params;
            gd.EndBiffurcationRecord(id, series_params);
        }
        sim.clear();

        
        //Processing backward growth by iterating over each tip id and its series_params
        for(auto[id, series_params]: ids_seriesparams_map)
            if(mdl.q_growth(series_params))
                tree.GetBranch(id)->
                    Shrink(mdl.next_point(
                        series_params, 
                        0 /*we are not constraining here speed growth near 
                        biffuraction points*/).r);

        //collect branches which reached zero lenght(biffurcation point)
        vector<int> zero_size_branches_id;
        for(auto tip_id: tree.TipBranchesId())
            if(tree.HasParentBranch(tip_id) && tree.GetBranch(tip_id)->Lenght() == 0)
                zero_size_branches_id.push_back(tree.GetParentBranchId(tip_id));
        
        //collect difference between branches lenght and delete them
        for(int parent_id: zero_size_branches_id)
            //for each pair of subbranches we can delete them only once
            if(tree.HasSubBranches(parent_id))
            {
                auto[branch_left, branch_right] = tree.GetSubBranches(parent_id);
                auto biff_diff = abs(branch_left->Lenght() - branch_right->Lenght());
                gd.StartBiffurcationRecord(parent_id, biff_diff);
                tree.DeleteSubBranches(parent_id);
            }

        
        //One step forward growth
        Tree tree_B = tree;
        Model mdl_B = mdl;
        mdl_B.biffurcation_type = 3;//3 - means no biffuraction at all.
        cout << "-><- One Step Forward" << endl;
        ForwardRiverEvolution(mdl, tria, sim, tree_B, border, "backward_forward_"+file_name);
             
        //comparsion of tip points with the same ids.
        auto original_points = tree_A.TipIdsAndPoints(),
            simulated_points = tree_B.TipIdsAndPoints();
             
        for(auto& el: original_points)
            if(simulated_points.count(el.first))
            {
                auto orig_point = el.second,
                    sim_point = simulated_points.at(el.first);
                auto dalpha = orig_point.angle(sim_point),
                    ds = (orig_point - sim_point).norm();
                gd.RecordBranchSeriesParamsAndGeomDiff(
                    el.first, 
                    dalpha, ds,
                    ids_seriesparams_map.at(el.first));
            }

        return stop_flag;
    }




    bool EvaluateSeriesParams(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, GeometryDifference& gd, string file_name)
    {
        //initial boundaries of mesh
        auto mesh = BoundaryGenerator(mdl, tree, border);
        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);//triangulation
        mesh.convert();//convertaion from triangles to quadrangles
        mesh.write(file_name + ".msh");

        //Simulation
        //Deal.II library
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.run();
        sim.output_results(file_name);

        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id)->TipPoint();
            auto tip_angle = tree.GetBranch(id)->TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);

            gd.RecordBranchSeriesParamsAndGeomDiff(id, -1/*dalpha*/, -1/*ds*/, series_params);
        }
        sim.clear();

        return true;
    }
}//namespace River