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

///\cond
#include <math.h>
///\endcond

namespace River
{
    bool StopConditionOfRiverGrowth(const Model& mdl, const Border& border, const Tree& tree)
    {
        const auto tips = tree.TipPoints();

        auto border_init_point = border.GetVertices().back(),
            tip_init_point = tips.back();

        double xmin = tip_init_point.x, 
            xmax = xmin, 
            ymax = tip_init_point.y, 
            rxmin = border_init_point.x, 
            rxmax = rxmin, 
            rymax = border_init_point.y;

        for(auto &p :border.GetVertices())
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
        double dl = 0.007;

        for(auto &br: tree.branches)
            if(br.TipPoint().y > mdl.prog_opt.maximal_river_height)
                return true;

        return xmax > rxmax - dl || ymax > rymax - dl || xmin < rxmin + dl;
    }

    void TriangulateConvertRefineAndSolve(Model& mdl, Triangle& tria, Solver& sim,
        Tree& tree, const Border& border, const string file_name)
    {
        print(mdl.prog_opt.verbose, "Boundary generation...");
        //initial boundaries of mesh
        auto mesh = BoundaryGenerator(mdl, tree, border);
        mesh.write(file_name + "_boundary.msh");
        
        print(mdl.prog_opt.verbose, "Mesh generation...");
        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);//triangulation
        print(mdl.prog_opt.verbose, "Triangles to quadrangles trasformation...");
        mesh.convert();//convertaion from triangles to quadrangles
        print(mdl.prog_opt.verbose, "Save intermediate output(*.msh)...");
        mesh.write(file_name + ".msh");

