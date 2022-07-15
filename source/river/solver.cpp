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
#include <deal.II/grid/grid_tools.h>
#include <string>
#include <numeric>

namespace River
{
    // IntegrationParams
    ostream &operator<<(ostream &write, const IntegrationParams &ip)
    {
        write << "\t weigth_func_radius = " << ip.weigth_func_radius << endl;
        write << "\t integration_radius = " << ip.integration_radius << endl;
        write << "\t exponant = " << ip.exponant << endl;
        return write;
    }

    bool IntegrationParams::operator==(const IntegrationParams &ip) const
    {
        return abs(weigth_func_radius - ip.weigth_func_radius) < EPS && abs(integration_radius - ip.integration_radius) < EPS && abs(exponant - ip.exponant) < EPS;
    }

    // SolverParams
    ostream &operator<<(ostream &write, const SolverParams &sp)
    {
        write << "\t field_value = " << sp.field_value << endl;
        write << "\t quadrature_degree = " << sp.quadrature_degree << endl;
        write << "\t refinment_fraction = " << sp.refinment_fraction << endl;
        write << "\t adaptive_refinment_steps = " << sp.adaptive_refinment_steps << endl;
        write << "\t tollerance = " << sp.tollerance << endl;
        write << "\t number of iteration = " << sp.num_of_iterrations << endl;
        return write;
    }

    bool SolverParams::operator==(const SolverParams &sp) const
    {
        return abs(field_value - sp.field_value) < EPS && abs(tollerance - sp.tollerance) < EPS && num_of_iterrations == sp.num_of_iterrations && adaptive_refinment_steps == sp.adaptive_refinment_steps && abs(refinment_fraction - sp.refinment_fraction) < EPS && quadrature_degree == sp.quadrature_degree && renumbering_type == sp.renumbering_type && abs(max_distance - sp.max_distance) < EPS;
    }

    // Solver
    void Solver::OpenMesh(const string fileName)
    {
        GridIn<dim> gridin;
        gridin.attach_triangulation(triangulation);
        ifstream f(fileName);
        if (!f)
            throw Exception("OpenMesh: there is no mesh file  - " + fileName);
        gridin.read_msh(f);
    }

    void Solver::OpenMesh(const tethex::Mesh &mesh)
    {
        auto n = mesh.get_vertices().size();

        //vertices conversion
        std::vector<dealii::Point<dim>> vertices(n);
        for(size_t i = 0; i < n; ++i)
        {
            vertices[i](0) = mesh.get_vertex(i).get_coord(0);
            vertices[i](1) = mesh.get_vertex(i).get_coord(1);
        }

        n = mesh.get_quadrangles().size();
        std::vector<CellData<dim>> cells(n);
        for(size_t i = 0; i < n; ++i)
        {
            auto &quadrangle = mesh.get_quadrangle(i);
            cells[i].vertices[0] = quadrangle.get_vertex(0);
            cells[i].vertices[1] = quadrangle.get_vertex(1);
            cells[i].vertices[2] = quadrangle.get_vertex(3);
            cells[i].vertices[3] = quadrangle.get_vertex(2);
            cells[i].material_id = quadrangle.get_material_id();
            cells[i].manifold_id = static_cast<types::manifold_id>(101);
        }

        n = mesh.get_lines().size();
        SubCellData subcelldata;
        subcelldata.boundary_lines.resize(n);
        for(size_t i = 0; i < n; ++i)
        {
            auto &line = mesh.get_line(i);
            subcelldata.boundary_lines[i].vertices[0] = line.get_vertex(0);
            subcelldata.boundary_lines[i].vertices[1] = line.get_vertex(1);
            subcelldata.boundary_lines[i].material_id = line.get_material_id();
            cells[i].manifold_id = static_cast<types::manifold_id>(101);
        }

        triangulation.create_triangulation(vertices, cells, subcelldata);
    }

