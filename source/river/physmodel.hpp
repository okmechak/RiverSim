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

/*! \file physmodel.hpp
    \brief Contains all RiverSim program parameters.
    \details RiverSim - is big program with a lot of parameters.
    Program handles geometry, boudary conditions, mesh generation, FEM solvers etc. And each this module has
    a lot of parameters. 

    These parameters can be specified by JSON file(\ref io.hpp) or through program options(\ref River::process_program_options).
 */
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

#include "tree.hpp"
#include "mesh.hpp"
#include "solver.hpp"

using namespace std;

namespace River
{

    class Parameter
    {};

    typedef map<t_branch_id, vector<vector<double>>> t_SeriesParameters;
    class SeriesParameters: public t_SeriesParameters
    {
        public:
            void record(const map<t_branch_id, vector<double>>& id_series_params);
    };
    
    typedef map<string, vector<double>> SimulationData;

    struct BackwardData
    {
        vector<double> a1, a2, a3;
        t_PointList init, backward, backward_forward;
        double branch_lenght_diff = -1;

        bool operator==(const BackwardData& data) const;

        friend ostream& operator <<(ostream& write, const BackwardData & data);
    };

    typedef map<t_branch_id, BackwardData> t_GeometryDiffernceNew;

    /*! \brief Global program options. 
        \details Program has some options that isn't part simulation itself but rather ease of use.
        So this object is dedicated for such options.
     */
    class ProgramOptions
    {
        public:
            ///Simulation type: 0 - Forward, 1 - Backward, 2 - For test purposes
            unsigned simulation_type = 0;

            ///Number of simulation steps.
            unsigned number_of_steps = 10;

            ///This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.
            double maximal_river_height = 100;

            ///Number of backward steps simulations used in backward simulation type.
            unsigned number_of_backward_steps = 1;

            ///Outputs VTK file of Deal.II solution
            bool save_vtk = false;

            bool save_each_step = false;

            ///If true - then program will print to standard output.
            bool verbose = true;

            ///If true - then program will save additional output files for each stage of simulation.
            bool debug = false;

            string output_file_name = "simdata",
                input_file_name = "";
            
            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const ProgramOptions & po);

            bool operator==(const ProgramOptions& po) const;
    };

    /*! \brief Physical model parameters.
        \details 
        Holds parameters related to model. Region, numerical preocessing, parameters of growth etc.
    */
    class Model
    {   
        public: 
            ///Some global program options
            ProgramOptions prog_opt;

            Region border;

            Sources sources;

            Tree tree, saved_tree;

            BoundaryConditions boundary_conditions;

            ///Mesh and mesh refinment parameters
            MeshParams mesh;

            ///Series parameteres integral parameters
            IntegrationParams integr;

            ///Solver parameters used by Deal.II
            SolverParams solver_params;

            SeriesParameters series_parameters;

            SimulationData sim_data;

            t_GeometryDiffernceNew backward_data;

            void InitializeLaplace();
            void InitializePoisson();
            void InitializeDirichlet();
            void InitializeDirichletWithHole();
            void Clear();

            //Simulation methods

            void RevertLastSimulationStep();
            
            void SaveCurrentTree()
            {
                saved_tree = tree;
            }
            void RestoreTree()
            {
                tree = saved_tree;
            }

            //Geometrical parameters
            ///Initial x position of source.
            ///Valid only for rectangular area.
            double dx = 0.2;

            ///Width of region
            double width = 1.;

            ///Height of region
            double height = 1.;

            ///river boundary id and bottom line
            unsigned river_boundary_id = 100;

            //Numeriacal parameters
            ///Maximal length of one step of growth.
            double ds = 0.01;

            ///Eta. Growth power of a1^eta
            double eta = 1.0;

            ///Bifurcation method type, 0 - no bif, 1 - a(3)/a(1) > bifurcation_threshold, 2 - a1 > bifurcation_threshold, 3 - combines both conditions.
            unsigned bifurcation_type = 1;
            
            ///Bifurcation threshold for "0" bifurcation type.
            double bifurcation_threshold = -0.1;//Probably should be -0.1

            ///Minimal distance between adjacent bifurcation points. Reduces numerical noise.
            double bifurcation_min_dist = 0.05;

            ///Bifurcation angle.
            double bifurcation_angle = M_PI/5;

            ///Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}
            unsigned growth_type = 0;

            ///Growth of branch will be done only if a1 > growth-threshold.
            double growth_threshold = 0;

            ///Distance of constant tip growing after bifurcation point. Reduces numerical noise.
            double growth_min_distance = 0.01;
            
            ///Checks by evaluating series params for bifuraction condition.
            bool q_bifurcate(const vector<double>& a) const;

            bool q_bifurcate(const vector<double>& a, double branch_lenght) const;

            ///Growth condition.
            ///Checks series parameters around river tip and evaluates if it is enough to grow.
            bool q_growth(const vector<double>& a) const;

            ///Evaluate next point of simualtion based on series parameters around tip.
            Polar next_point(const vector<double>& series_params) const;

            Polar next_point(const vector<double>& series_params, double branch_lenght, double max_a);

            ///Checks if values of parameters are in normal ranges.
            void CheckParametersConsistency() const;

            ///Prints model structure and its subclasses
            friend ostream& operator <<(ostream& write, const Model & mdl);

            bool operator==(const Model& model) const;
    };
}
