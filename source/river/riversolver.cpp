/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
 */

///\cond
#include <iostream>
///\endcond

#include "riversolver.hpp"

namespace River
{
    /*
        ForwardRiverSimulation
    */
    int RiverSolver::run()
    {
        try
        {
            if (verbose)
                cout << model << endl;

            // MAIN LOOP
            print(verbose, "Start of main loop...");

            // forward simulation case
            if (model.simulation_type == 0)
            {
                print(verbose, "Linear forward river simulation type selected.");
                linearSolver();
            }
            // forward non linear simulation case
            else if (model.simulation_type == 1)
            {
                print(verbose, "Non linear forward river simulation type selected.");
                nonLinearSolver();
            }
            // Backward simulation
            else if (model.simulation_type == 2)
            {
                print(verbose, "Bacward river simulation type selected.");
                backwardSolver();
            }
            // test simulation
            else if (model.simulation_type == 3)
            {
                print(verbose, "Test river simulation type selected. TODO");
            }
            // unhandled case
            else
                throw Exception("Invalid simulation type selected: " + to_string(model.simulation_type));

            print(verbose, "End of main loop...");
            print(verbose, "Done.");
        }
        catch (const River::Exception &caught)
        {
            cout << "river excetpion" << endl;
            cout << caught.what() << endl;
            return 1;
        }
        catch (const dealii::ExceptionBase &caught)
        {
            cout << "dealii excetpion" << endl;
            cout << caught.what() << endl;
            return 1;
        }
        catch (const cxxopts::OptionException &caught)
        {
            cout << "option excetpion" << endl;
            cout << caught.what() << endl;
            return 2;
        }
        catch (const std::exception &caught)
        {
            cout << "stl exception" << endl;
            cout << caught.what() << endl;
            return 3;
        }
        catch (...)
        {
            cout << "GOT UNDEFINED ERROR" << endl;
            return 4;
        }

        return 0;
    }

    map<t_branch_id, vector<double>> RiverSolver::solve_and_evaluate_series_parameters(string output_file_name)
    {
        print(verbose, "Boundary generation...");
        model.boundary = BoundaryGenerator(
            model.sources, model.region, model.rivers, model.river_width,
            model.mesh_params.smoothness_degree, model.mesh_params.ignored_smoothness_length);

        print(verbose, "Mesh generation...");
        triangle.mesh_params.tip_points = model.rivers.TipPoints();
        auto mesh = triangle.generate_quadrangular_mesh(model.boundary, model.region.holes);
        mesh.write(output_file_name + ".msh");

        print(verbose, "Solving...");
        solver.clear();
        solver.OpenMesh(mesh);
        solver.setBoundaryConditions(model.boundary_conditions);
        //solver.OpenMesh(output_file_name + ".msh");
        solver.static_refine_grid(model.integr.integration_radius, model.rivers.TipPoints());
        solver.run();
        if (prog_opt.save_vtk || prog_opt.debug)
            solver.output_results(output_file_name);

        print(verbose, "Series parameters integration...");
        map<t_branch_id, vector<double>> id_series_params;
        for (const auto id : model.rivers.TipBranchesIds())
        {
            auto tip_point = model.rivers.at(id).TipPoint();
            auto tip_angle = model.rivers.at(id).TipAngle();
            id_series_params[id] = solver.integrate_new(model.integr, tip_point, tip_angle);
        }

        return id_series_params;
    }

    double RiverSolver::get_max_a1(const map<t_branch_id, vector<double>> &id_series_params)
    {
        double max_a = 0.;
        for (const auto &[id, series_params] : id_series_params)
            if (max_a < series_params.at(0))
                max_a = series_params.at(0);

        return max_a;
    }

