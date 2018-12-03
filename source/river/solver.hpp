#pragma once

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/dofs/dof_accessor.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_refinement.h>

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
#include <utility>

#include "common.hpp"

using namespace dealii;

namespace River
{

class Simulation
{
  public:
    Simulation();
    ~Simulation();
    void SetMesh(struct vecTriangulateIO &mesh);
    void OpenMesh(string fileName = "river.msh");
    void run();

  private:
    const static int dim = 2;

    Triangulation<dim> triangulation;

    FE_Q<dim> fe;
    DoFHandler<dim> dof_handler;

    ConstraintMatrix constraints;

    SparseMatrix<double> system_matrix;
    SparsityPattern sparsity_pattern;

    Vector<double> solution;
    Vector<double> system_rhs;

    void setup_system();
    void assemble_system();
    void solve();
    void refine_grid();
    void output_results(const unsigned int cycle) const;

    void TryInsertCellBoundary(
        CellData<dim> &cellData,
        struct SubCellData &subcelldata,
        std::unordered_map<std::pair<int, int>, int> &bound_ids,
        int v1, int v2);

    class RightHandSide : public Function<dim>
    {
      public:
        RightHandSide() : Function<dim>() {}
        virtual double value(const dealii::Point<dim> &p,
                             const unsigned int component = 0) const;
    };

    class BoundaryValues : public Function<dim>
    {
      public:
        BoundaryValues() : Function<dim>() {}
        virtual double value(const dealii::Point<dim> &p,
                             const unsigned int component = 0) const;
    };
};

} // namespace River