        //Simulation
        //Deal.II library
        print(mdl.prog_opt.verbose, "Solving...");
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.static_refine_grid(mdl, tree.TipPoints());
        sim.run();
        sim.output_results(file_name);
    }


    void ForwardRiverEvolution(Model& mdl, Triangle& tria, Solver& sim,
        Tree& tree, const Border& border, const string file_name)
    {
        TriangulateConvertRefineAndSolve(mdl, tria, sim, tree, border, file_name);

        //Iterate over each tip and handle branch growth and its biffurcations
        print(mdl.prog_opt.verbose, "Series parameters integration over each tip point...");
        map<int, vector<double>> id_series_params;
        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id)->TipPoint();
            auto tip_angle = tree.GetBranch(id)->TipAngle();
            id_series_params[id] = sim.integrate(mdl, tip_point, tip_angle);
        }

        //Evaluate maximal a parameter to normalize growth of speed to all branches dr = ds*v / max(v_array).
        double max_a = 0.;
        for(auto&[id, series_params]: id_series_params)
            if (max_a < series_params.at(0))
                max_a = series_params.at(0);

        print(mdl.prog_opt.verbose, "Growth(or biffurcation) of tree...");
        for(auto&[id, series_params]: id_series_params)
            if(mdl.q_growth(series_params))
            {
                if(mdl.q_biffurcate(series_params, tree.GetBranch(id)->Lenght()))
                {
                    auto tip_point = tree.GetBranch(id)->TipPoint();
                    auto tip_angle = tree.GetBranch(id)->TipAngle();
                    auto br_left = BranchNew(tip_point, tip_angle + mdl.biffurcation_angle);
                    br_left.AddPoint(Polar{mdl.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - mdl.biffurcation_angle);
                    br_right.AddPoint(Polar{mdl.ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.GetBranch(id)->AddPoint(mdl.next_point(series_params, tree.GetBranch(id)->Lenght(), max_a));
            }
        sim.clear();
    }

    //This function only makes evaluation of bacward river growth based on pde solution and geometry
    //it returns data like difference betweem branches if they reached zero
    map<int, vector<double>> BackwardRiverEvolution(const Model& mdl, Solver& sim, Tree& tree, 
        GeometryDifference& gd)
    {
        //preevaluation series params to each tip point.
        print(mdl.prog_opt.verbose, "Series parameters integration over each tip point...");
        map<int, vector<double>> ids_seriesparams_map;
        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id)->TipPoint();
            auto tip_angle = tree.GetBranch(id)->TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);
            ids_seriesparams_map[id] = series_params;
            gd.EndBiffurcationRecord(id, series_params);
        }

        //lookup for maximal a(or first) series parameter through all tips
        double max_a = 0.;
        for(auto&[id, series_params]: ids_seriesparams_map)
            if(max_a < series_params.at(0))
                max_a = series_params.at(0);

        
        //Processing backward growth by iterating over each tip id and its series_params
        print(mdl.prog_opt.verbose, "Shrinking each branch...");
        for(auto[id, series_params]: ids_seriesparams_map)
            if(mdl.q_growth(series_params))
                tree.GetBranch(id)->
                    Shrink(mdl.next_point(
                        series_params, 
                        mdl.growth_min_distance + 1/*we are not constraining here speed growth near 
                        biffuraction points, so we set some value greater than it limit*/, max_a).r);

        //collect branches which reached zero lenght(biffurcation point)
        print(mdl.prog_opt.verbose, "Collecting branches with zero lenght(if they are)...");
        vector<int> zero_size_branches_id;
        for(auto tip_id: tree.TipBranchesId())
            if(tree.HasParentBranch(tip_id) && tree.GetBranch(tip_id)->Lenght() < 0.05*mdl.ds/*if lenght is less then 5% of ds*/)
                zero_size_branches_id.push_back(tree.GetParentBranchId(tip_id));
        
        //collect difference between adjacent branches lenght(if they are) and delete them
        print(mdl.prog_opt.verbose, "Collecting lenght difference between branches...");
        for(int parent_id: zero_size_branches_id)
            //for each pair of subbranches we can delete them only once
            if(tree.HasSubBranches(parent_id))
            {
                auto[branch_left, branch_right] = tree.GetSubBranches(parent_id);
                auto biff_diff = abs(branch_left->Lenght() - branch_right->Lenght());
                gd.StartBiffurcationRecord(parent_id, biff_diff);
                tree.DeleteSubBranches(parent_id);
            }

        return ids_seriesparams_map;
    }

    //This function combines backward evolution, backwardforward evolution, and collects data
    void BackwardForwardRiverEvolution(Model& mdl, Triangle& tria, Solver& sim, Tree& tree, 
        const Border& border, GeometryDifference& gd, const string file_name)
    {   
        //tree_A represent current river geometry,
        //tree_B will be representing simulated river geometry with new parameters.
        print(mdl.prog_opt.verbose, "Backward steps:");
        Tree tree_initial = tree;
        map<int, vector<double>> ids_seriesparams_map;
        for(unsigned i = 0; i < mdl.prog_opt.number_of_backward_steps; ++i)
        {
            print(mdl.prog_opt.verbose, "\t" + to_string(i));
            TriangulateConvertRefineAndSolve(mdl, tria, sim, tree, border, file_name + + "_backward_" + to_string(i));
            if( ids_seriesparams_map.empty() )
                ids_seriesparams_map = BackwardRiverEvolution(mdl, sim, tree, gd);
            sim.clear();
        }
        
        //One step forward growth
        Tree tree_backforward = tree;
        Model mdl_backforward = mdl;
        mdl_backforward.biffurcation_type = 3;//3 - means no biffuraction at all.
        print(mdl.prog_opt.verbose, "Forward steps:");
        for(unsigned i = 0; i < mdl.prog_opt.number_of_backward_steps; ++i)
        {
            print(mdl.prog_opt.verbose, "\t" + to_string(i));
            ForwardRiverEvolution(mdl, tria, sim, tree_backforward, border, file_name + "_backward_forward_" + to_string(i));
        }
             
        //comparison of tip points with the same ids.
        print(mdl.prog_opt.verbose, "process geometry differnce and collect data...");
        auto original_points = tree_initial.TipIdsAndPoints(),
            simulated_points = tree_backforward.TipIdsAndPoints();
        
        for(auto[tip_original_id, tip_original_point]: original_points)
            if(simulated_points.count(tip_original_id))
            {
                auto sim_point = simulated_points.at(tip_original_id);
                auto dalpha = tip_original_point.angle(sim_point),
                    ds = (tip_original_point - sim_point).norm();
                gd.RecordBranchSeriesParamsAndGeomDiff(
                    tip_original_id, 
                    dalpha, ds,
                    ids_seriesparams_map.at(tip_original_id));
            }
    }




    void EvaluateSeriesParams(Model& mdl, Triangle& tria, Solver& sim, Tree& tree, 
        const Border& border, GeometryDifference& gd, const string file_name)
    {
        TriangulateConvertRefineAndSolve(mdl, tria, sim, tree, border, file_name);

        auto branch_id = tree.TipBranchesId().back();
        if(branch_id != 1)
            throw invalid_argument("EvaluateSeriesParams: Branch does not equal to 1: " + to_string(branch_id));
                
        auto tip_point = tree.GetBranch(branch_id)->TipPoint();
        auto tip_angle = tree.GetBranch(branch_id)->TipAngle();
        print(mdl.prog_opt.verbose, "Integration..");
        auto series_params = sim.integrate(mdl, tip_point, tip_angle);
        auto circle_integr = sim.integration_test(tip_point, 0.1/*dr same as in FreeFEM++*/);
        auto whole_integr = sim.integration_test(tip_point, 10000/*large enough to cover whole region*/);
        sim.clear();

        gd.RecordBranchSeriesParamsAndGeomDiff(branch_id, 
            whole_integr/*whole region*/, circle_integr/*circle_integr*/, series_params);
    }
}//namespace River