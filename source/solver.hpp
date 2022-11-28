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
#include <functional>
#include <math.h>
///\endcond

#include "boundary.hpp"
#include "tethex.hpp"

using namespace dealii;

namespace River
{
    ///Enumeration of different boundary conditions
    enum t_boundary 
    {
        DIRICHLET = 0, 
        NEUMAN
    }; 

    /*! \struct BoundaryCondition
        \brief Describes boudary condition type.
    */
    struct BoundaryCondition
    {
        BoundaryCondition() = default;
        BoundaryCondition(t_boundary t, double v): type(t), value(v) {};
        t_boundary type = DIRICHLET;
        double value = 0;
        bool operator==(const BoundaryCondition& bc) const;
        friend ostream& operator <<(ostream& write, const BoundaryCondition & boundary_condition);
    };
    
    ///Map structure of boundary condition types.
    typedef map<t_boundary_id, BoundaryCondition> t_BoundaryConditions;
    
    /*! \class BoundaryConditions
        \brief Map structure of boundary condition types.
        
        \details Holds for each id its appropraite boundary condition.
    */
    class BoundaryConditions: public t_BoundaryConditions
    {
        public:
            ///Returns map structure with boundary conditions of specific type(Neuman or Dirichlet).
            t_BoundaryConditions Get(t_boundary type) const;
    };
    
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

        ///Series parameters precision integration
        double eps = 1e-10;

        /// Rho integration step
        double n_rho = 8;

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

        bool operator==(const IntegrationParams &ip) const
        {
            return abs(weigth_func_radius - ip.weigth_func_radius) < EPS 
                && abs(integration_radius - ip.integration_radius) < EPS 
                && abs(exponant - ip.exponant) < EPS
                && abs(eps - ip.eps) < EPS
                && abs(n_rho - ip.n_rho) < EPS;
        }
    };

    class Quadrature
    {
        public:
            size_t n = 0;
            virtual double next() = 0;
    };


    template<typename T> class Trapzd: virtual public Quadrature
    {
        public:
            T f;
            double s = 0, a, b;

            Trapzd(T func, double aa, double bb):
                f{func},
                a{aa},
                b{bb}
            {}

            double next()
            {
                ++n;
                if (n==1)
                    s = (b - a) / 2. * (f(a) + f(b));
                else if (n > 1)
                {
                    auto 
                        tnm = pow(2., n - 2.),
                        d = (b - a) / tnm,
                        x = a + 0.5 * d,
                        sum = 0.;
                    for(size_t j = 0; j < tnm; ++j)
                    {
                        sum += f(x);
                        x += d;
                    }
                    s = 0.5 * (s + (b - a) * sum / tnm);
                }
                return s;
            }
    };

    //vector<double> operator+(vector<double>& a, vector<double>& b)
    //{
    //    vector<double> c;
    //    c.resize(a.size());
    //    for(size_t i = 0; i < c.size(); ++i)
    //        c[i] = a[i] + b[i];
    //    return c;
    //};

    template<typename T>
    double qtrap(T func, double a, double b, double eps)
    {
        auto jmax = (size_t)30;
        auto s = 0., olds = 0.;
        auto t = Trapzd(func, a, b);
        for (size_t i = 0; i < jmax; ++i)
        {
            s = t.next();
            if ( i > 5)
            {
                auto cur_eps = abs(s - olds) / abs(olds);
                if ((cur_eps) < eps || (s < EPS && olds < EPS))
                    return s;
            }
            olds = s;
        }
        throw Exception("qtrap: Too many steps in routine.");
    }

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

        bool operator==(const SolverParams &sp) const
        {
            return abs(field_value - sp.field_value) < EPS 
                && abs(tollerance - sp.tollerance) < EPS 
                && num_of_iterrations == sp.num_of_iterrations 
                && adaptive_refinment_steps == sp.adaptive_refinment_steps 
                && static_refinment_steps == sp.static_refinment_steps 
                && abs(refinment_fraction - sp.refinment_fraction) < EPS 
                && quadrature_degree == sp.quadrature_degree 
                && renumbering_type == sp.renumbering_type;
        }
    };

    enum Renumbering 
    {
        Renumbering_none,
        Renumbering_cuthill_McKee, 
        Renumbering_hierarchical,
        Renumbering_random_renumbering,
        Renumbering_block_wise,
        Renumbering_clockwise_dg,
        Renumbering_subdomain_wise
    };

    template <int dim = 2, int spacedim = dim>
    void renumber(const Renumbering renumbering_type, DoFHandler<dim, spacedim> & dof_handler)
    {
        switch(renumbering_type)
        {
            case Renumbering_none:
                break;
            case Renumbering_cuthill_McKee:
                DoFRenumbering::Cuthill_McKee(dof_handler);
                break;
            case Renumbering_hierarchical:
                DoFRenumbering::hierarchical(dof_handler);
                break;
            case Renumbering_random_renumbering:
                DoFRenumbering::random(dof_handler);
                break;
            case Renumbering_block_wise:
                DoFRenumbering::block_wise(dof_handler);
                break;
            case Renumbering_clockwise_dg:
                DoFRenumbering::clockwise_dg(dof_handler, dealii::Point<dim>(0, 0));
                break;
            case Renumbering_subdomain_wise:
                DoFRenumbering::subdomain_wise(dof_handler);
                break;
        }
    }
    
    /*! \brief Deal.II Solver Wrapper
        \details
        For more details read [Deal.II ste-6 tutorial](https://www.dealii.org/current/doxygen/deal.II/step_6.html).
    */
    class Solver
    {
    public:
        /// Solver constructor
        Solver(
            const SolverParams& solver_params):
            dof_handler{triangulation},
            fe{solver_params.quadrature_degree},
            quadrature_formula{solver_params.quadrature_degree},
            face_quadrature_formula{solver_params.quadrature_degree}

        {
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

        /// Interation of series parameters around tips points using trapezoidal recursive integration.
        vector<double> integrate_trap(const IntegrationParams &integ, const Point &point, const double angle);

        /// Evaluate integral values.
        vector<double> integral_value(const Functions::FEFieldFunction<2> &field_function, 
            const double rho, const double phi, 
            const River::Point &tip_coord, const double angle, const IntegrationParams &integ);

        /// Evaluate integral values.
        vector<double> integral_value_res( 
            const double rho, const double phi, 
            const River::Point &tip_coord, const double angle, const IntegrationParams &integ);

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