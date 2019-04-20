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

/** 
 * @file solver.hpp
 * 
 * PDE Solver and Integration tool.
 *
 * Incapsulates all logic of Deal.II PDE solver library and postprocessing of its results.
 */
#pragma once

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

#include "common.hpp"
#include "tethex.hpp"
#include "physmodel.hpp"

using namespace dealii;

namespace River
{
    /**
     * Deal.II Solver Wrapper 
     */
    class Solver
    {
        public:
          Solver(int quadrature_degree = 2): fe(2), dof_handler(triangulation),quadrature_formula(quadrature_degree){};
          ~Solver(){clear();}

          unsigned int num_of_refinments = 1;
          void SetBoundaryRegionValue(std::vector<int> regionTags, double value);
          void SetMesh(tethex::Mesh &meshio);
          void OpenMesh(string fileName = "river.msh");
          void run();
          void output_results(const string file_name) const;
          vector<double> integrate(Model& mdl, Point point, double angle);
          double integration_test(Point point, double dr);
          double max_value();
          void clear()
          {
            dof_handler.clear();
            triangulation.clear();
            constraints.clear();
            system_matrix.clear();
          }

          double field_value = 1.;
          double refinment_fraction = 0.01;
          double coarsening_fraction = 0;

        private:
          const static int dim = 2;

          Triangulation<dim> triangulation;

          std::map<double, std::vector<int>> boundaryRegionValue;

          FE_Q<dim> fe;
          DoFHandler<dim> dof_handler;
          const QGauss<dim> quadrature_formula;

          ConstraintMatrix constraints;

          SparseMatrix<double> system_matrix;
          SparsityPattern sparsity_pattern;

          Vector<double> solution;
          Vector<double> system_rhs;


          void setup_system();
          void assemble_system();
          void solve();
          void refine_grid();

          void TryInsertCellBoundary(
              CellData<dim> &cellData,
              struct SubCellData &subcelldata,
              std::unordered_map<std::pair<int, int>, int> &bound_ids,
              int v1, int v2);

          class RightHandSide : public Function<dim>
          {
            public:
              double fieldValue = 1.;
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