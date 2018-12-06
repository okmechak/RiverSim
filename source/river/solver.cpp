#include "solver.hpp"

namespace River
{

/*
    Simulation Class

*/

Simulation::Simulation() : fe(2), dof_handler(triangulation)
{
}

Simulation::~Simulation()
{
    system_matrix.clear();
}

void Simulation::TryInsertCellBoundary(
    CellData<dim> &cellData,
    struct SubCellData &subcelldata,
    std::unordered_map<std::pair<int, int>, int> &bound_ids,
    int v1, int v2)
{
    CellData<1> boundary;
    pair<int, int> p;
    if (bound_ids.count(p = make_pair(v1, v2)) &&
        bound_ids[p] != 0)
    {
        boundary.boundary_id = bound_ids[p];
        boundary.vertices[0] = v1;
        boundary.vertices[1] = v2;
        subcelldata.boundary_lines.push_back(boundary);
        cellData.manifold_id = boundary.boundary_id;
    }
    else if (bound_ids.count(p = make_pair(v2, v1)) &&
             bound_ids[p] != 0)
    {
        boundary.boundary_id = bound_ids[p];
        boundary.vertices[0] = v2;
        boundary.vertices[1] = v1;
        subcelldata.boundary_lines.push_back(boundary);
        cellData.manifold_id = boundary.boundary_id;
    }
}


void Simulation::OpenMesh(string fileName)
{
    GridIn<dim> gridin;
    gridin.attach_triangulation(triangulation);
    std::ifstream f(fileName);
    gridin.read_msh(f);
}


void Simulation::SetMesh(struct vecTriangulateIO &mesh)
{

    //VERTICES
    auto n_points = mesh.points.size() / 3; //FIXME: replace hardcoded 3 by more general thing
    vector<dealii::Point<dim>> vertices(n_points);
    for (unsigned int i = 0; i < n_points; ++i)
        vertices[i] = dealii::Point<dim>(mesh.points[3 * i], mesh.points[3 * i + 1]);

    //generat boundary id structure
    unordered_map<std::pair<int, int>, int> bound_id;
    for (unsigned int i = 0; i < mesh.segments.size() / 2; ++i)
        bound_id.insert(make_pair(
            make_pair(mesh.segments[2 * i], mesh.segments[2 * i + 1]), mesh.segmentMarkers[i]));

    //SETTING QUADRANGLES
    auto n_cells = mesh.triangles.size() / 4; //FIXME: remove hardcode
    vector<CellData<dim>> cells(n_cells, CellData<dim>());
    auto subCell = SubCellData();
    for (unsigned int i = 0; i < n_cells; ++i)
    {
        auto v1 = cells[i].vertices[0] = mesh.triangles[4 * i + 0] - 1;
        auto v2 = cells[i].vertices[1] = mesh.triangles[4 * i + 1] - 1;
        auto v4 = cells[i].vertices[2] = mesh.triangles[4 * i + 3] - 1;
        auto v3 = cells[i].vertices[3] = mesh.triangles[4 * i + 2] - 1;
        TryInsertCellBoundary(cells[i], subCell, bound_id, v1, v2);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v2, v3);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v3, v4);
        TryInsertCellBoundary(cells[i], subCell, bound_id, v4, v1);

        //if(mesh.numOfAttrPerTriangle > 0)
        //    cells[i].material_id = mesh.triangleAttributes[i];
        //else
        //    cells[i].material_id = 0;
    }
    cout << "checking consistency" << endl;
    if (subCell.check_consistency(dim))
        cout << "i don't know what it means" << endl;
    triangulation.create_triangulation(vertices,
                                       cells,
                                       subCell);
}


double Simulation::RightHandSide::value(const dealii::Point<dim> & /*p*/,
                                        const unsigned int /*component*/) const
{
    double return_value = 1.0;

    return return_value;
}


double Simulation::BoundaryValues::value(const dealii::Point<dim> & /*p*/,
                                         const unsigned int /*component*/) const
{
    return 0.;
}


void Simulation::setup_system()
{
    dof_handler.distribute_dofs(fe);

    solution.reinit(dof_handler.n_dofs());
    system_rhs.reinit(dof_handler.n_dofs());

    constraints.clear();
    DoFTools::make_hanging_node_constraints(
        dof_handler, constraints);

    VectorTools::interpolate_boundary_values(
        dof_handler, 0, ZeroFunction<dim>(), constraints);

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


void Simulation::assemble_system()
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


void Simulation::solve()
{
    SolverControl solver_control(4000, 1e-12);
    SolverCG<> solver(solver_control);
    PreconditionSSOR<> preconditioner;
    preconditioner.initialize(system_matrix, 1.2);
    solver.solve(system_matrix, solution, system_rhs,
                 preconditioner);
    constraints.distribute(solution);
}


void Simulation::refine_grid()
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


void Simulation::output_results(const unsigned int cycle) const
{
    DataOut<dim> data_out;
    data_out.attach_dof_handler(dof_handler);
    data_out.add_data_vector(solution, "solution");
    data_out.build_patches();
    std::ofstream output("solution-" + std::to_string(cycle) + ".vtk");
    data_out.write_vtk(output);
}


void Simulation::run()
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
        output_results(cycle);
    }
}

} // namespace River