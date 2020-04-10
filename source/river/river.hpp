#pragma once

#include "physmodel.hpp"
#include "mesh.hpp"
#include "boundary_generator.hpp"

//Deal.II headers
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/logstream.h>
#include <deal.II/base/smartpointer.h>
#include <deal.II/base/convergence_table.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/affine_constraints.h>

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_refinement.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_in.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_renumbering.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/error_estimator.h>
#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/vector_tools.h>

#include <array>
#include <fstream>
#include <iostream>

using namespace dealii;
using namespace std;

namespace River
{
    enum Renumbering 
    {
        Renumbering_none,
        Renumbering_cuthill_McKee, 
        Renumbering_hierarchical,
        Renumbering_random_renumbering,
        Renumbering_block_wise,
        Renumbering_clockwise_dg,
        Renumbering_subdomain_wise
    };

    template <int dim = 2, int spacedim = dim>
    void renumber(const Renumbering renumbering_type, DoFHandler<dim, spacedim> & dof_handler)
    {
        switch(renumbering_type)
        {
            case Renumbering_none:
                break;
            case Renumbering_cuthill_McKee:
                DoFRenumbering::Cuthill_McKee(dof_handler);
                break;
            case Renumbering_hierarchical:
                DoFRenumbering::hierarchical(dof_handler);
                break;
            case Renumbering_random_renumbering:
                DoFRenumbering::random(dof_handler);
                break;
            case Renumbering_block_wise:
                DoFRenumbering::block_wise(dof_handler);
                break;
            case Renumbering_clockwise_dg:
                DoFRenumbering::clockwise_dg(dof_handler, dealii::Point<dim>(0, 0));
                break;
            case Renumbering_subdomain_wise:
                DoFRenumbering::subdomain_wise(dof_handler);
                break;
        }
    }
    
    template<int dim = 2, int spacedim = dim>//currently dim will be only implemented
    class RiverSolver
    {
        public:
            RiverSolver(Model *mdl):
            dof_handler(triangulation),
            fe(mdl->solver_params.quadrature_degree),
            model(mdl)
            {
                deallog.depth_console(3);
                triangle.mesh_params = &(mdl->mesh);
            }

            void one_step_forward()
            {
                for(unsigned i = 0; i < model->prog_opt.number_of_steps; ++i)
                {
                    cout << "=====================================" << endl;
                    dof_handler.clear();
                    triangulation.clear();
                    constraints.clear();
                    system_matrix.clear();
                    solution = 0;
                    system_rhs = 0;

                    cout << "generate_triangulation_file" <<endl;
                    generate_triangulation_file();
                    cout << "initialize_solver_triangulation_from_file" <<endl;
                    initialize_solver_triangulation_from_file();
                    cout << "static_grid_refinement" <<endl;
                    static_grid_refinement();
                    cout << "setup_system" <<endl;
                    setup_system((Renumbering)model->solver_params.renumbering_type);
                    cout << "assemble_system" <<endl;
                    assemble_system();
                    cout << "solve" <<endl;
                    solve(model->solver_params.num_of_iterrations, model->solver_params.tollerance);
                    cout << "refine_grid" <<endl;
                    refine_grid();
                    cout << "grow_tree" <<endl;
                    grow_tree();
                }
            }

            void one_step_backward()
            {

            }

        private:

            void generate_triangulation_file()
            {
                auto boundary = SimpleBoundaryGenerator(*model);
                tethex::Mesh mesh(boundary);
                
                cout << "triangle" <<endl;
                triangle.mesh_params->tip_points = model->tree.TipPoints();
                cout << "generate" <<endl;
                triangle.generate(mesh);//triangulation

                cout << "convert" <<endl;
                mesh.convert();//convertaion from triangles to quadrangles

                cout << "output" <<endl;
                mesh.write(model->prog_opt.output_file_name + ".msh");
            }

            void initialize_solver_triangulation_from_file()
            {
                GridIn<dim> gridin;
                gridin.attach_triangulation(triangulation);
                ifstream f(model->prog_opt.output_file_name + ".msh");
                gridin.read_msh(f);
            }

