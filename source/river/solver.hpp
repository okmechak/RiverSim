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

/*! \file solver.hpp 
    \brief PDE Solver and Integration tool.
    \details Incapsulates all logic of Deal.II PDE solver library and postprocessing of its results.
*/
#pragma once

///\cond
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/base/convergence_table.h>
#include <deal.II/base/logstream.h>
#include <deal.II/base/types.h>

#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_refinement.h>
#include <deal.II/grid/grid_reordering.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_renumbering.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/affine_constraints.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/error_estimator.h>
#include <deal.II/numerics/data_out.h>

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <unordered_map>
#include <map>
#include <utility>
///\endcond

#include "GeometryPrimitives.hpp"
#include "tethex.hpp"
#include "physmodel.hpp"

using namespace dealii;

namespace River
{
    /*! \brief Deal.II Solver Wrapper 
        \details
        For more details read [Deal.II ste-6 tutorial](https://www.dealii.org/current/doxygen/deal.II/step_6.html).
    */
    class Solver
    {
        public:
            ///Solver constructor
            Solver(Model *model): 
                model(model),
                dof_handler(triangulation),
                fe(model->solver_params.quadrature_degree), 
                quadrature_formula(model->solver_params.quadrature_degree),
                face_quadrature_formula(model->solver_params.quadrature_degree)
            { 
                tollerance = model->solver_params.tollerance;
                number_of_iterations = model->solver_params.num_of_iterrations;
                num_of_adaptive_refinments = model->solver_params.adaptive_refinment_steps;
                refinment_fraction = model->solver_params.refinment_fraction;
                verbose = model->prog_opt.verbose;
                field_value = model->field_value;
                num_of_static_refinments = model->mesh.static_refinment_steps;
            };
            
            ~Solver(){clear();}

            ///Solver tollerance
            double tollerance = 1e-12;

            ///Number of solver iterations.
            unsigned number_of_iterations = 6000;

            ///If true, output will be produced to stadard output
            bool verbose = false;

            ///Number of adaptive mesh refinments.
            ///Splits mesh elements and resolves.
            unsigned num_of_adaptive_refinments = 0;

            ///Number of static mesh refinments.
            ///Splits elements without resolving.
            unsigned num_of_static_refinments = 0;

            ///Open mesh data from file. Msh 2 format.
            void OpenMesh(const string fileName = "river.msh");

            ///Static adaptive mesh refinment.
            void static_refine_grid(const Model& mdl, const vector<Point>& tips_points);

            ///Number of refined by Deal.II mesh cells.
            unsigned long NumberOfRefinedCells()
            {
                return triangulation.n_active_cells();
            }

            unsigned long NumberOfDOFs()
            {
                return dof_handler.n_dofs();
            }

            ///Run fem solution.
            void run();

            ///Save results to VTK file.
            void output_results(const string file_name) const;

            ///Interation of series parameters around tips points.
            vector<double> integrate(const Model& mdl, const Point& point, const double angle);

            ///Integration used for test purpose.
            double integration_test(const Point& point, const double dr);

            ///Maximal value of solution, used for test purpose.
            double max_value();

            bool solved() const 
            {
                return solution.size() > 0;
            }

            ///Clear Solver object.
            void clear()
            {
              dof_handler.clear();
              triangulation.clear();
              hanging_node_constraints.clear();
              system_matrix.clear();
              solution.reinit(0);
            }
            
            ///Outer field value. See Puasson, Laplace equations.
            double field_value = 0.;

            ///Refinment fraction. Used static mesh elements refinment.
            double refinment_fraction = 0.01;
            
            ///Coarsening fraction. Used static mesh elements refinment.
            double coarsening_fraction = 0;

        private:
            Model *model = NULL;

            ///Dimension of problem.
            const static int dim = 2;

            Triangulation<dim> triangulation;
            DoFHandler<dim> dof_handler;

            FE_Q<dim> fe;
            const QGauss<dim> quadrature_formula;
            const QGauss<dim - 1> face_quadrature_formula;
            
            AffineConstraints<double> hanging_node_constraints;

            SparsityPattern sparsity_pattern;
            SparseMatrix<double> system_matrix;

            ///Holds solution of problem.
            Vector<double> solution;
            ///Holds right hand side values of linear system.
            Vector<double> system_rhs;

            ConvergenceTable convergence_table;

            void setup_system();
            void assemble_system();
            void solve();
            void refine_grid();
    };
} // namespace River