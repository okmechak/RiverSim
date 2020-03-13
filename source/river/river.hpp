#pragma once

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
    struct Exception : public exception
    {
        string s;
        Exception(string ss) : s(ss) {}
        ~Exception() throw () {} // Updated
        const char* what() const throw() { return s.c_str(); }
    };
    

    template<int dim = 2, int spacedim = dim>//currently dim will be only implemented
    class RiverSolver
    {
        public:
            RiverSolver(const FE_Q< dim, spacedim> &fe):
            dof_handler(triangulation),
            fe(&fe)
            {
                deallog.depth_console(3);
            }

            void run(){}


        private:

            //geometry part
            void set_boundary();
            void generate_triangulation();
            void transform_triangulation();

            //linear solver part
            void setup_system();
            void assemble_system();
            void solve();
            void refine_grid();
            void process_solution();

            Triangulation<dim> triangulation;
            DoFHandler<dim> dof_handler;

            SmartPointer<const FiniteElement<dim>> fe;

            AffineConstraints<double> hanging_node_constraints;

            SparsityPattern      sparsity_pattern;
            SparseMatrix<double> system_matrix;

            Vector<double> solution;
            Vector<double> system_rhs;

            ConvergenceTable convergence_table;
    };

} //namespace River