            void static_grid_refinement()
            {
                auto tips_points = model->tree.TipPoints();
                //iterating over refinment steps
                for (unsigned step = 0; step < model->mesh.static_refinment_steps; ++step)
                {
                    //iterating over each mesh cell   
                    for (auto& cell: triangulation.active_cell_iterators())
                        for (unsigned int v = 0; v < GeometryInfo<dim>::vertices_per_cell; ++v)
                        {
                            for(auto& p: tips_points)
                            {
                                auto vertex = cell->vertex(v);
                                auto r = dealii::Point<dim>{p.x, p.y}.distance(vertex);
                                if(r < model->integr.integration_radius)
                                {
                                    cell->set_refine_flag ();
                                    break;
                                }
                            }
                            if(cell->refine_flag_set())
                                break;
                        }
                    triangulation.execute_coarsening_and_refinement ();
                }
            }

            //linear solver part
            void setup_system(const Renumbering renumbering_type)
            {
                dof_handler.initialize(triangulation, fe);

                renumber<dim>(renumbering_type, dof_handler);

                solution.reinit(dof_handler.n_dofs());
                system_rhs.reinit(dof_handler.n_dofs());

                constraints.clear();

                DoFTools::make_hanging_node_constraints(
                    dof_handler, 
                    constraints);

                VectorTools::interpolate_boundary_values(
                    dof_handler, 
                    1, 
                    Functions::ConstantFunction< dim >(0),
                    constraints);

                constraints.close();

                DynamicSparsityPattern dsp(dof_handler.n_dofs());
                DoFTools::make_sparsity_pattern(
                    dof_handler, 
                    dsp, 
                    constraints, 
                    false);

                sparsity_pattern.copy_from(dsp);
                
                system_matrix.reinit(sparsity_pattern);
            }

            void assemble_system()
            {
                QGauss<dim> quadrature_formula(fe.degree + 1);
                QGauss<dim - 1> face_quadrature_formula(fe.degree + 1);

                FEValues<dim, spacedim> fe_values(
                    fe, 
                    quadrature_formula, 
                    update_values | update_gradients | update_JxW_values);

                FEFaceValues<dim> fe_face_values(
                    fe,
                    face_quadrature_formula,
                    update_values | update_JxW_values);

                const unsigned dofs_per_cell = fe.dofs_per_cell;
                const unsigned n_q_points = quadrature_formula.size();
                const unsigned n_face_q_points = face_quadrature_formula.size();

                FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
                Vector<double> cell_rhs(dofs_per_cell);

                vector<types::global_dof_index> local_dof_indices(dofs_per_cell);

                for(const auto& cell: dof_handler.active_cell_iterators())
                {
                    fe_values.reinit(cell);
                    
                    cell_matrix = 0;
                    cell_rhs = 0;

                    for(unsigned q_index = 0; q_index < n_q_points; ++q_index)
                        for(unsigned i = 0; i < dofs_per_cell; ++i)
                        {
                            for(unsigned j = 0; j < dofs_per_cell; ++j)
                                cell_matrix(i, j) += 
                                    fe_values.shape_grad(i, q_index) *
                                    fe_values.shape_grad(j, q_index) *
                                    fe_values.JxW(q_index);

                            cell_rhs(i) += fe_values.shape_value(i, q_index) *
                                1 *
                                fe_values.JxW(q_index);
                        }

                    for(unsigned face_number = 0; face_number < GeometryInfo<dim>::faces_per_cell; ++face_number)
                        if(cell->face(face_number)->at_boundary() && cell->face(face_number)->boundary_id() == 2)
                        {
                            fe_face_values.reinit(cell, face_number);

                            for(unsigned q_point = 0; q_point < n_face_q_points; ++q_point)
                            {
                                const double neuman_value = 1;
                                for(unsigned i = 0; i < dofs_per_cell; ++i)
                                    cell_rhs(i) += 
                                        neuman_value * 
                                        fe_face_values.shape_value(i, q_point)*
                                        fe_face_values.JxW(q_point);

                            }
                        }

                    cell->get_dof_indices(local_dof_indices);
                    constraints.distribute_local_to_global(
                        cell_matrix, 
                        cell_rhs, 
                        local_dof_indices, 
                        system_matrix, 
                        system_rhs);
                }
            }

            void solve(const unsigned number_of_iterations = 1000, const double tollerance = 1e-12)
            {
                SolverControl solver_control(number_of_iterations, tollerance);
                SolverCG<Vector<double>> solver(solver_control);

                //TODO try different preconditions Jacobi, SparseILU
                PreconditionSSOR<> preconditioner;
                preconditioner.initialize(system_matrix, 1.2);

                solver.solve(
                    system_matrix, 
                    solution, 
                    system_rhs, 
                    preconditioner);

                constraints.distribute(solution);
            }

