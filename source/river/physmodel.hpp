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

#include "GeometryPrimitives.hpp"
#include "tree.hpp"
#include "boundary.hpp"
#include "tethex.hpp"

using namespace std;

///Characteristic radius used by River::MeshParams object.
#define Radius 0.01

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
    
    /*! \brief Adaptive mesh area constraint function.
        \details
        MeshParams holds all parameters used by mesh generation(see \ref triangle.hpp, \ref mesh.hpp)
     */
    class MeshParams
    {
        public:
            /*! \brief Vector of tip points.
                \details Tips - are points where mesh size should be small for better accuracy.
                in this case it corresponds to river tip points.
            */
            vector<Point> tip_points;

            ///Radius of mesh refinment.
            double refinment_radius = 4*Radius;

            ///This value controlls transition slope between small mesh elements and big or course.
            double exponant = 7.;

            /*! \brief Sigma is used in exponence, also as \ref River::MeshParams::exponant controls slope. */
            double sigma = 1.9;

            /*! \brief Number of mesh refinment steps used by Deal.II mesh functionality.
                \details Refinment means splitting one rectangular element into four rectagular elements.
            */ 
            unsigned static_refinment_steps = 1;

            ///Minimal area of mesh.
            double min_area = 7e-4;

            ///Maximal area of mesh element.
            double max_area = 1e5;

            ///Minimal angle of mesh element.
            double min_angle = 30.;

            /*! \brief Maximal edge size.
                \todo implement checks for this values.
            */
            double max_edge = 1;

            ///Minimal edge size.
            double min_edge = 8.e-8;

            /*! \brief Ratio of the triangles: 
                
                \details
                Aspect ratio of a triangle is the ratio of the longest edge to shortest edge. 
                AR = abc/(8(s-a)(s-b)(s-c)) 
                Value 2 correspond to 30 degree.

                \todo check if it is implemented
                \todo handle edge values of ration which will correspond to 35 degree.
            */
            double ratio = 2.3;

            /*! \brief Width of branch.
                \details
                eps is width of splitting branch of tree into two lines, to make one border line.
                This is when tree and border is converted into one boundary line.
                \todo eps should depend on elementary step size __ds__.
            */
            double eps = 1e-6;

            /*! \brief Evaluates mesh area constraint at {x, y} point.
                \details

                ### detailed implementation of function:  

                \snippet physmodel.hpp MeshConstrain
            */
            inline double operator()(double x, double y) const
            {
                double result_area = 10000000/*some large area value*/;
                for(auto& tip: tip_points)
                {
                    auto r = (Point{x, y} - tip).norm();
                    auto exp_val = exp( -pow(r/refinment_radius, exponant)/2./sigma/sigma);
                    auto cur_area = min_area + (max_area - min_area)*(1. - exp_val)/(1. + exp_val);
                    if(result_area > cur_area)
                        result_area = cur_area;   
                }
                
                return result_area;
            }

            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const MeshParams & mp);

            bool operator==(const MeshParams& mp) const;
    };

    /*! \brief Holds parameters used by integration of series paramets functionality(see River::Solver::integrate())
    */
    class IntegrationParams
    {
        public:
            /*! \brief Circle radius with centrum in tip point.
                \details Parameter is used in River::IntegrationParams::WeightFunction
            */
            double weigth_func_radius = Radius;

            /*! \brief Circle radius with centrum in tip point.
                \details Parameter is used in River::IntegrationParams::WeightFunction
            */
            double integration_radius = 3 * Radius;

            /*! \brief Controls slope.
                \details Parameter is used in River::IntegrationParams::WeightFunction
            */
            double exponant = 2.;
            
            /*! Weight function used in computation of series parameters.
                \snippet physmodel.hpp WeightFunc
            */
            inline double WeightFunction(const double r) const
            {
                //! [WeightFunc]
                return exp(-pow(r / weigth_func_radius, exponant));
                //! [WeightFunc]
            }
            
            ///Base Vector function used in computation of series parameters.
            inline double BaseVector(const int nf, const complex<double> zf) const
            {
                if( (nf % 2) == 0)
                    return -imag(pow(zf, nf/2.));
                else
                    return real(pow(zf, nf/2.));
                
            }

            ///Base Vector function used in computation of series parameters.
            inline double BaseVectorFinal(const int nf, const double angle, const double dx, const double dy ) const
            {
                return BaseVector(nf, 
                    exp(-complex<double>(0.0, 1.0)*angle)
                    *(dx + complex<double>(0.0, 1.0)*dy));
            }

            ///Prints options structure to output stream.
            friend ostream& operator <<(ostream& write, const IntegrationParams & ip);

            bool operator==(const IntegrationParams& ip) const;
    };

    /*! \brief Holds All parameters used in Deal.II solver.
    */
    class SolverParams
    {
        public:
            ///Tollerarnce used by dealii Solver.
            double tollerance = 1.e-12;

            ///Number of solver iteration steps
            unsigned num_of_iterrations = 6000;

            ///Number of adaptive refinment steps.
            unsigned adaptive_refinment_steps = 2;

            ///Fraction of refined mesh elements.
            double refinment_fraction = 0.1;

            ///Polynom degree of quadrature integration.
            unsigned quadrature_degree = 3;

            ///Renumbering algorithm(0 - none, 1 - cuthill McKee, 2 - hierarchical, 3 - random, ...) for the degrees of freedom on a triangulation.
            unsigned renumbering_type = 0;

            ///maximal distance between middle point and first solved point, used in non euler growth.
            double max_distance = 0.002;

            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const SolverParams & mp);

            bool operator==(const SolverParams& sp) const;
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

            Boundaries border;

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

            //Model parameters
            ///Field value used for Poisson conditions.
            double field_value = 1.0;
            
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
            ///More details about that you can find at [PMorawiecki work]()
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
