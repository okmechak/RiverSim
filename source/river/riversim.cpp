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
#include "boundary_generator.hpp"

///\cond
#include <math.h>
///\endcond

namespace River
{
    /*
        ForwardRiverSimulation
    */
    SimpleBoundary ForwardRiverSimulation::generate_boudaries(string file_name)
    {
        auto boundary = SimpleBoundaryGenerator(*model);
        if(boundary.vertices.empty())
            throw Exception("TriangulateBoundaries: boundary is enpty");
        
        return boundary;
    }

    void ForwardRiverSimulation::generate_mesh_file(SimpleBoundary& boundary, string file_name)
    {
        auto mesh = tethex::Mesh(boundary);
        if(model->prog_opt.debug)
            mesh.write(file_name + "_boundary.msh");

        triangle->mesh_params->tip_points = model->tree.TipPoints();
        triangle->generate(mesh);//triangulation

        if(model->prog_opt.debug)
            mesh.write(file_name + "_triangles.msh");

        mesh.convert();//convertaion from triangles to quadrangles

        mesh.write(file_name + ".msh");
    }

    void ForwardRiverSimulation::solve(string mesh_file_name)
    {
        solver->clear();
        solver->OpenMesh(mesh_file_name + ".msh");
        solver->static_refine_grid(*model, model->tree.TipPoints());
        solver->run();
        if (model->prog_opt.save_vtk || model->prog_opt.debug)
            solver->output_results(mesh_file_name);
    }

    map<t_branch_id, vector<double>> ForwardRiverSimulation::integrate_series_parameters()
    {
        if(!solver->solved())
            throw Exception("EvaluateSeriesParameteresOfTips: run of solver wasn't called");

        map<t_branch_id, vector<double>> id_series_params;
        for(const auto id: model->tree.TipBranchesIds())
        {
            auto tip_point = model->tree.at(id).TipPoint();
            auto tip_angle = model->tree.at(id).TipAngle();
            id_series_params[id] = solver->integrate(*model, tip_point, tip_angle);
        }

        return id_series_params;
    }

    map<t_branch_id, vector<double>> ForwardRiverSimulation::solve_and_evaluate_series_parameters(string output_file_name)
    {
        print(model->prog_opt.verbose, "Boundary generation...");
        auto boundary = generate_boudaries(output_file_name);
        
        print(model->prog_opt.verbose, "Mesh generation...");
        generate_mesh_file(boundary, output_file_name);
        
        print(model->prog_opt.verbose, "Solving...");
        solve(output_file_name);

        print(model->prog_opt.verbose, "Series parameters integration...");
        auto id_series_params = integrate_series_parameters();

        return id_series_params;
    }

    double ForwardRiverSimulation::get_max_a1(const map<t_branch_id, vector<double>>& id_series_params)
    {
        double max_a = 0.;
        for(const auto&[id, series_params]: id_series_params)
            if(max_a < series_params.at(0))
                max_a = series_params.at(0);

        return max_a;
    }