            vector<double> series_param_integral(const River::Point point, const double angle)
            {
                QGauss<dim> quadrature_formula(fe.degree);

                FEValues<dim> fe_values(
                    fe, quadrature_formula,
                    update_values |
                    update_JxW_values);
    
                vector<double> 
                    values(quadrature_formula.size()),
                    integral(3, 0),
                    normalization_integral(3, 0),
                    series_params(3, 0); //Series params

                cout << "point.... " << point << endl;

                for(auto dof_cell: dof_handler.active_cell_iterators())
                {
                    //central point of mesh element
                    auto center = dof_cell->center();

                    //distance between tip point and current mesh element
                    auto dx = center[0] - point[0],
                        dy = center[1] - point[1],
                        dist = sqrt(dx*dx + dy*dy);

                    //Integrates over points only in this circle
                    if(dist <= model->integr.integration_radius)
                    {
                        fe_values.reinit (dof_cell);
                        fe_values.get_function_values(solution, values);
                        auto weight_func_value = model->integr.WeightFunction(dist);

                        //cycle over all series parameters order
                        for(unsigned param_index = 0; param_index < series_params.size(); ++param_index)
                        {
                            //preevaluate basevector value
                            auto base_vector_value = model->integr.BaseVectorFinal(param_index + 1, angle, dx, dy);

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

            void refine_grid(const double refinemnt_fraction = 0.3, const double coarsening_fraction = 0.03)
            {
                Vector< float > estimate_error_per_cell(triangulation.n_active_cells());

                KellyErrorEstimator<dim>::estimate(
                    dof_handler,
                    QGauss<dim - 1>(fe.degree + 1),
                    map<types::boundary_id, const Function<dim> *>(),
                    solution, 
                    estimate_error_per_cell
                );

                GridRefinement::refine_and_coarsen_fixed_number(
                    triangulation, 
                    estimate_error_per_cell,
                    refinemnt_fraction, 
                    coarsening_fraction
                );

                triangulation.execute_coarsening_and_refinement();
            }

            void grow_tree(double max_a_backward = -1)
            {
                //Iterate over each tip and handle branch growth and its bifurcations
                map<int, vector<double>> id_series_params;
                for(auto id: model->tree.TipBranchesIds())
                {
                    auto tip_point = model->tree.at(id).TipPoint();
                    auto tip_angle = model->tree.at(id).TipAngle();
                    id_series_params[id] = series_param_integral(tip_point, tip_angle);
                }

                //Evaluate maximal a parameter to normalize growth of speed to all branches ds = dt*v / max(v_array).
                double max_a = 0.;
                if(max_a_backward > 0)
                    max_a = max_a_backward;
                else 
                    for(auto&[id, series_params]: id_series_params)
                        if (max_a < series_params.at(0))
                            max_a = series_params.at(0);

                for(auto&[id, series_params]: id_series_params)
                    if(model->q_growth(series_params))
                    {
                        if(model->q_bifurcate(series_params, model->tree.at(id).Lenght()))
                        {
                            auto tip_point = model->tree.at(id).TipPoint();
                            auto tip_angle = model->tree.at(id).TipAngle();
                            auto br_left = BranchNew(tip_point, tip_angle + model->bifurcation_angle);
                            br_left.AddPoint(Polar{model->ds, 0});
                            auto br_right = BranchNew(tip_point, tip_angle - model->bifurcation_angle);
                            br_right.AddPoint(Polar{model->ds, 0});
                            model->tree.AddSubBranches(id, br_left, br_right);
                        }
                        else
                            model->tree.at(id).AddPoint(model->next_point(series_params, model->tree.at(id).Lenght(), max_a));
                    }
            }

            double MaximalA1Value(map<int, vector<double>> ids_seriesparams_map)
            {
                double max_a = 0.;
                for(auto&[id, series_params]: ids_seriesparams_map)
                    if(max_a < series_params.at(0))
                        max_a = series_params.at(0);

                return max_a;
            }

            void process_solution()
            {

            }

            void output_results(bool generateSparsityPattern = false)
            {
                ofstream out("sparsity_pattern_.svg");
                sparsity_pattern.print_svg(out);
                out.close();
            }

            Triangulation<dim> triangulation;
            DoFHandler<dim> dof_handler;

            const FE_Q<dim> fe;

            AffineConstraints<double> constraints;

            SparsityPattern sparsity_pattern;
            SparseMatrix<double> system_matrix;

            Vector<double> solution;
            Vector<double> system_rhs;

            Renumbering renumbering_type = Renumbering_none;

            ConvergenceTable convergence_table;

            Model *model;

            Triangle triangle;
    };

} //namespace River