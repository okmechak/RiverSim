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
    bool StopConditionOfRiverGrowth(const Model& model)
    {
        const auto tips = model.tree.TipPoints();

        auto border_init_point = model.border.GetVertices().back(),
            tip_init_point = tips.back();

        double xmin = tip_init_point.x, 
            xmax = xmin, 
            ymax = tip_init_point.y, 
            rxmin = border_init_point.x, 
            rxmax = rxmin, 
            rymax = border_init_point.y;

        for(auto &p :model.border.GetVertices())
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

        for(auto &br: model.tree.branches)
            if(br.TipPoint().y > model.prog_opt.maximal_river_height)
                return true;

        return xmax > rxmax - dl || ymax > rymax - dl || xmin < rxmin + dl;
    }

    tethex::Mesh TriangulateBoundaries(Model& model, Triangle& tria, const string file_name)
    {
        print(model.prog_opt.verbose, "Boundary generation...");
        //initial boundaries of mesh
        auto mesh = BoundaryGenerator(model);
        mesh.write(file_name + "_boundary.msh");
        
        print(model.prog_opt.verbose, "Mesh generation...");
        tria.mesh_params->tip_points = model.tree.TipPoints();
        tria.generate(mesh);//triangulation

        print(model.prog_opt.verbose, "Triangles to quadrangles trasformation...");
        mesh.convert();//convertaion from triangles to quadrangles
        model.mesh.number_of_quadrangles = mesh.get_n_quadrangles(); // just saving number of quadrangles

        print(model.prog_opt.verbose, "Save intermediate output(*.msh)...");
        mesh.write(file_name + ".msh");

        return mesh;
    }

    void SolvePDE(Model& model, Solver& sim, const string file_name)
    {
        //Simulation
        //Deal.II library
        print(model.prog_opt.verbose, "Solving...");
        sim.SetBoundaryRegionValue(model.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(model.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.static_refine_grid(model, model.tree.TipPoints());
        sim.run();
        model.mesh.number_of_refined_quadrangles = sim.NumberOfRefinedCells();
        if (model.prog_opt.save_vtk)
            sim.output_results(file_name);
    }

    map<int, vector<double>> EvaluateSeriesParameteresOfTips(Model &model, Solver& sim)
    {
        map<int, vector<double>> id_series_params;
        for(auto id: model.tree.TipBranchesId())
        {
            auto tip_point = model.tree.GetBranch(id)->TipPoint();
            auto tip_angle = model.tree.GetBranch(id)->TipAngle();
            id_series_params[id] = sim.integrate(model, tip_point, tip_angle);
        }

        return id_series_params;
    }

    double MaximalA1Value(map<int, vector<double>> ids_seriesparams_map)
    {
        double max_a = 0.;
        for(auto&[id, series_params]: ids_seriesparams_map)
            if(max_a < series_params.at(0))
                max_a = series_params.at(0);

        return max_a;
    }

    void GrowTree(Model &model, map<int, vector<double>> id_series_params, double max_a)
    {
        for(auto&[id, series_params]: id_series_params)
            if(model.q_growth(series_params))
            {
                if(model.q_bifurcate(series_params, model.tree.GetBranch(id)->Lenght()))
                {
                    auto tip_point = model.tree.GetBranch(id)->TipPoint();
                    auto tip_angle = model.tree.GetBranch(id)->TipAngle();
                    auto br_left = BranchNew(tip_point, tip_angle + model.bifurcation_angle);
                    br_left.AddPoint(Polar{model.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - model.bifurcation_angle);
                    br_right.AddPoint(Polar{model.ds, 0});
                    model.tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    model.tree.GetBranch(id)->AddPoint(
                        model.next_point(
                            series_params, 
                            model.tree.GetBranch(id)->Lenght(), 
                            max_a));
            }
    }

    void ShrinkTree(Model& model, map<int, vector<double>> id_series_params, double max_a)
    {
        print(model.prog_opt.verbose, "Shrinking each branch...");
        for(auto[id, series_params]: id_series_params)
            if(model.q_growth(series_params))
                model.tree.GetBranch(id)->
                    Shrink(model.next_point(
                        series_params, 
                        model.growth_min_distance + 1/*we are not constraining here speed growth near 
                        biffuraction points, so we set some value greater than it limit*/, max_a).r);
    }


    void ForwardRiverEvolution(Model& model, Triangle& tria, Solver& sim,
        const string file_name, double max_a_backward)
    {
        TriangulateBoundaries(model, tria, file_name);
        SolvePDE(model, sim, file_name);
        auto id_series_params_map = EvaluateSeriesParameteresOfTips(model, sim);
        sim.clear();

        //Evaluate maximal a parameter to normalize growth of speed to all branches ds = dt*v / max(v_array).
        double max_a = 0.;
        if(max_a_backward > 0)
            max_a = max_a_backward;
        else 
            max_a = MaximalA1Value(id_series_params_map);

        GrowTree(model, id_series_params_map, max_a);
    }

    //This function only makes evaluation of bacward river growth based on pde solution and geometry
    //it returns data like difference betweem branches if they reached zero
    map<int, vector<double>> BackwardRiverEvolution(Model& model, Solver& sim)
    {
        //preevaluation series params to each tip point.
        print(model.prog_opt.verbose, "Series parameters integration over each tip point...");
        auto id_series_params_map = EvaluateSeriesParameteresOfTips(model, sim);
        for(auto&[id, series_params]: id_series_params_map)
            model.geometry_difference.EndBifurcationRecord(id, series_params);

        //lookup for maximal a(or first) series parameter through all tips
        double max_a = MaximalA1Value(id_series_params_map);

        //Processing backward growth by iterating over each tip id and its series_params
        ShrinkTree(model, id_series_params_map, max_a);

        //collect branches which reached zero lenght(bifurcation point)
        print(model.prog_opt.verbose, "Collecting branches with zero lenght(if they are)...");
        vector<int> zero_size_branches_id;
        for(auto tip_id: model.tree.TipBranchesId())
            if(model.tree.HasParentBranch(tip_id) && model.tree.GetBranch(tip_id)->Lenght() < 0.05*model.ds/*if lenght is less then 5% of ds*/)
                zero_size_branches_id.push_back(model.tree.GetParentBranchId(tip_id));
        
        //collect difference between adjacent branches lenght(if they are) and delete them
        print(model.prog_opt.verbose, "Collecting lenght difference between branches...");
        for(int parent_id: zero_size_branches_id)
            //for each pair of subbranches we can delete them only once
            if(model.tree.HasSubBranches(parent_id))
            {
                auto[branch_left, branch_right] = model.tree.GetSubBranches(parent_id);
                auto biff_diff = abs(branch_left->Lenght() - branch_right->Lenght());
                model.geometry_difference.StartBifurcationRecord(parent_id, biff_diff);
                model.tree.DeleteSubBranches(parent_id);
            }

        return id_series_params_map;
    }

    //This function combines backward evolution, backwardforward evolution, and collects data
    void BackwardForwardRiverEvolution(Model& model, Triangle& tria, Solver& sim, const string file_name)
    {   
        //tree_A represent current river geometry,
        //tree_B will be representing simulated river geometry with new parameters.
        print(model.prog_opt.verbose, "Backward steps:");
        Tree tree_initial = model.tree;
        map<int, vector<double>> ids_seriesparams_map;
        vector<double> maximal_a1_params; //this vectors stores maximal a1 params which are used further in forward simulation
        for(unsigned i = 0; i < model.prog_opt.number_of_backward_steps; ++i)
        {
            print(model.prog_opt.verbose, "\t" + to_string(i));
            TriangulateBoundaries(model, tria, file_name);
            SolvePDE(model, sim, file_name + "_backward_" + to_string(i));
            if( ids_seriesparams_map.empty())
            {
                ids_seriesparams_map = BackwardRiverEvolution(model, sim);
                maximal_a1_params.push_back(MaximalA1Value(ids_seriesparams_map));
            }
            else
                maximal_a1_params.push_back(MaximalA1Value(BackwardRiverEvolution(model, sim)));
            sim.clear();
        }
        
        //Several steps of forward growth
        Model model_backforward = model;
        model_backforward.bifurcation_type = 3;//3 - means no biffuraction at all.
        print(model.prog_opt.verbose, "Forward steps:");
        for(unsigned i = 0; i < model.prog_opt.number_of_backward_steps; ++i)
        {
            print(model.prog_opt.verbose, "\t" + to_string(i));
            ForwardRiverEvolution(
                model_backforward, 
                tria, sim, 
                file_name + "_backward_forward_" + to_string(i), 
                maximal_a1_params.at(i));
        }
             
        //comparison of tip points with the same ids.
        print(model.prog_opt.verbose, "process geometry difference and collect data...");
        auto original_points = tree_initial.TipIdsAndPoints(),
            simulated_points = model_backforward.tree.TipIdsAndPoints();
        
        for(auto[tip_original_id, tip_original_point]: original_points)
            if(simulated_points.count(tip_original_id))
            {
                auto sim_point = simulated_points.at(tip_original_id);
                auto dalpha = tip_original_point.angle(sim_point),
                    ds = (tip_original_point - sim_point).norm();
                model.geometry_difference.RecordBranchSeriesParamsAndGeomDiff(
                    tip_original_id, 
                    dalpha, ds,
                    ids_seriesparams_map.at(tip_original_id));
            }
    }

    void EvaluateSeriesParams(Model& model, Triangle& tria, Solver& sim, 
         const string file_name)
    {
        TriangulateBoundaries(model, tria, file_name);
        SolvePDE(model, sim, file_name);

        auto branch_id = model.tree.TipBranchesId().back();
        if(branch_id != 1)
            throw Exception("EvaluateSeriesParams: Branch does not equal to 1: " + to_string(branch_id));
                
        auto tip_point = model.tree.GetBranch(branch_id)->TipPoint();
        auto tip_angle = model.tree.GetBranch(branch_id)->TipAngle();
        print(model.prog_opt.verbose, "Integration..");
        auto series_params = sim.integrate(model, tip_point, tip_angle);
        auto circle_integr = sim.integration_test(tip_point, 0.1/*dr same as in FreeFEM++*/);
        auto whole_integr = sim.integration_test(tip_point, 10000/*large enough to cover whole region*/);
        sim.clear();

        model.geometry_difference.RecordBranchSeriesParamsAndGeomDiff(branch_id, 
            whole_integr/*whole region*/, circle_integr/*circle_integr*/, series_params);
    }
}//namespace River