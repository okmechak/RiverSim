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
#include "border.hpp"
#include "tethex.hpp"

using namespace std;

///Characteristic radius used by River::MeshParams object.
#define Radius 0.01

namespace River
{
    /*! \brief Timing is used for measuring time of each evaluation cycle and whole program simulation time.
        \details
        At creation of object it records current time into string.
        Then \ref River::Timing::Record() can be used for erecording each cycle time.
    */
    class Timing
    {
        public:
            
            ///Default Constructor, saves time of creation.
            Timing():
                tik(clock()),
                creation_date(chrono::high_resolution_clock::to_time_t(chrono::high_resolution_clock::now()))
            {
                tik = clock();
            }

            ///Return current date string.
            string CurrentDate() const
            {   
                auto now = chrono::high_resolution_clock::now();
                auto t_time = chrono::high_resolution_clock::to_time_t(now);
                return ctime(&t_time);
            }

            ///Return date of object creation string.
            string CreationtDate() const
            {   
                return ctime(&creation_date);
            }

            ///Returns elapsed time from last \ref Record() call and saves it into vector.
            double Record()
            {
                auto tok = clock();
                records.push_back((double)(tok - tik)/CLOCKS_PER_SEC);
                tik = tok;
                return records.back();
            }


            ///Returns total time of simmulation.
            double Total() const
            {
                double sum = 0;
                for(auto t: records)
                    sum += t;
                return sum;
            }

            ///Vector that holds all times between Record() calls.
            vector<double> records;
        
        private:

            ///Stores last time from callig Record function.
            clock_t tik;
            ///Used for time record.
            clock_t tok;
            ///Creation date of object
            time_t creation_date;
    };

    /**
     * This structure holds comparsion data from Backward river simulation.
     */
    struct GeometryDifference
    {
        ///holds for each branch id all its series parameters: a1, a2, a3
        map<int, vector<vector<double>>> branches_series_params_and_geom_diff;
        ///series params info in bifurcation points
        map<int, vector<vector<double>>> branches_bifuraction_info;

        ///Used for backward river simulation data gathering.
        void StartBifurcationRecord(int br_id, double bifurcation_difference)
        {
            if(bif_difference.count(br_id))
                throw Exception("StartBifurcationRecord. Such branch already recorded, id: " + to_string(br_id));
            else
            {
                bif_difference[br_id] = bifurcation_difference;
            }
        }

        ///Used for backward river simulation data gathering.
        void EndBifurcationRecord(int br_id, vector<double> series_params)
        {
            if(bif_difference.count(br_id))
            {
                RecordBifurcationPoint(br_id, bif_difference[br_id], series_params);
                bif_difference.erase(br_id);
            }
        }

        ///Record branch seriesc parameters and geometry difference.
        void RecordBranchSeriesParamsAndGeomDiff(int branch_id, double dalpha, double ds, const vector<double>& series_params)
        {
            if(!branches_series_params_and_geom_diff.count(branch_id))
            {
                branches_series_params_and_geom_diff[branch_id] = vector<vector<double>>{{0}, {0}, {0}, {0}, {0}};

                branches_series_params_and_geom_diff[branch_id].at(0/*bif difference index*/).at(0) = dalpha;
                branches_series_params_and_geom_diff[branch_id].at(1/*bif difference index*/).at(0) = ds;

                branches_series_params_and_geom_diff[branch_id].at(2/*a1 index*/).at(0) = series_params.at(0);
                branches_series_params_and_geom_diff[branch_id].at(3/*a2 index*/).at(0) = series_params.at(1);
                branches_series_params_and_geom_diff[branch_id].at(4/*a3 index*/).at(0) = series_params.at(2);
            }
            else
            {
                branches_series_params_and_geom_diff[branch_id].at(0/*bif difference index*/).push_back( dalpha);
                branches_series_params_and_geom_diff[branch_id].at(1/*bif difference index*/).push_back( ds);

                branches_series_params_and_geom_diff[branch_id].at(2/*a1 index*/).push_back(series_params.at(0));
                branches_series_params_and_geom_diff[branch_id].at(3/*a2 index*/).push_back(series_params.at(1));
                branches_series_params_and_geom_diff[branch_id].at(4/*a3 index*/).push_back(series_params.at(2));
            }
        }