    map<t_branch_id, vector<double>> RiverSolver::linearStep(
        string output_file_name, double backwardforward_max_a1)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);

        print(verbose, "Rivers growth...");
        auto max_a1 = get_max_a1(id_series_params);
        // this functionality is in bacward forward growth, where we should use presaved a1 max parameters from backward evolution
        if (backwardforward_max_a1 > 0)
            max_a1 = backwardforward_max_a1;

        for (const auto &[id, series_params] : id_series_params)
            if (model.q_growth(series_params))
            {
                if (model.q_bifurcate(series_params, model.rivers.at(id).Lenght()))
                {
                    auto tip_point = model.rivers.at(id).TipPoint();
                    auto tip_angle = model.rivers.at(id).TipAngle();
                    auto br_left = Branch(tip_point, tip_angle + model.bifurcation_angle);
                    br_left.AddPoint(Polar{model.ds, 0}, model.river_boundary_id);
                    auto br_right = Branch(tip_point, tip_angle - model.bifurcation_angle);
                    br_right.AddPoint(Polar{model.ds, 0}, model.river_boundary_id);
                    model.rivers.AddSubBranches(id, br_left, br_right);
                }
                else
                    model.rivers.at(id).AddPoint(
                        model.next_point(series_params, model.rivers.at(id).Lenght(), max_a1),
                        model.river_boundary_id);
            }

        return id_series_params;
    }

    void RiverSolver::nonLinearStep(string output_file_name, double max_a1_step1, double max_a1_step2)
    {
        linearStep(output_file_name + "_first_half_step", max_a1_step1);

        if (prog_opt.debug)
            // Save(this, output_file_name + "_first_half_step");

            for (unsigned step = 0; true; ++step)
            {
                print(prog_opt.verbose, "non-euler solver cycle----------#" + to_string(step) + "--------------------------------");

                auto bif_type = model.bifurcation_type;
                model.bifurcation_type = 0; // no biffurcation

                linearStep(output_file_name + "_second_half_step_" + to_string(step), max_a1_step2);

                model.bifurcation_type = bif_type;

                if (prog_opt.debug)
                    Save(model, output_file_name + "_second_half_step_" + to_string(step));

                if (model.rivers.maximal_tip_curvature_distance() >= model.solver_params.max_distance)
                {
                    model.rivers.flatten_tip_curvature();
                    model.rivers.remove_tip_points();
                }
                else
                {
                    model.RevertLastSimulationStep();
                    break;
                }
            }
    }

    map<t_branch_id, vector<double>> RiverSolver::shrinkStep(string output_file_name)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);

        print(prog_opt.verbose, "Shrinking of rivers...");
        auto max_a1 = get_max_a1(id_series_params);
        for (const auto &[id, series_params] : id_series_params)
            if (model.q_growth(series_params))
                model.rivers.at(id).Shrink(
                    model.next_point(
                             series_params,
                             100 /*we are not constraining here speed growth near
                              biffuraction points, so we set some value greater than its limit*/
                             ,
                             max_a1)
                        .r);

        print(prog_opt.verbose, "Handle branches with zero lenght(if they are) and collect data...");
        for (auto id : model.rivers.zero_lenght_tip_branches_ids(0.01 * model.ds))
        {
            const auto &[branch_left, branch_right] = model.rivers.GetSubBranches(id);
            auto biff_diff = abs(branch_left.Lenght() - branch_right.Lenght());
            model.backward_data[id].branch_lenght_diff = biff_diff;
            model.rivers.DeleteSubBranches(id);
        }

        return id_series_params;
    }

    void RiverSolver::backwardStep()
    {
        // save initial rivers
        auto initial_rivers = model.rivers;
        map<t_branch_id, vector<double>> tip_id_series_params;
        vector<double> presaved_max_a1_parameters;

        for (unsigned step = 0; step < model.number_of_backward_steps; step++)
        {
            print(verbose, "backward cycle--------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = prog_opt.output_file_name;
            if (prog_opt.save_each_step || prog_opt.debug)
                output_file_name += "_backward_" + to_string(step);

            auto id_series_params = shrinkStep(output_file_name);

            presaved_max_a1_parameters.push_back(get_max_a1(id_series_params));

            if (tip_id_series_params.empty())
                tip_id_series_params = id_series_params;

            if (prog_opt.debug)
                Save(model, output_file_name);
        }

        auto prev_rivers = model.rivers;
        auto temp_bif = model.bifurcation_type;
        model.bifurcation_type = 0; // no biffuraction

        for (unsigned step = 0; step < model.number_of_backward_steps; step++)
        {
            print(verbose, "backward forward cycle------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = prog_opt.output_file_name;
            if (prog_opt.save_each_step || prog_opt.debug)
                output_file_name += "_backwardforward_" + to_string(step);

            double
                max_a1_step1 = presaved_max_a1_parameters.at(step),
                max_a1_step2 = -1;

            if (step < model.number_of_backward_steps - 1)
                max_a1_step2 = presaved_max_a1_parameters.at(step + 1);

            nonLinearStep(output_file_name, max_a1_step1, max_a1_step2);

            if (prog_opt.debug)
                Save(model, prog_opt.output_file_name);
        }

        auto backwardforward_rivers = model.rivers;
        model.rivers = prev_rivers;
        model.bifurcation_type = temp_bif;

        // collect data
        model.collect_backward_data(initial_rivers, backwardforward_rivers, tip_id_series_params);
    }

    void RiverSolver::linearSolver()
    {
        for (unsigned step = 0; step < model.number_of_steps; ++step)
        {
            if (growth_stop_condition())
                break;

            print(verbose, "----------------------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = prog_opt.output_file_name;
            if (prog_opt.save_each_step || prog_opt.debug)
                output_file_name += "_" + to_string(step);

            auto id_series_params = linearStep(output_file_name);

            model.series_parameters.record(id_series_params);
            model.sim_data["MeshSize"].push_back(solver.NumberOfRefinedCells());
            model.sim_data["DegreeOfFreedom"].push_back(solver.NumberOfDOFs());

            Save(model, output_file_name);
        }
    }

    void RiverSolver::nonLinearSolver()
    {
        for (unsigned step = 0; step < model.number_of_steps; ++step)
        {
            print(verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = prog_opt.output_file_name;
            if (prog_opt.save_each_step || prog_opt.debug)
                output_file_name += "_" + to_string(step);

            nonLinearStep(output_file_name);
            Save(model, output_file_name);
        }
    }

    void RiverSolver::backwardSolver()
    {
        for (unsigned step = 0; step < model.number_of_steps; ++step)
        {
            print(verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = prog_opt.output_file_name;
            if (prog_opt.save_each_step || prog_opt.debug)
                output_file_name += "_" + to_string(step);

            backwardStep();
            Save(model, output_file_name);
        }
    }
}