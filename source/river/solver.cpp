#include "solver.hpp"

namespace River
{

/*
    Solver Class

*/

Solver::Solver() : fe(2), dof_handler(triangulation)
{
}

Solver::~Solver()
{
    clear();
}

void Solver::SetBoundaryRegionValue(std::vector<int> regionTags, double value)
{
    boundaryRegionValue.insert(std::make_pair(value, regionTags));
}


void Solver::OpenMesh(string fileName)
{
    GridIn<dim> gridin;
    gridin.attach_triangulation(triangulation);
    std::ifstream f(fileName);
    gridin.read_msh(f);
}



void Solver::SetMesh(tethex::Mesh &meshio)
{
    cout << "set_mesh" << endl;
    //VERTICES
    auto n_vertices = meshio.get_n_vertices(); 
    vector<dealii::Point<dim>> vertices(n_vertices);
    for (unsigned int i = 0; i < n_vertices; ++i)
    {
        auto vertice = meshio.get_vertex(i);
        vertices[i] = dealii::Point<dim>(vertice.get_coord(0), vertice.get_coord(1));
    }

    cout << "incidence_matrix" << endl;
    //initialize edge enumeration structure
    auto incidence_matrix = tethex::IncidenceMatrix(meshio.get_n_vertices(), meshio.lines);

    //QUADRANGLES
    cout << "quads" << endl << flush;
    auto n_cells = meshio.get_n_quadrangles();
    vector<CellData<dim>> cells(n_cells, CellData<dim>());
    SubCellData sub_cells;
    cout << "for" << endl;
    for(int i = 0; i < n_cells; ++i)
    {
        cells[i].material_id = meshio.get_quadrangle(i).get_material_id();
        
        auto v0 = cells[i].vertices[0] = meshio.get_quadrangle(i).get_vertex(0);
        auto v1 = cells[i].vertices[1] = meshio.get_quadrangle(i).get_vertex(1);
        auto v2 = cells[i].vertices[2] = meshio.get_quadrangle(i).get_vertex(2);
        auto v3 = cells[i].vertices[3] = meshio.get_quadrangle(i).get_vertex(3);
        
        /*
                        TODO: boundary conditions here are wrong! fix it
                    */
        auto cellEdges = 
            //vector<pair<unsigned int, unsigned int>>{{v0, v2}, {v1, v3}, {v0, v1}, {v2, v3}};
            vector<pair<unsigned int, unsigned int>>{{v0, v1}, {v1, v2}, {v2, v3}, {v3, v0}};
        
        //Boundary IDS
        for(auto & edge: cellEdges)
        {
            int lineIndex = incidence_matrix.find(edge.first, edge.second);
            int boundary_id;
            if(lineIndex > 0)
                boundary_id = meshio.get_line(lineIndex).get_material_id();
            else
                boundary_id = numbers::internal_face_boundary_id;
            
            CellData<1> cell_data;

            cout << "boundary lines" << endl;
            cout << edge.first << "  " << edge.second << endl;
            cout << boundary_id << endl;
            
            cell_data.vertices[0] = edge.first;
            cell_data.vertices[1] = edge.second;
            cell_data.boundary_id = boundary_id;
            
            sub_cells.boundary_lines.push_back(cell_data);
        }
        
    }

    
    GridReordering<dim, dim>::invert_all_cells_of_negative_grid (vertices, cells);
    GridReordering<dim, dim>::reorder_cells (cells);
    triangulation.create_triangulation_compatibility(vertices, cells, sub_cells);

}


double Solver::RightHandSide::value(const dealii::Point<dim> & /*p*/,
                                        const unsigned int /*component*/) const
{
    double return_value = fieldValue;

    return return_value;
}


double Solver::BoundaryValues::value(const dealii::Point<dim> & p,
                                         const unsigned int component) const
{
    if (component == 0)
      return (p[0] < 0 ? -1 : (p[0] > 0 ? 1 : 0));
    return 0;
}


void Solver::setup_system()
{
    dof_handler.distribute_dofs(fe);

    solution.reinit(dof_handler.n_dofs());
    system_rhs.reinit(dof_handler.n_dofs());

    constraints.clear();
    DoFTools::make_hanging_node_constraints(
        dof_handler, constraints);

    for(auto &key: boundaryRegionValue)
        for(auto regionTag: key.second)
            VectorTools::interpolate_boundary_values(
                dof_handler, regionTag, ConstantFunction<dim>(key.first), constraints);
    

    constraints.close();
    DynamicSparsityPattern dsp(dof_handler.n_dofs());
    DoFTools::make_sparsity_pattern(
        dof_handler, dsp, constraints, false);
    sparsity_pattern.copy_from(dsp);
    system_matrix.reinit(sparsity_pattern);
}


double coefficient(const dealii::Point<2> &p)
{
    //if (p.square() < 0.5*0.5)
    //  return 20;
    //else
    return 1;
}


void Solver::assemble_system()
{
    const QGauss<dim> quadrature_formula(3);
    FEValues<dim> fe_values(fe, quadrature_formula,
                            update_values | update_gradients |
                                update_quadrature_points | update_JxW_values);
    const unsigned int dofs_per_cell = fe.dofs_per_cell;
    const unsigned int n_q_points = quadrature_formula.size();
    FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
    Vector<double> cell_rhs(dofs_per_cell);
    std::vector<types::global_dof_index> local_dof_indices(dofs_per_cell);
    typename DoFHandler<dim>::active_cell_iterator
        cell = dof_handler.begin_active(),
        endc = dof_handler.end();
    for (; cell != endc; ++cell)
    {
        cell_matrix = 0;
        cell_rhs = 0;
        fe_values.reinit(cell);
        for (unsigned int q_index = 0; q_index < n_q_points; ++q_index)
        {
            const double current_coefficient = coefficient(fe_values.quadrature_point(q_index));
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
                for (unsigned int j = 0; j < dofs_per_cell; ++j)
                    cell_matrix(i, j) += (current_coefficient *
                                          fe_values.shape_grad(i, q_index) *
                                          fe_values.shape_grad(j, q_index) *
                                          fe_values.JxW(q_index));
                cell_rhs(i) += (fe_values.shape_value(i, q_index) *
                                1.0 *
                                fe_values.JxW(q_index));
            }
        }
        cell->get_dof_indices(local_dof_indices);
        constraints.distribute_local_to_global(cell_matrix,
                                               cell_rhs,
                                               local_dof_indices,
                                               system_matrix,
                                               system_rhs);
    }
}


