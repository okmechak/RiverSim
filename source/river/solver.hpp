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
#include <deal.II/base/types.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_renumbering.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_refinement.h>
#include <deal.II/grid/grid_reordering.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/constraint_matrix.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/error_estimator.h>

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
///\endcond

#include "common.hpp"
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
            Solver(int quadrature_degree = 2): fe(2), dof_handler(triangulation),quadrature_formula(quadrature_degree){};
            
            ~Solver(){clear();}///<Solver desctructor

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

            ///Inner function
            void SetBoundaryRegionValue(const vector<int>& regionTags, const double value);
            /*!
              \todo implement this function
            */
            void SetMesh(const tethex::Mesh &meshio);

            ///Open mesh data from file. Msh 2 format.
            void OpenMesh(const string fileName = "river.msh");

            ///Static adaptive mesh refinment.
            void static_refine_grid(const Model& mdl, const vector<Point>& tips_points);

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

            ///Clear Solver object.
            void clear()
            {
              dof_handler.clear();
              triangulation.clear();
              constraints.clear();
              system_matrix.clear();
            }
            
            ///Outer field value. See Puasson, Laplace equations.
            double field_value = 1.;

            ///Refinment fraction. Used static mesh elements refinment.
            double refinment_fraction = 0.01;
            
            ///Coarsening fraction. Used static mesh elements refinment.
            double coarsening_fraction = 0;

        private:
            ///Dimension of problem.
            const static int dim = 2;

            Triangulation< dim> triangulation;///<See Deal.II from more deatails.

            ///Used for setup boudary conditions.
            map<double, vector<int>> boundaryRegionValue;

            FE_Q<dim> fe;///<See Deal.II from more deatails.
            
            DoFHandler<dim> dof_handler;///<See Deal.II from more deatails.
            
            const QGauss<dim> quadrature_formula;///<See Deal.II from more deatails.
            
            ConstraintMatrix constraints;///<See Deal.II from more deatails.

            SparseMatrix<double> system_matrix;///<See Deal.II from more deatails.

            SparsityPattern sparsity_pattern;///<See Deal.II from more deatails.

            ///Holds solution of problem.
            Vector<double> solution;
            ///Holds right hand side values of linear system.
            Vector<double> system_rhs;

            ///See Deal.II ste-6 from more deatails.
            void setup_system();
            ///Creates linear system.
            void assemble_system();
            ///Runs solver
            void solve();
            ///Refines grid.
            void refine_grid();

            ///Unknown.
            void TryInsertCellBoundary(
                CellData<dim> &cellData,
                struct SubCellData &subcelldata,
                std::unordered_map<std::pair<int, int>, int> &bound_ids,
                int v1, int v2);
            
            ///See Deal.II step-6 for more details.
            class RightHandSide : public Function<dim>
            {
              public:
                ///Value of field in Puasson equation.
                double fieldValue = 1.;
                ///See Deal.II step-6 for more details.
                RightHandSide() : Function<dim>() {}
                ///See Deal.II step-6 for more details.
                virtual double value(const dealii::Point<dim> &p,
                                     const unsigned int component = 0) const;
            };

            ///See Deal.II step-6 for more details.
            class BoundaryValues : public Function<dim>
            {
              public:
                ///See Deal.II step-6 for more details.
                BoundaryValues() : Function<dim>() {}
                ///See Deal.II step-6 for more details.
                virtual double value(const dealii::Point<dim> &p,
                                     const unsigned int component = 0) const;
            };
    };
} // namespace River