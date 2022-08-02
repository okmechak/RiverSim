#pragma once

///\cond
#include <iostream>
#include <cmath>
#include <complex>

#include <algorithm>
#include <map>
#include <chrono>
#include <time.h>
#include <numeric>
///\endcond

#include "rivers.hpp"
#include "solver.hpp"
#include "triangle.hpp"
#include "region.hpp"

using namespace std;

namespace River
{

    /*! \brief Physical model parameters.
        \details 
        Holds parameters related to model. Region, numerical preocessing, parameters of growth etc.
    */
    class Model
    {   
        public: 

            void InitializeLaplace();
            void InitializePoisson();
            void InitializeDirichlet();
            void InitializeDirichletWithHole();

            //Simulation methods
            void clear();

            //Growth controll functions
            bool q_bifurcate(const vector<double>& a) const;

            bool q_bifurcate(const vector<double>& a, double branch_lenght) const;

            ///Growth condition.
            ///Checks series parameters around river tip and evaluates if it is enough to grow.
            bool q_growth(const vector<double>& a) const;

            ///Evaluate next point of simualtion based on series parameters around tip.
            Polar next_point(const vector<double>& series_params) const;

            Polar next_point(const vector<double>& series_params, double branch_lenght, double max_a);

            ///Prints model structure and its subclasses
            bool operator==(const Model& model) const;

            Region region;

            Boundary boundary;

            Sources sources;

            Rivers rivers;

            RegionParams region_params;

            ///Mesh and mesh refinment parameters
            MeshParams mesh_params;
            
            BoundaryConditions boundary_conditions;
            
            ///Solver parameters used by Deal.II
            SolverParams solver_params;

            ///Series parameteres integral parameters
            IntegrationParams integr;

            //Geometrical parameters
            ///Number of simulation steps.
            unsigned number_of_steps = 10;

            ///Initial x position of source.
            ///Valid only for rectangular area.
            double dx = 0.2;

            ///Width of region
            double width = 1.;

            ///Height of region
            double height = 1.;

            //Numeriacal parameters
            ///Maximal length of one step of growth.
            double ds = 0.01;

            ///Eta. Growth power of a1^eta
            double eta = 1.0;

            ///Bifurcation method type, 0 - no bif, 1 - a(3)/a(1) > bifurcation_threshold, 2 - a1 > bifurcation_threshold, 3 - combines both conditions, 4 - a1 * p(random number) >= threshold.
            unsigned bifurcation_type = 1;
            
            ///Bifurcation threshold for "0" bifurcation type.
            double bifurcation_threshold = -0.1;//Probably should be -0.1

            ///Minimal distance between adjacent bifurcation points. Reduces numerical noise.
            double bifurcation_min_dist = 0.05;

            ///Bifurcation angle.
            double bifurcation_angle = M_PI/5;

            ///Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}
            unsigned growth_type = 1;

            ///Growth of branch will be done only if a1 > growth-threshold.
            double growth_threshold = 0;

            ///Distance of constant tip growing after bifurcation point. Reduces numerical noise.
            double growth_min_distance = 0.005;
    };
}