    void ForwardRiverSimulation::grow_tree(const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        auto& tree = model->tree;
        if(tree.empty())
            throw Exception("GrowTree: tree is empty");

        for(const auto&[id, series_params]: id_series_params)
            if(model->q_growth(series_params))
            {
                if(model->q_bifurcate(series_params, tree.at(id).Lenght()))
                {
                    auto tip_point = tree.at(id).TipPoint();
                    auto tip_angle = tree.at(id).TipAngle();
                    auto br_left = BranchNew(tip_point, tip_angle + model->bifurcation_angle);
                    br_left.AddPoint(Polar{model->ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - model->bifurcation_angle);
                    br_right.AddPoint(Polar{model->ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.at(id).AddPoint(
                        model->next_point(series_params, tree.at(id).Lenght(), max_a));
            }
    }

    void ForwardRiverSimulation::shrink_tree(const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        if(model->tree.empty())
            throw Exception("ShrinkTree: tree is empty");

        for(const auto&[id, series_params]: id_series_params)
            if(model->q_growth(series_params))
                model->tree.at(id).
                    Shrink(
                        model->next_point(
                            series_params, 
                            100/*we are not constraining here speed growth near 
                            biffuraction points, so we set some value greater than its limit*/, 
                            max_a).r);
    }

    map<t_branch_id, vector<double>> ForwardRiverSimulation::one_linear_step(
        string output_file_name, double backwardforward_max_a1)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);
        
        print(model->prog_opt.verbose, "Tree growth...");
        auto max_a1 = get_max_a1(id_series_params);
        //this functionality is in bacward forward growth, where we should use presaved a1 max parameters from backward evolution
        if(backwardforward_max_a1 > 0)
            max_a1 = backwardforward_max_a1;

        grow_tree(id_series_params, max_a1);

        return id_series_params;
    }

    void ForwardRiverSimulation::one_non_linear_step(string output_file_name, double max_a1_step1, double max_a1_step2)
    {              
        one_linear_step(output_file_name + "_first_half_step", max_a1_step1);
        
        if( model->prog_opt.debug)
            Save(*model, output_file_name + "_first_half_step");

        for(unsigned step = 0; true; ++step)
        {                                   
            print(model->prog_opt.verbose, "non-euler solver cycle----------#" + to_string(step) + "--------------------------------");
            
            auto bif_type = model->bifurcation_type;
            model->bifurcation_type = 3;//no biffurcation

            one_linear_step(output_file_name + "_second_half_step_" + to_string(step), max_a1_step2);

            model->bifurcation_type = bif_type;

            if (model->prog_opt.debug)
                Save(*model, output_file_name + "_second_half_step_" + to_string(step));
                    
            if(model->tree.maximal_tip_curvature_distance() >= model->solver_params.max_distance)
            {
                model->tree.flatten_tip_curvature();
                model->tree.remove_tip_points();
            }
            else
            {
                model->RevertLastSimulationStep();
                break;
            }
        }
    }

    map<t_branch_id, vector<double>> ForwardRiverSimulation::one_shrink_step(string output_file_name)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);
        
        print(model->prog_opt.verbose, "Shrinking of trees...");
        auto max_a1 = get_max_a1(id_series_params);
        shrink_tree(id_series_params, max_a1);

        print(model->prog_opt.verbose, "Handle branches with zero lenght(if they are) and collect data...");
        for(auto id: model->tree.zero_lenght_tip_branches_ids(0.01 * model->ds))
        {
            const auto& [branch_left, branch_right] = model->tree.GetSubBranches(id);
            auto biff_diff = abs(branch_left.Lenght() - branch_right.Lenght());
            model->backward_data[id].branch_lenght_diff = biff_diff;
            model->tree.DeleteSubBranches(id);
        }

        return id_series_params;
    }