void Solver::solve()
{
    SolverControl solver_control(4000, 1e-6);
    SolverCG<> solver(solver_control);
    PreconditionSSOR<> preconditioner;
    preconditioner.initialize(system_matrix, 1.2);
    solver.solve(system_matrix, solution, system_rhs,
                 preconditioner);
    constraints.distribute(solution);
}


void Solver::refine_grid()
{
    Vector<float> estimated_error_per_cell(triangulation.n_active_cells());
    KellyErrorEstimator<dim>::estimate(dof_handler,
                                       QGauss<dim - 1>(3),
                                       typename FunctionMap<dim>::type(),
                                       solution,
                                       estimated_error_per_cell);
    GridRefinement::refine_and_coarsen_fixed_number(triangulation,
                                                    estimated_error_per_cell,
                                                    0.3, 0.03);
    triangulation.execute_coarsening_and_refinement();
}


vector<double> Solver::integrate(Point point, double angle)
{   

    const QGauss<dim> quadrature_formula(3);
    FEValues<dim> fe_values(fe, quadrature_formula,
                            update_values            |
                            update_quadrature_points |
                            update_JxW_values);

    const unsigned int dofs_per_cell = fe.dofs_per_cell;
    const unsigned int n_q_points    = quadrature_formula.size();
    
    std::vector<double> values(n_q_points);
    std::vector<double> series_params(3, 0); //Series params
    
    for (auto cell: dof_handler.active_cell_iterators())
    {
        fe_values.reinit (cell);
        fe_values.get_function_values(solution, values);
        auto quad_points = fe_values.get_quadrature_points();

        for (unsigned int q_point = 0; q_point < n_q_points; ++q_point)
        {
            auto quad_point = quad_points[q_point];
            auto dx = (quad_point[0] - point[0]), 
                dy = (quad_point[1] - point[1]);

            double dist = sqrt(dx*dx + dy*dy);
            double angle = Point::angle(dx, dy);

            if(dist >= Model::Rmin && dist <= Model::Rmax)
                for(int param_index = 0; param_index < series_params.size(); ++param_index)
                    for (unsigned int i = 0; i < dofs_per_cell; ++i)
                        series_params[param_index] += 
                            fe_values.shape_value(i, q_point)
                             * values[q_point]
                             * fe_values.JxW(q_point)
                             * Model::Gain(param_index)(dist, angle);
        }
    }
    
    return series_params;
}



void Solver::output_results(const unsigned int cycle) const
{
    DataOut<dim> data_out;
    data_out.attach_dof_handler(dof_handler);
    data_out.add_data_vector(solution, "solution");
    data_out.build_patches();
    std::ofstream output("solution-" + std::to_string(cycle) + ".vtk");
    data_out.write_vtk(output);

    std::ofstream out("grid-"+std::to_string(cycle)+".eps");
    GridOut       grid_out;
    grid_out.write_eps(triangulation, out);

}


void Solver::run(int step)
{
    for (unsigned int cycle = 0; cycle < numOfRefinments; ++cycle)
    {
        std::cout << "Cycle " << cycle << ':' << std::endl;
        if (cycle > 0)
            refine_grid();

        std::cout << "   Number of active cells:       "
                  << triangulation.n_active_cells()
                  << std::endl;
        setup_system();
        std::cout << "   Number of degrees of freedom: "
                  << dof_handler.n_dofs()
                  << std::endl;
        assemble_system();
        solve();
        
    }

    output_results(step);
}

} // namespace Riverestimated_error_per_cell