        private: 

            ///holds difference between adjacent branches and id of source branch.
            ///So called branch inconsistency at backward river simulation.
            map<int, double> bif_difference;

            ///Record bifurcation point
            void RecordBifurcationPoint(int branch_id, double bif_difference, const vector<double>& bif_series_params)
            {
                if(!branches_bifuraction_info.count(branch_id))
                {
                    branches_bifuraction_info[branch_id] = vector<vector<double>>{{0}, {0}, {0}, {0}};
                    branches_bifuraction_info[branch_id].at(0/*biff difference index*/).at(0) = bif_difference;

                    branches_bifuraction_info[branch_id].at(1/*a1 index*/).at(0) = bif_series_params.at(0);
                    branches_bifuraction_info[branch_id].at(2/*a2 index*/).at(0) = bif_series_params.at(1);
                    branches_bifuraction_info[branch_id].at(3/*a3 index*/).at(0) = bif_series_params.at(2);
                }
                else
                {
                    branches_bifuraction_info[branch_id][0/*biff difference index*/].push_back(bif_difference);

                    branches_bifuraction_info[branch_id].at(1/*a1 index*/).push_back(bif_series_params.at(0));
                    branches_bifuraction_info[branch_id].at(2/*a2 index*/).push_back(bif_series_params.at(1));
                    branches_bifuraction_info[branch_id].at(3/*a3 index*/).push_back(bif_series_params.at(2));
                }
            }
    };

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

