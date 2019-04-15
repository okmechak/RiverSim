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

        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id).TipPoint();
            auto tip_angle = tree.GetBranch(id).TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);

            if(mdl.q_growth(series_params))
            {
                if(mdl.q_biffurcate(series_params))
                {
                    auto br_left = BranchNew(tip_point, tip_angle + mdl.biffurcation_angle);
                    br_left.AddPoint(Polar{mdl.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - mdl.biffurcation_angle);
                    br_right.AddPoint(Polar{mdl.ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.GetBranch(id).AddPoint(mdl.next_point(series_params));
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
        Tree tree_A = tree, tree_B;
        
        //this vector holds ids of branches which adjacent branch reached biffurcation point
        vector<int> deleted_branches;
        for(auto id: tree.TipBranchesId())
        {
            //if remove one branch then adjacent branch is removed too, so we should ommit them
            if(find(begin(deleted_branches), end(deleted_branches), id)!= end(deleted_branches))
            {
                auto tip_point = tree.GetBranch(id).TipPoint();
                auto tip_angle = tree.GetBranch(id).TipAngle();
                auto series_params = sim.integrate(mdl, tip_point, tip_angle);
            
                gd.Add(series_params);

                if(mdl.q_growth(series_params))
                {
                    auto& branch = tree.GetBranch(id);
                    if(branch.Lenght() > 0)
                        branch.Shrink(mdl.next_point(series_params).r);
                
                    if(branch.Lenght() == 0 && tree.IsSourceBranch(id))
                        //stop simulation
                        stop_flag = true;
                    else if(branch.Lenght() == 0 && !tree.IsSourceBranch(id))
                    {
                        auto adjacent_branch_id = tree.GetAdjacentBranchId(id);
                        gd.Add(tree.GetBranch(adjacent_branch_id).Lenght());
                        tree.DeleteSubBranches(tree.GetSourceBranch(id));
                        deleted_branches.push_back(adjacent_branch_id);
                    }       
                }
            }
        }

        sim.clear();

        tree_B = tree;

        Model mdl_B = mdl;
        mdl_B.biffurcation_type = 3;//3 - means no biffuraction at all.
        ForwardRiverEvolution(mdl, tria, sim, tree_B, border, file_name);

        ///TODO compare tip points
        auto original_points = tree_A.TipIdsAndPoints(),
            simulated_points = tree_B.TipIdsAndPoints();

        for(auto& el: original_points)
            if(simulated_points.count(el.first))
            {
                auto orig_point = el.second,
                    sim_point = simulated_points.at(el.first);
                gd.Add(orig_point, sim_point);
            }
        return stop_flag;
    }
}//namespace River