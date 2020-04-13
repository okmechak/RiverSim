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
#include "solver.hpp"

namespace River
{
    void Solver::OpenMesh(const string fileName)
    {
        GridIn<dim> gridin;
        gridin.attach_triangulation(triangulation);
        ifstream f(fileName);
        if(!f)
            throw Exception("OpenMesh: there is no mesh file  - " + fileName);
        gridin.read_msh(f);
    }

    void Solver::setup_system()
    {
        dof_handler.distribute_dofs(fe);
        DoFRenumbering::Cuthill_McKee(dof_handler);

        hanging_node_constraints.clear();
        DoFTools::make_hanging_node_constraints(
            dof_handler, hanging_node_constraints);

        hanging_node_constraints.close();

        DynamicSparsityPattern dsp(dof_handler.n_dofs(), dof_handler.n_dofs());
        DoFTools::make_sparsity_pattern(
            dof_handler, 
            dsp);
        hanging_node_constraints.condense(dsp);

        sparsity_pattern.copy_from(dsp);
        system_matrix.reinit(sparsity_pattern);

        solution.reinit(dof_handler.n_dofs());
        system_rhs.reinit(dof_handler.n_dofs());
    }

    void Solver::assemble_system()
    {
        const unsigned n_q_points = quadrature_formula.size();
        const unsigned n_face_q_points = face_quadrature_formula.size();

        const unsigned dofs_per_cell = fe.dofs_per_cell;

        FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
        Vector<double> cell_rhs(dofs_per_cell);

        vector<types::global_dof_index> local_dof_indices(dofs_per_cell);

        FEValues<dim> fe_values(fe, 
            quadrature_formula,
            update_values | update_gradients | update_quadrature_points | update_JxW_values);

        FEFaceValues<dim> fe_face_values(fe,
            face_quadrature_formula,
            update_values | update_quadrature_points | update_JxW_values);

        for (const auto& cell: dof_handler.active_cell_iterators())
        {
            cell_matrix = 0.;
            cell_rhs = 0.;

            fe_values.reinit(cell);

            for (unsigned q_index = 0; q_index < n_q_points; ++q_index)
                for (unsigned i = 0; i < dofs_per_cell; ++i)
                {
                    for (unsigned int j = 0; j < dofs_per_cell; ++j)
                        cell_matrix(i, j) += fe_values.shape_grad(i, q_index) *
                                             fe_values.shape_grad(j, q_index) *
                                             fe_values.JxW(q_index);

                    cell_rhs(i) += field_value *
                                   fe_values.shape_value(i, q_index) *
                                   fe_values.JxW(q_index);
                }

            auto neuman_bd = model->boundary_conditions.Get(NEUMAN);
            for (unsigned int face_number = 0; face_number < GeometryInfo<dim>::faces_per_cell; ++face_number)
                if (cell->face(face_number)->at_boundary() &&
                    neuman_bd.count(cell->face(face_number)->boundary_id()))
                {
                    fe_face_values.reinit(cell, face_number);
                    auto boundary_id = cell->face(face_number)->boundary_id();
                    const auto neuman_value = neuman_bd.at(boundary_id).value;
                    for (unsigned int q_point = 0; q_point < n_face_q_points; ++q_point)
                        for (unsigned int i = 0; i < dofs_per_cell; ++i)
                            cell_rhs(i) += neuman_value *
                                           fe_face_values.shape_value(i, q_point) *
                                           fe_face_values.JxW(q_point);
                }

            cell->get_dof_indices(local_dof_indices);
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
                for (unsigned int j = 0; j < dofs_per_cell; ++j)
                    system_matrix.add(
                        local_dof_indices[i],
                        local_dof_indices[j],
                        cell_matrix(i, j));

                system_rhs(local_dof_indices[i]) += cell_rhs(i);
            }
        }

        hanging_node_constraints.condense(system_matrix);
        hanging_node_constraints.condense(system_rhs);

        std::map<types::global_dof_index, double> boundary_values;
        auto dirichlet_bd = model->boundary_conditions.Get(DIRICHLET);
        for(const auto &[boundary_id, boundary_condition]: dirichlet_bd)
                VectorTools::interpolate_boundary_values(
                    dof_handler, 
                    boundary_id, 
                    ConstantFunction<dim>(boundary_condition.value), 
                    boundary_values);

