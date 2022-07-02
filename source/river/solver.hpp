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
#include <deal.II/numerics/fe_field_function.h>

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <unordered_map>
#include <map>
#include <utility>
///\endcond

#include "boundary.hpp"
#include "tethex.hpp"

using namespace dealii;

namespace River
{
    /*! \brief Holds parameters used by integration of series paramets functionality(see River::Solver::integrate())
     */
    class IntegrationParams
    {
    public:
        /*! \brief Circle radius with centrum in tip point.
            \details Parameter is used in River::IntegrationParams::WeightFunction
        */
        double weigth_func_radius = 0.01;

        /*! \brief Circle radius with centrum in tip point.
            \details Parameter is used in River::IntegrationParams::WeightFunction
        */
        double integration_radius = 3 * 0.01;

        /*! \brief Controls slope.
            \details Parameter is used in River::IntegrationParams::WeightFunction
        */
        double exponant = 2.;

        /// Weight function used in computation of series parameters.
        inline double WeightFunction(const double r) const
        {
            //! [WeightFunc]
            return exp(-pow(r / weigth_func_radius, exponant));
            //! [WeightFunc]
        }

        /// Base Vector function used in computation of series parameters.
        inline double BaseVector(const int nf, const complex<double> zf) const
        {
            if ((nf % 2) == 0)
                return -imag(pow(zf, nf / 2.));
            else
                return real(pow(zf, nf / 2.));
        }

        /// Base Vector function used in computation of series parameters.
        inline double BaseVectorFinal(const int nf, const double angle, const double dx, const double dy) const
        {
            return BaseVector(nf,
                              exp(-complex<double>(0.0, 1.0) * angle) * (dx + complex<double>(0.0, 1.0) * dy));
        }

        /// Prints options structure to output stream.
        friend ostream &operator<<(ostream &write, const IntegrationParams &ip);

        bool operator==(const IntegrationParams &ip) const;
    };

    /*! \brief Holds All parameters used in Deal.II solver.
     */
    class SolverParams
    {
    public:
        ///Field value used for Poisson conditions.
        double field_value = 1.0;
        
        /// Tollerarnce used by dealii Solver.
        double tollerance = 1.e-12;

        /// Number of solver iteration steps.
        unsigned num_of_iterrations = 6000;

        /// Number of adaptive refinment steps.
        unsigned adaptive_refinment_steps = 0;

        /*! \brief Number of mesh refinment steps used by Deal.II mesh functionality.
            \details Refinment means splitting one rectangular element into four rectagular elements.
        */
        unsigned static_refinment_steps = 0;

        /// Fraction of refined mesh elements.
        double refinment_fraction = 0.1;

        /// Polynom degree of quadrature integration.
        unsigned quadrature_degree = 3;

        /// Renumbering algorithm(0 - none, 1 - cuthill McKee, 2 - hierarchical, 3 - random, ...) for the degrees of freedom on a triangulation.
        unsigned renumbering_type = 0;

        /// Maximal distance between middle point and first solved point, used in non euler growth.
        double max_distance = 0.002;

        /// Prints program options structure to output stream.
        friend ostream &operator<<(ostream &write, const SolverParams &mp);

        bool operator==(const SolverParams &sp) const;
    };
    
    /*! \brief Deal.II Solver Wrapper
        \details
        For more details read [Deal.II ste-6 tutorial](https://www.dealii.org/current/doxygen/deal.II/step_6.html).
    */
    class Solver
    {
    public:
        /// Solver constructor
        Solver(
            const SolverParams& solver_params, 
            const bool verb):
            dof_handler{triangulation},
            fe{solver_params.quadrature_degree},
            quadrature_formula{solver_params.quadrature_degree},
            face_quadrature_formula{solver_params.quadrature_degree}

        {
            verbose = verb;
            tollerance = solver_params.tollerance;
            number_of_iterations = solver_params.num_of_iterrations;
            num_of_adaptive_refinments = solver_params.adaptive_refinment_steps;
            refinment_fraction = solver_params.refinment_fraction;
            field_value = solver_params.field_value;
            num_of_static_refinments = solver_params.static_refinment_steps;
        };

        ~Solver() { clear(); }

        /// Solver tollerance
        double tollerance = 1e-12;

        /// Number of solver iterations.
        unsigned number_of_iterations = 6000;

        /// Number of adaptive mesh refinments. Splits mesh elements and resolves.
        unsigned num_of_adaptive_refinments = 0;

        /// Number of static mesh refinments. Splits elements without resolving.
        unsigned num_of_static_refinments = 0;

        /// Open mesh data from file. Msh 2 format.
        void OpenMesh(const string fileName = "river.msh");

        /// Open mesh data from object.
        void OpenMesh(const tethex::Mesh &mesh);

        ///Sets boundary conditions of solver.
        void setBoundaryConditions(const BoundaryConditions &boundary_conds);

        /// Static adaptive mesh refinment.
        void static_refine_grid(unsigned int num_of_static_refinments, const double integration_radius, const t_PointList &tips_points);

        /// Number of refined by Deal.II mesh cells.
        unsigned long NumberOfRefinedCells()
        {
            return triangulation.n_active_cells();
        }

        unsigned long NumberOfDOFs()
        {
            return dof_handler.n_dofs();
        }

        /// Run fem solution.
        void run();

        /// Save results to VTK file.
        void output_results(const string file_name) const;

        /// Interation of series parameters around tips points.
        vector<double> integrate(const IntegrationParams &integ, const Point &point, const double angle);

        /// Interation of series parameters around tips points using better values evaluation, but slower.
        vector<double> integrate_new(const IntegrationParams &integ, const Point &point, const double angle);

        /// Integration used for test purpose.
        double region_integral(const Point point = River::Point{0, 0}, const double dr = 100);

        /// Maximal value of solution, used for test purpose.
        double max_value();

        bool solved() const
        {
            return solution.size() > 0;
        }

        /// Clear Solver object.
        void clear()
        {
            dof_handler.clear();
            triangulation.clear();
            hanging_node_constraints.clear();
            system_matrix.clear();
            //solution.reinit(0);
        }

        void setup_system();
        void assemble_system(const BoundaryConditions & boundary_conditions);
        void solve();
        double value(const River::Point &p) const;
        double valueSafe(const double x, const double y) const;
        void refine_grid();
        double max_cell_error();
        double average_cell_error();

        ///Equal operator
        Solver& operator=(const Solver& s);

        /// Outer field value. See Puasson, Laplace equations.
        double field_value = 0.;

        /// Refinment fraction. Used static mesh elements refinment.
        double refinment_fraction = 0.01;

        /// Coarsening fraction. Used static mesh elements refinment.
        double coarsening_fraction = 0;

    private:

        /// Dimension of problem.
        const static int dim = 2;

        Triangulation<dim> triangulation;

        DoFHandler<dim> dof_handler;
        FE_Q<dim> fe;
        QGauss<dim> quadrature_formula;
        QGauss<dim - 1> face_quadrature_formula;
        /// If true, output will be produced to stadard output.
        bool verbose = false;
        BoundaryConditions boundary_conditions;

        AffineConstraints<double> hanging_node_constraints;

        SparsityPattern sparsity_pattern;
        SparseMatrix<double> system_matrix;

        /// Holds solution of problem.
        Vector<double> solution;

        /// Holds right hand side values of linear system.
        Vector<double> system_rhs;

        ConvergenceTable convergence_table;
    };
} // namespace River