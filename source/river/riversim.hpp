#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#include <iostream>
#include <fstream>

#include <tetgen.h>
#include <gmsh.h>
#include "triangle.h"
#include "tethex.hpp"

using namespace dealii;
using namespace std;
using namespace gmsh;

namespace po = boost::program_options;
namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;

class RiverSim
{
    public:
        RiverSim(po::variables_map &vm);
        ~RiverSim();
        void run();

    private:
        void geo_mesh_generator();
        void gmsh_mesh_generator();
        void mesh_covertor();
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

        //options fro command line 
        po::variables_map    option_map;  
};


RiverSim::RiverSim (po::variables_map &vm): fe (1), dof_handler(triangulation)
{
    option_map = vm;

    /*
        GMSH intialization
    */
    gmsh::initialize();
    gmsh::option::setNumber("Mesh.RecombineAll", 1);
    gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::option::setNumber("General.Terminal", (int)option_map["gmsh-log"].as<bool>());
    mdl::add("square");
}


RiverSim::~RiverSim()
{
    gmsh::finalize();
}


void RiverSim::geo_mesh_generator()
{
    geo::addPoint(0, 0, 0, 0.1, 1);
    geo::addPoint(0.5, 0, 0, 0.1, 2);
    geo::addPoint(1, 0, 0, 0.1, 3);
    geo::addPoint(1, 1, 0, 0.1, 4);
    geo::addPoint(0, 1, 0, 0.1, 5);
    geo::addPoint(0.5, 0.2, 0, 0.1, 6);

    geo::addLine(1, 2, 1);
    geo::addLine(2, 3, 2);
    geo::addLine(3, 4, 3);
    geo::addLine(4, 5, 4);
    geo::addLine(5, 1, 5);
    geo::addLine(2, 6, 6);
    geo::addLine(6, 2, 7);

    geo::addCurveLoop({1, 2, 3, 4, 5, 6, 7}, 1);
    geo::addPlaneSurface({1}, 6);
    geo::synchronize();
    mdl::mesh::generate(2);
}


void RiverSim::gmsh_mesh_generator()
{
    //TODO: implement right workwflow
    
    /*
        defining of geometry
    */
    const int geomTag = 1;
    cout << "discr entity" << endl;
    
    mdl::addDiscreteEntity(dim, geomTag);
    
    //node points
    //auto nodesTag = {1, 2, 3, 4, 5};
    cout << "set nodes" << endl;
    msh::setNodes(dim, geomTag, 
        {1, 2, 3, 4}, 
        {0.,  0.,  0.,   //node 1 
         1.,  0.,  0.,   //node 2
         1.,  1.,  0.,   //node 3
         0.,  1.,  0.}); //node 4           

    cout << "set elements" << endl;
    msh::setElements( dim, geomTag, 
        {1},//line element 
        {{1,2,3,4}},//line tags
        {{1,2,2,3,3,4,4,1}}); //lines
    
    mdl::mesh::generate(2);
}


void RiverSim::make_custom_grid()
{
    static const Point<2> vertices_1[] = 
    {   Point<2> (0., 0.),
        Point<2> (1,  0.),
        Point<2> (0,  1.),
        Point<2> (1., 1.)
    }; 
    
    const unsigned int
    n_vertices = sizeof(vertices_1) / sizeof(vertices_1[0]);
    
    const vector<Point<dim> > vertices (&vertices_1[0],
                                         &vertices_1[n_vertices]);
    
    static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell]
            = {{0, 1, 2, 3}};

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
    
    triangulation.refine_global (5);
}

void RiverSim::setup_system ()
{
    dof_handler.distribute_dofs (fe);
    cout << "Number of degrees of freedom: "
              << dof_handler.n_dofs()
              << endl; 

    DynamicSparsityPattern dsp(dof_handler.n_dofs());

    DoFTools::make_sparsity_pattern (dof_handler, dsp);
    sparsity_pattern.copy_from(dsp);
    system_matrix.reinit (sparsity_pattern);
    solution.reinit (dof_handler.n_dofs());
    system_rhs.reinit (dof_handler.n_dofs());
}

void RiverSim::assemble_system ()
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
        for (unsigned int q_index = 0; q_index < n_q_points; ++q_index)
        {
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                for (unsigned int j = 0; j < dofs_per_cell; ++j)
                    cell_matrix(i, j) += (fe_values.shape_grad (i, q_index) *
                                     fe_values.shape_grad (j, q_index) *
                                     fe_values.JxW (q_index));    
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                 cell_rhs(i) += (fe_values.shape_value (i, q_index) *
                              1 *
                              fe_values.JxW (q_index));
        }

        cell->get_dof_indices (local_dof_indices);

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
            for (unsigned int j = 0; j < dofs_per_cell; ++j)
                system_matrix.add (local_dof_indices[i],
                               local_dof_indices[j],
                               cell_matrix(i, j));

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
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

void RiverSim::solve ()
{
    SolverControl           solver_control (1000, 1e-12);
    SolverCG<>              solver (solver_control);
    solver.solve (system_matrix, solution, system_rhs,
                  PreconditionIdentity());
}


void RiverSim::output_results () const
{
    DataOut<2> data_out;
    data_out.attach_dof_handler (dof_handler);
    data_out.add_data_vector (solution, "solution");
    data_out.build_patches ();
    std::ofstream output ("solution.vtk");
    data_out.write_vtk (output);
}


void RiverSim::run ()
{
    make_custom_grid ();
    setup_system ();
    assemble_system ();
    solve ();
    output_results ();

    if (option_map["draw-mesh"].as<bool>())
        gmsh::fltk::run();

    if(option_map.count("output"))
        gmsh::write(option_map["output"].as<string>());
}

void gmsh_possibilites(int argc, char *argv[]);