        MatrixTools::apply_boundary_values(
            boundary_values,
            system_matrix,
            solution,
            system_rhs);
    }

    void Solver::solve()
    {
        SolverControl solver_control(number_of_iterations, tollerance);
        SolverCG<> cg(solver_control);

        PreconditionSSOR<> preconditioner;
        preconditioner.initialize(system_matrix, 1.2);

        cg.solve(
            system_matrix, 
            solution, 
            system_rhs,
            preconditioner);

        hanging_node_constraints.distribute(solution);
    }

    void Solver::refine_grid()
    {
        Vector<float> estimated_error_per_cell(triangulation.n_active_cells());

        KellyErrorEstimator<dim>::estimate(dof_handler,
                                           QGauss<dim - 1>(3),
                                           map<types::boundary_id, const Function<dim> *>(),
                                           solution,
                                           estimated_error_per_cell);

        GridRefinement::refine_and_coarsen_fixed_number(triangulation,
                                                        estimated_error_per_cell,
                                                        refinment_fraction, coarsening_fraction);

        triangulation.execute_coarsening_and_refinement();
    }

    void Solver::static_refine_grid(const Model& mdl, const vector<Point>& tips_points)
    {
        //iterating over refinment steps
        for (unsigned step = 0; step < num_of_static_refinments; ++step)
        {
            //iterating over each mesh cell
            for (auto cell: triangulation.active_cell_iterators())
            {
                for (unsigned int v = 0; v < GeometryInfo<2>::vertices_per_cell; ++v)
                {
                    for(auto& p: tips_points)
                    {
                        auto vertex = cell->vertex(v);
                        auto r = dealii::Point<2>{p.x, p.y}.distance(vertex);
                        if(r < mdl.integr.integration_radius)
                        {
                            cell->set_refine_flag ();
                            break;
                        }
                    }
                    if(cell->refine_flag_set())
                        break;
                }
            }
            triangulation.execute_coarsening_and_refinement ();
        }
    }

    vector<double> Solver::integrate(const Model& mdl, const Point& point, const double angle)
    {   

        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values |
            update_JxW_values);

        //Parameters definition
        vector<double> 
            values(quadrature_formula.size()),
            integral(3, 0),
            normalization_integral(3, 0),
            series_params(3, 0); //Series params

        //Iteratation over Mesh and DoFHandler 
        for(auto dof_cell: dof_handler.active_cell_iterators())
        {
            //central point of mesh element
            auto center = dof_cell->center();

            //distance between tip point and current mesh element
            auto dx = center[0] - point[0],
                dy = center[1] - point[1],
                dist = sqrt(dx*dx + dy*dy);

            //Integrates over points only in this circle
            if(dist <= mdl.integr.integration_radius)
            {
                fe_values.reinit (dof_cell);
                fe_values.get_function_values(solution, values);
                auto weight_func_value = mdl.integr.WeightFunction(dist);

                //cycle over all series parameters order
                for(unsigned param_index = 0; param_index < series_params.size(); ++param_index)
                {
                    //preevaluate basevector value
                    auto base_vector_value = mdl.integr.BaseVectorFinal(param_index + 1, angle, dx, dy);

                    //sum over all quadrature points overIntegrationRadius single mesh element
                    for (unsigned q_point = 0; q_point < quadrature_formula.size(); ++q_point)
                    {
                        //integration of weighted integral..
                        integral[param_index]
                            += values[q_point]
                            * weight_func_value
                            * base_vector_value
                            * fe_values.JxW(q_point);
                        
                        //.. and its normalization integral
                        normalization_integral[param_index]
                            += weight_func_value
                            * pow(base_vector_value, 2)
                            * fe_values.JxW(q_point);
                    }       
                }
            }
        }

        for(unsigned i = 0; i < integral.size(); ++i)
            series_params[i] = integral[i]/normalization_integral[i];

        return series_params;
    }


    double Solver::integration_test(const Point& point, const double dr)
    {

        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values            |
            update_quadrature_points |
            update_JxW_values);

        const unsigned int n_q_points = quadrature_formula.size();

        vector<double> values(n_q_points);
        double integration_result = 0; 

        for(const auto& cell: dof_handler.active_cell_iterators())
        {   
            auto center = cell->center();
            auto dx = center[0] - point[0],
                dy = center[1] - point[1],
                dist = sqrt(dx*dx + dy*dy);

            if(dist <= dr)
            {
                fe_values.reinit (cell);
                fe_values.get_function_values(solution, values);
                auto& JxW_values = fe_values.get_JxW_values();

                for (unsigned q_point = 0; q_point < n_q_points; ++q_point)
                    integration_result  
                        += values[q_point]
                        * JxW_values[q_point];
            }
        }

        return integration_result;
    }

    double Solver::max_value()
    {
        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values            |
            update_quadrature_points |
            update_JxW_values);

        const unsigned int n_q_points = quadrature_formula.size();

        vector<double> values(n_q_points);
        double max_value = 0; 

        for (auto cell: dof_handler.active_cell_iterators())
        {
            fe_values.reinit (cell);
            fe_values.get_function_values(solution, values);

            for (unsigned q_point = 0; q_point < n_q_points; ++q_point)
                if(max_value < values[q_point]) 
                    max_value = values[q_point];
        }

        return max_value;
    }

    void Solver::output_results(const string file_name) const
    {
        DataOut<dim> data_out;
        data_out.attach_dof_handler(dof_handler);
        data_out.add_data_vector(solution, "solution");
        data_out.build_patches();
        ofstream output(file_name + ".vtk");
        output.precision(20);//Fix for paraview
        data_out.write_vtk(output);

        //ofstream out("grid-"+to_string(cycle)+".eps");
        //GridOut       grid_out;
        //grid_out.write_eps(triangulation, out);
    }

    void Solver::run()
    {
        for (unsigned cycle = 0; cycle <= num_of_adaptive_refinments; ++cycle)
        {
            
            if (cycle > 0)
            {
                print(verbose, "--------------------------------#" + to_string(cycle) + "--------------------------------");
                refine_grid();
            }
            print(verbose, "   Number of active cells:");
            print(verbose, "\t" + to_string(triangulation.n_active_cells()));
            setup_system();
            print(verbose, "   Number of degrees of freedom:");
            print(verbose, "\t" + to_string(dof_handler.n_dofs()));
            assemble_system();
            solve();
        }
    }

} // namespace River