    void Solver::setBoundaryConditions(const BoundaryConditions &boundary_conds)
    {
        boundary_conditions = boundary_conds;
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

    void Solver::assemble_system(const BoundaryConditions &boundary_conditions)
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

        for (const auto &cell : dof_handler.active_cell_iterators())
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

            auto neuman_bd = boundary_conditions.Get(NEUMAN);
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
        auto dirichlet_bd = boundary_conditions.Get(DIRICHLET);
        for (const auto &[boundary_id, boundary_condition] : dirichlet_bd)
            VectorTools::interpolate_boundary_values(
                dof_handler,
                boundary_id,
                Functions::ConstantFunction<dim>(boundary_condition.value),
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

    double Solver::value(const River::Point &p) const
    {
        Functions::FEFieldFunction<dim> field_function(dof_handler, solution);
        return field_function.value(dealii::Point<dim>{p.x, p.y});
    }

    double Solver::valueSafe(const double x, const double y) const
    {
        Functions::FEFieldFunction<dim> field_function(dof_handler, solution);
        double value;
        try 
        {
            value = field_function.value(dealii::Point<dim>{x, y});
        }
        catch(const VectorTools::ExcPointNotAvailableHere &error)
        {
            value = 0;
        }
        
        return value;
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

    double Solver::max_cell_error()
    {
        Vector<float> estimated_error_per_cell(triangulation.n_active_cells());

        KellyErrorEstimator<dim>::estimate(dof_handler,
                                           QGauss<dim - 1>(3),
                                           map<types::boundary_id, const Function<dim> *>(),
                                           solution,
                                           estimated_error_per_cell);

        return (double)*max_element(begin(estimated_error_per_cell), end(estimated_error_per_cell));
    }

    double Solver::average_cell_error()
    {
        Vector<float> estimated_error_per_cell(triangulation.n_active_cells());

        KellyErrorEstimator<dim>::estimate(dof_handler,
                                           QGauss<dim - 1>(3),
                                           map<types::boundary_id, const Function<dim> *>(),
                                           solution,
                                           estimated_error_per_cell);

        return (double)reduce(begin(estimated_error_per_cell), end(estimated_error_per_cell))
            /static_cast<double>(estimated_error_per_cell.size());
    }

    void Solver::static_refine_grid(unsigned int num_of_static_refinments, const double integration_radius, const t_PointList &tips_points)
    {
        // iterating over refinment steps
        for (unsigned step = 0; step < num_of_static_refinments; ++step)
        {
            // iterating over each mesh cell
            for (auto cell : triangulation.active_cell_iterators())
            {
                for (unsigned int v = 0; v < GeometryInfo<2>::vertices_per_cell; ++v)
                {
                    for (auto &p : tips_points)
                    {
                        auto vertex = cell->vertex(v);
                        auto r = dealii::Point<2>{p.x, p.y}.distance(vertex);
                        if (r < integration_radius)
                        {
                            cell->set_refine_flag();
                            break;
                        }
                    }
                    if (cell->refine_flag_set())
                        break;
                }
            }
            triangulation.execute_coarsening_and_refinement();
        }
    }

    vector<double> Solver::integrate(const IntegrationParams &integ, const Point &point, const double angle)
    {

        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values |
                update_JxW_values);

        // Parameters definition
        vector<double>
            values(quadrature_formula.size()),
            integral(3, 0),
            normalization_integral(3, 0),
            series_params(3, 0); // Series params

        // Iteratation over Mesh and DoFHandler
        for (auto dof_cell : dof_handler.active_cell_iterators())
        {
            // central point of mesh element
            auto center = dof_cell->center();

            // distance between tip point and current mesh element
            auto dx = center[0] - point[0],
                 dy = center[1] - point[1],
                 dist = sqrt(dx * dx + dy * dy);

            // Integrates over points only in this circle
            if (dist <= integ.integration_radius)
            {
                fe_values.reinit(dof_cell);
                fe_values.get_function_values(solution, values);
                auto weight_func_value = integ.WeightFunction(dist);

                // cycle over all series parameters order
                for (unsigned param_index = 0; param_index < series_params.size(); ++param_index)
                {
                    // preevaluate basevector value
                    auto base_vector_value = integ.BaseVectorFinal(param_index + 1, angle, dx, dy);

                    // sum over all quadrature points overIntegrationRadius single mesh element
                    for (unsigned q_point = 0; q_point < quadrature_formula.size(); ++q_point)
                    {
                        // integration of weighted integral..
                        integral[param_index] += values[q_point] * weight_func_value * base_vector_value * fe_values.JxW(q_point);

                        //.. and its normalization integral
                        normalization_integral[param_index] += weight_func_value * pow(base_vector_value, 2) * fe_values.JxW(q_point);
                    }
                }
            }
        }

        for (unsigned i = 0; i < integral.size(); ++i)
            series_params[i] = integral[i] / normalization_integral[i];

        return series_params;
    }

    vector<double> Solver::integrate_new(const IntegrationParams &integ, const Point &tip_coord, const double angle)
    {
        Functions::FEFieldFunction<dim> field_function(dof_handler, solution);

        // Parameters definition
        vector<double>
            integral(3, 0),
            normalization_integral(3, 0),
            series_params(3, 0);                  // Series params
        auto drho = integ.integration_radius / 8; // 8 higher value gives better results
        for (double rho = drho; rho < integ.integration_radius; rho += drho)
        {
            auto weight_func_value = integ.WeightFunction(rho);
            for (double phi = 0; phi < 2 * M_PI; phi += M_PI / 4200 /*4200*/ / rho * integ.integration_radius)
            {
                River::Polar cylind_coord{rho, phi};
                River::Point
                    abs_coord{tip_coord.x + rho * cos(phi + angle), tip_coord.y + rho * sin(phi + angle)},
                    rel_coord = abs_coord - tip_coord;
                double value;
                try
                {
                    value = field_function.value(dealii::Point<dim>{abs_coord.x, abs_coord.y});
                }
                catch (const VectorTools::ExcPointNotAvailableHere &error)
                {
                    continue;
                }

                // cycle over all series parameters order
                for (unsigned param_index = 0; param_index < series_params.size(); ++param_index)
                {
                    // preevaluate basevector value
                    auto base_vector_value = integ.BaseVectorFinal(param_index + 1, angle, rel_coord.x, rel_coord.y);

                    // integration of weighted integral..
                    integral[param_index] += value * weight_func_value * base_vector_value * rho;

                    //.. and its normalization integral
                    normalization_integral[param_index] += weight_func_value * pow(base_vector_value, 2) * rho;
                }
            }
        }

        for (unsigned i = 0; i < integral.size(); ++i)
            series_params[i] = integral[i] / normalization_integral[i];

        return series_params;
    }

    vector<double> Solver::integral_value(const Functions::FEFieldFunction<2> &field_function, 
        const double rho, double phi, const River::Point &tip_coord, const double angle,
        const IntegrationParams &integ)
    {
        vector<double> I(6, 0);

        River::Polar cylind_coord{rho, phi};
        River::Point
            abs_coord{tip_coord.x + rho * cos(phi + angle), tip_coord.y + rho * sin(phi + angle)},
            rel_coord = abs_coord - tip_coord;
        double value = 0;
        try
        {
            value = field_function.value(dealii::Point<dim>{abs_coord.x, abs_coord.y});
        }
        catch (const VectorTools::ExcPointNotAvailableHere &error)
        {}

        // cycle over all series parameters order
        for (unsigned param_index = 0; param_index < 3; ++param_index)
        {
            // preevaluate basevector value
            auto base_vector_value = integ.BaseVectorFinal(param_index + 1, angle, rel_coord.x, rel_coord.y);

            // integration of weighted integral..
            I[param_index] += value * base_vector_value * rho;

            //.. and its normalization integral
            I[3 + param_index] += pow(base_vector_value, 2) * rho;
        }

        return I;
    }

    vector<double> Solver::integral_value_res( 
        const double rho, double phi, const River::Point &tip_coord, const double angle,
        const IntegrationParams &integ)
    {
        Functions::FEFieldFunction<dim> field_function(dof_handler, solution);
        
        vector<double> I(6, 0);

        River::Polar cylind_coord{rho, phi};
        River::Point
            abs_coord{tip_coord.x + rho * cos(phi + angle), tip_coord.y + rho * sin(phi + angle)},
            rel_coord = abs_coord - tip_coord;
        double value = 0;
        try
        {
            value = field_function.value(dealii::Point<dim>{abs_coord.x, abs_coord.y});
        }
        catch (const VectorTools::ExcPointNotAvailableHere &error)
        {}

        // cycle over all series parameters order
        for (unsigned param_index = 0; param_index < 3; ++param_index)
        {
            // preevaluate basevector value
            auto base_vector_value = integ.BaseVectorFinal(param_index + 1, angle, rel_coord.x, rel_coord.y);

            // integration of weighted integral..
            I[param_index] += value * base_vector_value * rho;

            //.. and its normalization integral
            I[3 + param_index] += pow(base_vector_value, 2) * rho;
        }

        return I;
    }

    vector<double> operator*(const vector<double>& v, double alfa)
    {
        auto w{v};
        for(auto & w_el: w)
            w_el = w_el * alfa;
        return w;
    }

    vector<double> operator/(const vector<double>& v, double alfa)
    {
        auto w{v};
        for(auto & w_el: w)
            w_el = w_el / alfa;
        return w;
    }

    vector<double> operator+(const vector<double>& v1, const vector<double>& v2)
    {
        auto w{v1};

        size_t n = 0;
        for(auto & w_el: w)
            w_el = w_el + v2[n++];
        return w;
    }

    vector<double> operator-(const vector<double>& v1, const vector<double>& v2)
    {
        auto w{v1};

        size_t n = 0;
        for(auto & w_el: w)
        {
            w_el = w_el - v2[n];
            n++;
        }
        return w;
    }

    vector<double> operator/(const vector<double>& v1, const vector<double>& v2)
    {
        auto w{v1};

        size_t n = 0;
        for(auto & w_el: w)
        {
            w_el = w_el / v2[n];
            n++;
        }
        return w;
    }

    double max_v(const vector<double>& v)
    {
        auto m = v.at(0);

        for(auto & v_el: v)
            if (v_el > m)
                m = v_el;
        return m;
    }

    vector<double> abs_v(const vector<double>& v)
    {
        auto w{v};
        for(auto & w_el: w)
            if (w_el < 0)
                w_el = -w_el;
        return w;
    }

    vector<double> Solver::integrate_trap(const IntegrationParams &integ, const Point &tip_coord, const double angle)
    {
        Functions::FEFieldFunction<dim> field_function(dof_handler, solution);

        // Parameters definition
        vector<double>
            integral(3, 0),
            normalization_integral(3, 0),
            series_params(3, 0);                  // Series params
        auto drho = integ.integration_radius / integ.n_rho; // 8 higher value gives better results

        auto f = [&](double rho, double phi){return integral_value(field_function, rho, phi, tip_coord, angle, integ);};

        auto jmax = (size_t)15;
        vector<double> s(6, 0), olds(6, 0); 
        auto a = 0., b = 2*M_PI;

        for (double rho = drho; rho < integ.integration_radius; rho += drho)
        {
            auto w = integ.WeightFunction(rho);
            for(size_t j = 0; j < jmax; ++j)
            {
                if (j == 0)
                    s = (f(rho, b) - f(rho, a)) * (b - a) / 2.; 
                else if (j > 0)
                {
                    auto 
                        tnm = pow(2., j - 1.),
                        d = (b - a) / tnm,
                        x = a + 0.5 * d;
                    vector<double> sum(6, 0);
                    for(size_t k = 0; k < tnm; ++k)
                    {
                        sum = sum + f(rho, x);
                        x += d;
                    }
                    s = (s + sum * w * d) / 2.;
                }

                if ( j > 5)
                {
                    auto cur_eps = max_v(abs_v(s - olds) / abs_v(olds));
                    if ((cur_eps) < integ.eps || (max_v(s) < EPS && max_v(olds) < EPS))
                        goto Finish;
                }
                olds = s;
            }
        }
        Finish:

        series_params[0] = s[0] / s[3];
        series_params[1] = s[1] / s[4];
        series_params[2] = s[2] / s[5];

        return series_params;
    }

    double Solver::region_integral(const Point point, const double dr)
    {
        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values |
                update_quadrature_points |
                update_JxW_values);

        const unsigned int n_q_points = quadrature_formula.size();

        vector<double> values(n_q_points);
        double integration_result = 0;

        for (const auto &cell : dof_handler.active_cell_iterators())
        {
            auto center = cell->center();
            auto dx = center[0] - point[0],
                 dy = center[1] - point[1],
                 dist = sqrt(dx * dx + dy * dy);

            if (dist <= dr)
            {
                fe_values.reinit(cell);
                fe_values.get_function_values(solution, values);
                auto &JxW_values = fe_values.get_JxW_values();

                for (unsigned q_point = 0; q_point < n_q_points; ++q_point)
                    integration_result += values[q_point] * JxW_values[q_point];
            }
        }

        return integration_result;
    }

    double Solver::max_value()
    {
        FEValues<dim> fe_values(
            fe, quadrature_formula,
            update_values |
                update_quadrature_points |
                update_JxW_values);

        const unsigned int n_q_points = quadrature_formula.size();

        vector<double> values(n_q_points);
        double max_value = 0;

        for (auto cell : dof_handler.active_cell_iterators())
        {
            fe_values.reinit(cell);
            fe_values.get_function_values(solution, values);

            for (unsigned q_point = 0; q_point < n_q_points; ++q_point)
                if (max_value < values[q_point])
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
        output.precision(20); // Fix for paraview
        data_out.write_vtk(output);

        // ofstream out("grid-"+to_string(cycle)+".eps");
        // GridOut       grid_out;
        // grid_out.write_eps(triangulation, out);
    }

    void Solver::run()
    {
        for (unsigned cycle = 0; cycle <= num_of_adaptive_refinments; ++cycle)
        {
            if (verbose) cout <<  "adaptive mesh solver cycle-#" + to_string(cycle) + "---------------------------" << endl;

            if (cycle > 0) refine_grid();

            if (verbose) cout <<  "   Number of active cells:" << endl;
            if (verbose) cout <<  "\t" + to_string(triangulation.n_active_cells()) << endl;

            setup_system();

            if (verbose) cout <<  "   Number of degrees of freedom:" << endl;
            if (verbose) cout <<  "\t" + to_string(dof_handler.n_dofs()) << endl;
            
            assemble_system(boundary_conditions);
            solve();
        }
    }

    Solver& Solver::operator=(const Solver& s)
    {
        tollerance = s.tollerance;
        number_of_iterations = s.number_of_iterations;
        num_of_adaptive_refinments = s.num_of_adaptive_refinments;
        num_of_static_refinments = s.num_of_static_refinments;
        field_value = s.field_value;
        refinment_fraction = s.refinment_fraction;
        coarsening_fraction = s.coarsening_fraction;

        //dof_handler = DoFHandler<dim>{triangulation};
        //fe = FE_Q<dim>{s.quadrature_degree};
        //quadrature_formula = QGauss<dim>{solver_params.quadrature_degree};
        //face_quadrature_formula = QGauss<dim - 1>{solver_params.quadrature_degree};
        return *this;
    }

} // namespace River
