#pragma once

#include "physmodel.hpp"
#include "mesh.hpp"

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
            RiverSolver(const FE_Q< dim, spacedim> &fe, Model *model):
            dof_handler(triangulation),
            fe(&fe),
            model(model)
            {
                deallog.depth_console(3);
            }

            void run()
            {

            }


        private:

            //geometry part
            void set_boundary()
            {

            }

            void generate_triangulation_file()
            {
                auto mesh = BoundaryGenerator(*model, model->tree, model->border);
                
                triangle.mesh_params->tip_points = model->tree.TipPoints();
                triangle.generate(mesh);//triangulation
            
                mesh.convert();//convertaion from triangles to quadrangles
                model->mesh.number_of_quadrangles = mesh.get_n_quadrangles(); // just saving number of quadrangles

                mesh.write(model->prog_opt.output_file_name + ".msh");

            }

            void initialize_solver_triangulation_from_file()
            {

            }

            void transform_triangulation()
            {

            }

            //linear solver part
            void setup_system(const Renumbering renumbering_type)
            {
                dof_handler.distribute_dofs(fe);

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

            vector<double> series_param_integral(const Model &mdl, const dealii::Point<dim> point, const double angle)
            {
                QGauss<dim> quadrature_formula(fe.degree + 1);

                FEValues<dim> fe_values(
                    fe, quadrature_formula,
                    update_values |
                    update_JxW_values);
    
                vector<double> 
                    values(quadrature_formula.size()),
                    integral(3, 0),
                    normalization_integral(3, 0),
                    series_params(3, 0); //Series params

                for(auto dof_cell: dof_handler.active_cell_iterators())
                {
                    //central point of mesh element
                    auto center = dof_cell->center();

                    //distance between tip point and current mesh element
                    auto dx = center(0) - point(0),
                        dy = center(1) - point(1),
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

            SmartPointer<const FiniteElement<dim>> fe;

            AffineConstraints<double> constraints;

            SparsityPattern sparsity_pattern;
            SparseMatrix<double> system_matrix;

            Vector<double> solution;
            Vector<double> system_rhs;

            ConvergenceTable convergence_table;

            Model *model;

            Triangle triangle;
    };

} //namespace River