            string output_file_name = "simdata",
                input_file_name = "";
            
            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const ProgramOptions & po);
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

            ///Number of quadrangle elements.
            unsigned long number_of_quadrangles = 0;

            ///Number of refined quadrangle elements.
            unsigned long number_of_refined_quadrangles = 0;

            ///Radius of mesh refinment.
            double refinment_radius = 4*Radius;

            ///This value controlls transition slope between small mesh elements and big or course.
            double exponant = 7.;

            /*! \brief Sigma is used in exponence, also as \ref River::MeshParams::exponant controls slope. */
            double sigma = 1.9;

            /*! \brief Number of mesh refinment steps used by Deal.II mesh functionality.
                \details Refinment means splitting one rectangular element into four rectagular elements.
            */ 
            unsigned static_refinment_steps = 3;

            ///Minimal area of mesh.
            double min_area = 7e-8;

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
                //! [MeshConstrain]
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
                //! [MeshConstrain]
            }

            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const MeshParams & mp);
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
            unsigned adaptive_refinment_steps = 0;

            ///Fraction of refined mesh elements.
            double refinment_fraction = 0.1;

            ///Polynom degree of quadrature integration.
            unsigned quadrature_degree = 3;

            ///Renumbering algorithm(0 - none, 1 - cuthill McKee, 2 - hierarchical, 3 - random, ...) for the degrees of freedom on a triangulation.
            unsigned renumbering_type = 0;

            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const SolverParams & mp);
    };

    /*! \brief Physical model parameters.
        \details 
        Holds parameters related to model. Region, numerical preocessing, parameters of growth etc.
    */
    class Model
    {   
        public: 
            Border border;
            Tree tree;
            Timing timing;
            GeometryDifference geometry_difference;

            ///Some global program options
            ProgramOptions prog_opt;

            ///Mesh and mesh refinment parameters
            MeshParams mesh;

            ///Series parameteres integral parameters
            IntegrationParams integr;

            ///Solver parameters used by Deal.II
            SolverParams solver_params;

            void InitializeBorderAndTree();

            //Geometrical parameters
            ///Initial x position of source.
            ///Valid only for rectangular area.
            double dx = 0.2;

            ///Width of region
            double width = 1.;

            ///Height of region
            double height = 1.;

            ///river boundary id and bottom line
            int river_boundary_id = 4;

            ///all boundaries ids in next order - right, top, left, bottom and river.
            vector<int> boundary_ids{1, 5, 3, river_boundary_id};

            //Model parameters
            /*! \brief Boundary conditions.
                \details 0 - Poisson(indexes 0,1 and 3 corresponds to free boundary condition, 4 - to zero value on boundary), 
                1 - Laplacea(Indexes 1 and 3 - free condition, 2 - value one, and 4 - value zero.)
            */
            unsigned boundary_condition = 0;

            ///Field value used for Poisson conditions.
            double field_value = 1.0;
            
            //Numeriacal parameters
            ///Maximal length of one step of growth.
            double ds = 0.003;

            ///Eta. Growth power of a1^eta
            double eta = 1.0;

            ///Bifurcation method type. 
            ///0 - a(3)/a(1) > bifurcation_threshold, 
            ///1 - a1 > bifurcation_threshold, 2 - combines both conditions, 3 - no bifurcation at all.
            unsigned bifurcation_type = 0;
            
            ///Bifurcation threshold for "0" bifurcation type.
            double bifurcation_threshold = -0.1;//Probably should be -0.1
            ///Bifurcation threshold for "1" bifurcation type.
            double bifurcation_threshold_2 = 0.001;//Probably should be -0.1

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
            bool q_bifurcate(vector<double> a, double branch_lenght) const
            {
                bool dist_flag = branch_lenght > bifurcation_min_dist;

                if(bifurcation_type == 0)
                {
                    if(prog_opt.verbose)
                        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
                    return a.at(2)/a.at(0) < bifurcation_threshold && dist_flag;
                }
                else if(bifurcation_type == 1)
                {
                    if(prog_opt.verbose)
                        cout << "a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold_2 << endl;
                    return a.at(0) > bifurcation_threshold_2 && dist_flag;
                }
                else if(bifurcation_type == 2)
                {
                    if(prog_opt.verbose)
                        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold
                             << " a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold_2 << endl;
                    return a.at(2)/a.at(0) < bifurcation_threshold && a.at(0) > bifurcation_threshold_2 && dist_flag;
                }
                else if(bifurcation_type == 3)
                    return false;
                else 
                    throw Exception("Wrong bifurcation_type value!");
            }

            ///Growth condition.
            ///Checks series parameters around river tip and evaluates if it is enough to grow.
            inline bool q_growth(vector<double> a) const
            {
                return a.at(0) > growth_threshold;
            }

            /*! \brief Evaluate next point of simualtion based on series parameters around tip.
                \todo test different types of growth. Especially growth_type == 1.
            */
            Polar next_point(vector<double> series_params, double branch_lenght, double max_a) const
            {
                //handle situation near bifurcation point, to reduce "killing/shading" one branch by another
                auto eta_local = eta;
                if(branch_lenght < growth_min_distance)
                    eta_local = 0;//constant growth of both branches.

                auto beta = series_params.at(0)/series_params.at(1),
                    dl = ds * pow(series_params.at(0)/max_a, eta_local);
                if(growth_type == 0)
                {
                    double phi = -atan(2 / beta * sqrt(dl));
                    return {dl, phi};
                }
                else if(growth_type == 1)
                {
                    auto dy = beta*beta/9*( pow(27/2*dl/beta/beta + 1, 2./3.) - 1),
                        dx = 2*sqrt( pow(dy, 3)/pow(beta, 2) + pow(dy, 4) / pow(beta, 3));
                        
                    return ToPolar(Point{dx, dy}.rotate(-M_PI/2));
                }
                else
                    throw Exception("Invalid value of growth_type!");
            }
            
            ///Return boundary line indices with zero value conditions.
            vector<int> GetZeroIndices() const
            {
                if(boundary_condition == 0)
                    return {river_boundary_id};//boundary_ids;
                else if(boundary_condition == 1)
                    return {river_boundary_id};
                else
                    throw Exception("Invalid value of boundary_condition");   
            }

            ///Return boundary line indices with non-zero value conditions.
            vector<int> GetNonZeroIndices() const
            {
                if(boundary_condition == 0)
                    return {};
                else if(boundary_condition == 1)
                    return {boundary_ids.at(1)};
                else
                    throw Exception("Invalid value of boundary_condition");   
            }

            ///Checks if values of parameters are in normal ranges.
            void CheckParametersConsistency() const;

            ///Prints model structure and its subclasses
            friend ostream& operator <<(ostream& write, const Model & mdl);
    };

    /*! \brief Finnal Boudary Geneartor Class
        \details
        Sticks together all components: Tree class, boudary class and model parameters.

        \todo reserve size of tet_lines.
     */ 
    tethex::Mesh BoundaryGenerator(const Model& model);
}
