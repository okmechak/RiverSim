#include <deal.II/grid/tria.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/dofs/dof_tools.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>
#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/numerics/data_out.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace dealii;




class Step3
{
  public:
    Step3 ();
      void run ();
  private:
    void make_grid ();
    void make_custom_grid();
    void setup_system ();
    void assemble_system ();
    void solve ();
    void output_results () const;
    const static int dim = 2;
    Triangulation<dim>   triangulation;
    FE_Q<dim>            fe;
    DoFHandler<dim>      dof_handler;
    SparsityPattern      sparsity_pattern;
    SparseMatrix<double> system_matrix;
    Vector<double>       solution;
    Vector<double>       system_rhs;
};


Step3::Step3 (): fe (1), dof_handler (triangulation){}

void Step3::make_custom_grid()
{
  static const Point<2> vertices_1[]
    = {  Point<2> (-1.,   -1.),
         Point<2> (-1./2, -1.),
         Point<2> (0.,    -1.),
         Point<2> (+1./2, -1.),
         Point<2> (+1,    -1.),
         Point<2> (-1.,   -1./2.),
         Point<2> (-1./2, -1./2.),
         Point<2> (0.,    -1./2.),
         Point<2> (+1./2, -1./2.),
         Point<2> (+1,    -1./2.),
         Point<2> (-1.,    0.),
         Point<2> (-1./2,  0.),
         Point<2> (+1./2,  0.),
         Point<2> (+1,     0.),
         Point<2> (-1.,    1./2.),
         Point<2> (-1./2,  1./2.),
         Point<2> (0.,     1./2.),
         Point<2> (+1./2,  1./2.),
         Point<2> (+1,     1./2.),
         Point<2> (-1.,    1.),
         Point<2> (-1./2,  1.),
         Point<2> (0.,     1.),
         Point<2> (+1./2,  1.),
         Point<2> (+1,     1.)
      }; 
  const unsigned int
  n_vertices = sizeof(vertices_1) / sizeof(vertices_1[0]);
  const vector<Point<dim> > vertices (&vertices_1[0],
                                           &vertices_1[n_vertices]);
  static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell]
  = {{0, 1, 5, 6},
    {1, 2, 6, 7},
    {2, 3, 7, 8},
    {3, 4, 8, 9},
    {5, 6, 10, 11},
    {8, 9, 12, 13},
    {10, 11, 14, 15},
    {12, 13, 17, 18},
    {14, 15, 19, 20},
    {15, 16, 20, 21},
    {16, 17, 21, 22},
    {17, 18, 22, 23}
  };
  const unsigned int
  n_cells = sizeof(cell_vertices) / sizeof(cell_vertices[0]);

  vector<CellData<dim> > cells (n_cells, CellData<dim>());

  for (unsigned int i=0; i<n_cells; ++i)
  {
    for (unsigned int j=0;
         j<GeometryInfo<dim>::vertices_per_cell;
         ++j)
      cells[i].vertices[j] = cell_vertices[i][j];
    cells[i].material_id = 0;
  }
  triangulation.create_triangulation (vertices,
                                      cells,
                                      SubCellData());
  triangulation.refine_global (1);
}


void Step3::make_grid ()
{
  GridGenerator::hyper_cube (triangulation, -1, 1);
  triangulation.refine_global (5);
  std::cout << "Number of active cells: "
            << triangulation.n_active_cells()
            << std::endl;
}


void Step3::setup_system ()
{
  dof_handler.distribute_dofs (fe);
  std::cout << "Number of degrees of freedom: "
            << dof_handler.n_dofs()
            << std::endl;

  DynamicSparsityPattern dsp(dof_handler.n_dofs());
  DoFTools::make_sparsity_pattern (dof_handler, dsp);
  sparsity_pattern.copy_from(dsp);
  system_matrix.reinit (sparsity_pattern);
  solution.reinit (dof_handler.n_dofs());
  system_rhs.reinit (dof_handler.n_dofs());
}


void Step3::assemble_system ()
{
  QGauss<2>  quadrature_formula(2);
  FEValues<2> fe_values (fe, quadrature_formula,
                         update_values | update_gradients | update_JxW_values);
  const unsigned int   dofs_per_cell = fe.dofs_per_cell;
  const unsigned int   n_q_points    = quadrature_formula.size();
  FullMatrix<double>   cell_matrix (dofs_per_cell, dofs_per_cell);
  Vector<double>       cell_rhs (dofs_per_cell);
  std::vector<types::global_dof_index> local_dof_indices (dofs_per_cell);
  DoFHandler<2>::active_cell_iterator
  cell = dof_handler.begin_active(),
  endc = dof_handler.end();
  for (; cell!=endc; ++cell)
  {
    fe_values.reinit (cell);
    cell_matrix = 0;
    cell_rhs = 0;
    for (unsigned int q_index=0; q_index<n_q_points; ++q_index)
    {
      for (unsigned int i=0; i<dofs_per_cell; ++i)
        for (unsigned int j=0; j<dofs_per_cell; ++j)
          cell_matrix(i,j) += (fe_values.shape_grad (i, q_index) *
                               fe_values.shape_grad (j, q_index) *
                               fe_values.JxW (q_index));
                               
      for (unsigned int i=0; i<dofs_per_cell; ++i)
        cell_rhs(i) += (fe_values.shape_value (i, q_index) *
                        1 *
                        fe_values.JxW (q_index));
    }
    cell->get_dof_indices (local_dof_indices);

    for (unsigned int i=0; i<dofs_per_cell; ++i)
      for (unsigned int j=0; j<dofs_per_cell; ++j)
        system_matrix.add (local_dof_indices[i],
                           local_dof_indices[j],
                           cell_matrix(i,j));
    for (unsigned int i=0; i<dofs_per_cell; ++i)
      system_rhs(local_dof_indices[i]) += cell_rhs(i);
  }

  std::map<types::global_dof_index,double> boundary_values;
  VectorTools::interpolate_boundary_values (dof_handler,
                                            0,
                                            ZeroFunction<2>(),
                                            boundary_values);

  MatrixTools::apply_boundary_values (boundary_values,
                                      system_matrix,
                                      solution,
                                      system_rhs);
}


void Step3::solve ()
{
  SolverControl           solver_control (1000, 1e-12);
  SolverCG<>              solver (solver_control);
  solver.solve (system_matrix, solution, system_rhs,
                PreconditionIdentity());
}


void Step3::output_results () const
{
  DataOut<2> data_out;
  data_out.attach_dof_handler (dof_handler);
  data_out.add_data_vector (solution, "solution");
  data_out.build_patches ();
  std::ofstream output ("solution.gpl");
  data_out.write_gnuplot (output);
}


void Step3::run ()
{
  make_custom_grid ();
  setup_system ();
  assemble_system ();
  solve ();
  output_results ();
}


int main ()
{
  deallog.depth_console (2);
  Step3 laplace_problem;
  laplace_problem.run ();
  return 0;
}