    void ForwardRiverSimulation::one_backward_step(string output_file_name)
    {
        //save initial tree
        auto initial_tree = model->tree;
        map<t_branch_id, vector<double>> tip_id_series_params;
        vector<double> presaved_max_a1_parameters;

        for(unsigned step = 0; step < model->prog_opt.number_of_backward_steps; step++)
        {
            print(model->prog_opt.verbose, "backward cycle--------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_backward_" + to_string(step);

            auto id_series_params = one_shrink_step(output_file_name);

            presaved_max_a1_parameters.push_back(get_max_a1(id_series_params));
            
            if (tip_id_series_params.empty())
                tip_id_series_params = id_series_params;

            if (model->prog_opt.debug)
                Save(*model, output_file_name);
        }

        model->SaveCurrentTree();
        auto temp_bif = model->bifurcation_type;
        model->bifurcation_type = 3;//no biffuraction

        for(unsigned step = 0; step < model->prog_opt.number_of_backward_steps; step++)
        {
            print(model->prog_opt.verbose, "backward forward cycle------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_backwardforward_" + to_string(step);
            
            double 
                max_a1_step1 = presaved_max_a1_parameters.at(step), 
                max_a1_step2 = -1;

            if (step < model->prog_opt.number_of_backward_steps - 1)
                max_a1_step2 = presaved_max_a1_parameters.at(step + 1);

            one_non_linear_step(output_file_name, max_a1_step1, max_a1_step2);

            if (model->prog_opt.debug)
                Save(*model, output_file_name);
        }

        auto backwardforward_tree = model->tree;
        model->RestoreTree();
        model->bifurcation_type = temp_bif;

        //collect data
        collect_backward_data(initial_tree, backwardforward_tree, tip_id_series_params);
    }

    void ForwardRiverSimulation::collect_backward_data(Tree& init, Tree& forwrdbackward, map<t_branch_id, vector<double>>& tip_id_series_params)
    {
        for(const auto&[id, series]: tip_id_series_params)
        {
            model->backward_data[id].a1.push_back(series.at(0));
            model->backward_data[id].a2.push_back(series.at(1));
            model->backward_data[id].a3.push_back(series.at(2));
        }
        
        auto tip_points_init = init.TipIdsAndPoints(),
            tip_points_final = forwrdbackward.TipIdsAndPoints(),
            tip_poins_cur = model->tree.TipIdsAndPoints();

        for(const auto&[id, point]: tip_points_init)
            if (tip_points_final.count(id) && tip_poins_cur.count(id))
            {
                model->backward_data[id].init.push_back(point);
                model->backward_data[id].backward.push_back(tip_poins_cur.at(id));
                model->backward_data[id].backward_forward.push_back(tip_points_final.at(id));
            }
    }

    void ForwardRiverSimulation::linear_solver()
    {   
        for(unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {
            if (growth_stop_condition())
                break;
            
            print(model->prog_opt.verbose, "----------------------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            auto id_series_params = one_linear_step(output_file_name);
            
            model->series_parameters.record(id_series_params);
            model->sim_data["MeshSize"].push_back(solver->NumberOfRefinedCells());
            model->sim_data["DegreeOfFreedom"].push_back(solver->NumberOfDOFs());

            Save(*model, output_file_name);
        }
    }
    
    void ForwardRiverSimulation::non_linear_solver()
    {
        for (unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {                                   
            print(model->prog_opt.verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if (model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            one_non_linear_step(output_file_name);
            Save(*model, output_file_name);
        }
    }

    void ForwardRiverSimulation::backward_solver()
    {
        for (unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {                                   
            print(model->prog_opt.verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if (model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            one_backward_step(output_file_name);
            Save(*model, output_file_name);
        }

    }
    




















    tethex::Mesh TriangulateBoundaries(Model& model, Triangle& tria, const string file_name)
    {
        print(model.prog_opt.verbose, "Boundary generation...");
        //initial boundaries of mesh
        auto boundary = SimpleBoundaryGenerator(model);
        if(boundary.vertices.size() == 0)
            throw Exception("TriangulateBoundaries: boundary is enpty");

        tethex::Mesh mesh(boundary);
        mesh.write(file_name + "_boundary.msh");
        
        print(model.prog_opt.verbose, "Mesh generation...");
        tria.mesh_params->tip_points = model.tree.TipPoints();
        tria.generate(mesh);//triangulation

        print(model.prog_opt.verbose, "Triangles to quadrangles trasformation...");
        mesh.convert();//convertaion from triangles to quadrangles

        print(model.prog_opt.verbose, "Save intermediate output(*.msh)...");
        mesh.write(file_name + ".msh");

        return mesh;
    }

    void SolvePDE(Model& model, Solver& sim, const string file_name)
    {
        //Simulation
        //Deal.II library
        print(model.prog_opt.verbose, "Solving...");
        sim.clear();
        sim.OpenMesh(file_name + ".msh");
        sim.static_refine_grid(model, model.tree.TipPoints());
        sim.run();
        if (model.prog_opt.save_vtk)
            sim.output_results(file_name);
    }

    map<t_branch_id, vector<double>> EvaluateSeriesParameteresOfTips(Model &model, Solver& sim)
    {
        if(!sim.solved())
            throw Exception("EvaluateSeriesParameteresOfTips: run of solver wasn't called");

        map<t_branch_id, vector<double>> id_series_params;
        for(auto id: model.tree.TipBranchesIds())
        {
            auto tip_point = model.tree.at(id).TipPoint();
            auto tip_angle = model.tree.at(id).TipAngle();
            id_series_params[id] = sim.integrate(model, tip_point, tip_angle);
        }

        return id_series_params;
    }

    double MaximalA1Value(const map<t_branch_id, vector<double>>& ids_seriesparams_map)
    {
        double max_a = 0.;
        for(const auto&[id, series_params]: ids_seriesparams_map)
            if(max_a < series_params.at(0))
                max_a = series_params.at(0);

        return max_a;
    }

    void GrowTree(Model &model, const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        if(model.tree.empty())
            throw Exception("GrowTree: tree is empty");

        for(const auto&[id, series_params]: id_series_params)
            if(model.q_growth(series_params))
            {
                if(model.q_bifurcate(series_params, model.tree.at(id).Lenght()))
                {
                    auto tip_point = model.tree.at(id).TipPoint();
                    auto tip_angle = model.tree.at(id).TipAngle();
                    auto br_left = BranchNew(tip_point, tip_angle + model.bifurcation_angle);
                    br_left.AddPoint(Polar{model.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - model.bifurcation_angle);
                    br_right.AddPoint(Polar{model.ds, 0});
                    model.tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    model.tree.at(id).AddPoint(
                        model.next_point(
                            series_params, 
                            model.tree.at(id).Lenght(), 
                            max_a));
            }
    }

    void ShrinkTree(Model& model, const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        if(model.tree.empty())
            throw Exception("ShrinkTree: tree is empty");

        print(model.prog_opt.verbose, "Shrinking each branch...");
        for(const auto&[id, series_params]: id_series_params)
            if(model.q_growth(series_params))
                model.tree.at(id).
                    Shrink(
                        model.next_point(
                            series_params, 
                            model.growth_min_distance + 9/*we are not constraining here speed growth near 
                            biffuraction points, so we set some value greater than its limit*/, max_a).r);
    }

    void ForwardRiverEvolution(Model& model, Triangle& tria, Solver& sim,
        const string file_name, double max_a_backward)
    {
        TriangulateBoundaries(model, tria, file_name);
        SolvePDE(model, sim, file_name);
        auto id_series_params_map = EvaluateSeriesParameteresOfTips(model, sim);
        model.sim_data["MeshSize"].push_back(sim.NumberOfRefinedCells());
        model.sim_data["DegreeOfFreedom"].push_back(sim.NumberOfDOFs());
        sim.clear();

        model.series_parameters.record(id_series_params_map);

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
    map<t_branch_id, vector<double>> BackwardRiverEvolution(Model& model, Solver& sim)
    {
        //preevaluation series params to each tip point.
        print(model.prog_opt.verbose, "Series parameters integration over each tip point...");
        auto id_series_params_map = EvaluateSeriesParameteresOfTips(model, sim);
        //for(auto&[id, series_params]: id_series_params_map)
        //    model.geometry_difference.EndBifurcationRecord(id, series_params);

        //lookup for maximal a(or first) series parameter through all tips
        double max_a = MaximalA1Value(id_series_params_map);

        //Processing backward growth by iterating over each tip id and its series_params
        ShrinkTree(model, id_series_params_map, max_a);

        //collect branches which reached zero lenght(bifurcation point)
        print(model.prog_opt.verbose, "Collecting branches with zero lenght(if they are)...");
        vector<int> zero_size_branches_id;
        for(auto tip_id: model.tree.TipBranchesIds())
            if(model.tree.HasParentBranch(tip_id) && model.tree.at(tip_id).Lenght() < 0.05*model.ds/*if lenght is less then 5% of ds*/)
                zero_size_branches_id.push_back(model.tree.GetParentBranchId(tip_id));
        
        //collect difference between adjacent branches lenght(if they are) and delete them
        print(model.prog_opt.verbose, "Collecting lenght difference between branches...");
        for(int parent_id: zero_size_branches_id)
            //for each pair of subbranches we can delete them only once
            if(model.tree.HasSubBranches(parent_id))
            {
                auto [branch_left, branch_right] = model.tree.GetSubBranches(parent_id);
                auto biff_diff = abs(branch_left.Lenght() - branch_right.Lenght());
                //model.geometry_difference.StartBifurcationRecord(parent_id, biff_diff);
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
        map<t_branch_id, vector<double>> ids_seriesparams_map;
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
                //model.geometry_difference.RecordBranchSeriesParamsAndGeomDiff(
                //    tip_original_id, 
                //    dalpha, ds,
                //    ids_seriesparams_map.at(tip_original_id));
            }
    }

    void EvaluateSeriesParams(Model& model, Triangle& tria, Solver& sim, 
         const string file_name)
    {
        TriangulateBoundaries(model, tria, file_name);
        SolvePDE(model, sim, file_name);

        auto branch_id = model.tree.TipBranchesIds().back();
        if(branch_id != 1)
            throw Exception("EvaluateSeriesParams: Branch does not equal to 1: " + to_string(branch_id));
                
        auto tip_point = model.tree.at(branch_id).TipPoint();
        auto tip_angle = model.tree.at(branch_id).TipAngle();
        print(model.prog_opt.verbose, "Integration..");
        auto series_params = sim.integrate(model, tip_point, tip_angle);
        auto circle_integr = sim.integration_test(tip_point, 0.1/*dr same as in FreeFEM++*/);
        auto whole_integr = sim.integration_test(tip_point, 10000/*large enough to cover whole region*/);
        sim.clear();

        //model.geometry_difference.RecordBranchSeriesParamsAndGeomDiff(branch_id, 
        //    whole_integr/*whole region*/, circle_integr/*circle_integr*/, series_params);
    }
}//namespace River