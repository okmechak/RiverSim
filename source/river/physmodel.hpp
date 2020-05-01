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
    {
        public:

            Parameter() = default;
            Parameter (const Parameter &old_obj) = default;
            Parameter(bool val, string d = "")
            {
                value_type = TYPE::BOOL;
                bool_val = val;
                description = d;
            }
            Parameter(unsigned val, string d = "")
            {
                value_type = TYPE::UNSIGNED;
                unsigned_val = val;
                description = d;
            }
            Parameter(double val, string d = "")
            {
                value_type = TYPE::DOUBLE;
                double_val = val;
                description = d;
            }
            ~Parameter(){}

            operator bool() const 
            { 
                if(value_type != TYPE::BOOL)
                    throw Exception("Parameter wrong cast");

                return bool_val; 
            }

            operator unsigned() const 
            { 
                if(value_type != TYPE::UNSIGNED)
                    throw Exception("Parameter wrong cast");

                return unsigned_val; 
            }

            operator double() const 
            { 
                if(value_type != TYPE::DOUBLE)
                    throw Exception("Parameter wrong cast");

                return double_val; 
            }

            Parameter& operator=(const Parameter& p)
            {
                if(value_type != TYPE::UNDEFINED && value_type != p.value_type)
                    throw Exception("Parameter: Can't assign types are different.");
                
                value_type = p.value_type;
                description = p.description;
                max_value = p.max_value;
                min_value = p.min_value;

                switch(p.value_type)
                {
                    case Parameter::TYPE::BOOL:
                        bool_val = p.bool_val;       
                        break;
                    case Parameter::TYPE::UNSIGNED:
                        unsigned_val = p.unsigned_val;
                        break;
                    case Parameter::TYPE::DOUBLE:
                        double_val = p.double_val;
                        break;
                    default:
                        throw Exception("Unknown parameter type");
                }   
                
                return *this;
            }

            Parameter& operator=(bool val)
            {
                if(value_type == TYPE::UNDEFINED || value_type == TYPE::BOOL)
                {
                    value_type = TYPE::BOOL;
                    bool_val = val;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return *this;
            }

            Parameter& operator=(unsigned val)
            {
                if(value_type == TYPE::UNDEFINED || value_type == TYPE::UNSIGNED)
                {
                    value_type = TYPE::UNSIGNED;
                    unsigned_val = val;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return *this;
            }

            Parameter& operator=(double val)
            {
                if(value_type == TYPE::UNDEFINED || value_type == TYPE::DOUBLE)
                {
                    value_type = TYPE::DOUBLE;
                    double_val = val;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return *this;
            }

            bool operator==(const Parameter& p) const
            {
                return value_type == p.value_type &&
                    bool_val == p.bool_val &&
                    unsigned_val == p.unsigned_val &&
                    abs(double_val - p.double_val) < EPS &&
                    description == p.description;
            }

            bool operator==(const bool& val) const
            {
                if(value_type == TYPE::BOOL)
                {
                    return bool_val == val;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return false;
            }

            bool operator==(const unsigned& val) const
            {
                if(value_type == TYPE::UNSIGNED)
                {
                    return unsigned_val == val;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return false;
            }

            bool operator==(const double& val) const
            {
                if(value_type == TYPE::DOUBLE)
                {
                    return abs(double_val - val) < EPS;
                }
                else
                    throw Exception("Parameter: Can't assign types are different.");
                
                return false;
            }

            friend ostream& operator <<(ostream& write, const Parameter & p)
            {
                switch(p.value_type)
                {
                    case Parameter::TYPE::UNDEFINED:
                        write << "Variable isn't initialized.";
                        break;
                    case Parameter::TYPE::BOOL:
                        write << p.bool_val;
                        break;
                    case Parameter::TYPE::UNSIGNED:
                        write << p.unsigned_val;
                        break;
                    case Parameter::TYPE::DOUBLE:
                        write << p.double_val;
                        break;
                    default:
                        throw Exception("Unknown parameter type");
                }
                write << " - " << p.description;
                return write;
            }
            
        private:
        
            union{
                bool bool_val;
                unsigned unsigned_val;
                double double_val;
            };
            enum TYPE{
                UNDEFINED,
                BOOL, 
                UNSIGNED, 
                DOUBLE} 
                value_type = TYPE::UNDEFINED;
            
            string description;
            double min_value = -100000, max_value = 100000;
    };

    typedef map<t_branch_id, vector<vector<double>>> t_SeriesParameters;
    class SeriesParameters: public t_SeriesParameters
    {
        public:
            void record(const map<t_branch_id, vector<double>>& id_series_params)
            {
                for(const auto&[branch_id, series_params]: id_series_params)
                {
                    if (!this->count(branch_id))
                        (*this)[branch_id] = {{}, {}, {}};
                    
                    size_t i = 0;
                    for(const auto& sp: series_params)
                    {
                        (*this)[branch_id].at(i).push_back(sp);
                        ++i;
                    }
                } 
            }
    };
    
    typedef map<string, vector<double>> SimulationData;

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

    struct BackwardData
    {
        vector<double> a1, a2, a3;
        vector<Point> init, backward, backward_forward;
        double branch_lenght_diff = -1;

        bool operator==(const BackwardData& data) const
        {
            return 
                a1 == data.a1 && a2 == data.a2 && a3 == data.a3 
                && init == data.init 
                && backward == data.backward 
                && backward_forward == data.backward_forward
                && branch_lenght_diff == data.branch_lenght_diff;
        }

        friend ostream& operator <<(ostream& write, const BackwardData & data)
        {
            for(size_t i = 0; i < data.a1.size(); ++i)
            {
                write 
                    << "a1 = " << data.a1.at(i)
                    << ", a2 = " << data.a2.at(i)
                    << ", a3 = " << data.a3.at(i) << endl;
                
                write 
                    << "init point = " << data.init.at(i) 
                    << ", backward point = " << data.backward.at(i)
                    << ", backward-forward point = " << data.backward_forward.at(i) << endl;
            }
            
            write << "branch diff = " << data.branch_lenght_diff << endl;

            return write;
        }
    };

    typedef map<t_branch_id, BackwardData> t_GeometryDiffernceNew;

    /*! \brief Global program options. 
        \details Program has some options that isn't part simulation itself but rather ease of use.
        So this object is dedicated for such options.
     */
    class ProgramOptions: public map<string, Parameter>
    {
        public:
            ProgramOptions()
            {
                auto po = *this;

                po["simulation_type"] = Parameter(0u, 
                    "Simulation type: 0 - Forward, 1 - Backward, 2 - For test purposes");
                po["number_of_steps"] = Parameter(10u, 
                    "Number of simulation steps.");
                po["maximal_river_height"] = Parameter(100u, 
                    "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.");
                po["number_of_backward_steps"] = Parameter(1u, 
                    "Number of backward steps simulations used in backward simulation type.");
                po["save_vtk"] = Parameter(false, 
                    "Outputs VTK file of Deal.II solution.");
                po["save_each_step"] = Parameter(false, 
                    "Save each step of simulation in JSON separate file.");
                po["verbose"] = Parameter(true, 
                    "If true - then program will print to standard output.");
                po["debug"] = Parameter(false, 
                    "If true - then program will save additional output files for each stage of simulation.");
            }

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

            bool operator==(const ProgramOptions& po) const
            {
                return simulation_type == po.simulation_type 
                    && number_of_steps == po.number_of_steps
                    && abs(maximal_river_height - po.maximal_river_height) < EPS
                    && number_of_backward_steps == po.number_of_backward_steps
                    && save_vtk == po.save_vtk
                    && save_each_step == po.save_each_step
                    && verbose == po.verbose
                    //&& output_file_name == po.output_file_name
                    && input_file_name == po.input_file_name;
            }
    };
    
    /*! \brief Adaptive mesh area constraint function.
        \details
        MeshParams holds all parameters used by mesh generation(see \ref triangle.hpp, \ref mesh.hpp)
     */
    class MeshParams: public map<string, Parameter>
    {
        public:
            MeshParams()
            {
                auto mp = *this;

                mp["refinment_radius"] = Parameter(4*Radius, 
                    "Radius of mesh refinment.");
                mp["exponant"] = Parameter(7., 
                    "This value controlls transition slope between small mesh elements and big or course.");
                mp["exponant"] = Parameter(1.9, 
                    "This number is used to stop simulation if some tip point of river gets bigger y-coord then the parameter value.");
                mp["sigma"] = Parameter(1u, 
                    "Sigma is used in exponence.");
                mp["static_refinment_steps"] = Parameter(0u, 
                    "Number of mesh refinment steps used by Deal.II mesh functionality.");
                mp["min_area"] = Parameter(7e-4, 
                    "Minimal area of mesh.");
                mp["max_area"] = Parameter(1e5, 
                    "Maximal area of mesh element.");
                mp["min_angle"] = Parameter(30., 
                    "Minimal angle of mesh element.");
                mp["max_edge"] = Parameter(1., 
                    "Maximal edge size.");
                mp["min_edge"] = Parameter(8e-8, 
                    "Minimal edge size.");
                mp["ratio"] = Parameter(2.3, 
                    "Ratio of the triangles.");
            }
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

            ///Maximal edge size.
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

            bool operator==(const MeshParams& mp) const
            {
                return 
                    abs(refinment_radius - mp.refinment_radius) < EPS 
                    && abs(exponant  - mp.exponant) < EPS
                    && static_refinment_steps == mp.static_refinment_steps
                    && abs(min_area  - mp.min_area) < EPS
                    && abs(max_area  - mp.max_area) < EPS
                    && abs(min_angle - mp.min_angle) < EPS
                    && abs(max_edge  - mp.max_edge) < EPS
                    && abs(min_edge  - mp.min_edge) < EPS
                    && abs(ratio     - mp.ratio) < EPS
                    && abs(eps       - mp.eps) < EPS;
            }
    };

    /*! \brief Holds parameters used by integration of series paramets functionality(see River::Solver::integrate())
    */
    class IntegrationParams: public map<string, Parameter>
    {
        public:
            
            IntegrationParams()
            {
                auto ip = *this;

                ip["weigth_func_radius"] = Parameter(Radius, 
                    "Circle radius with centrum in tip point.");
                ip["integration_radius"] = Parameter(3 * Radius, 
                    "Circle radius with centrum in tip point.");
                ip["exponant"] = Parameter(2., 
                    "TControls slope.");
            }
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

            bool operator==(const IntegrationParams& ip) const
            {
                return 
                    abs(weigth_func_radius - ip.weigth_func_radius) < EPS 
                    && abs(integration_radius  - ip.integration_radius) < EPS
                    && abs(exponant - ip.exponant) < EPS;
            }
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

            bool operator==(const SolverParams& sp) const
            {
                return 
                    abs(tollerance - sp.tollerance) < EPS 
                    && num_of_iterrations  == sp.num_of_iterrations
                    && adaptive_refinment_steps == sp.adaptive_refinment_steps
                    && abs(refinment_fraction - sp.refinment_fraction) < EPS
                    && quadrature_degree == sp.quadrature_degree
                    && renumbering_type == sp.renumbering_type
                    && abs(max_distance - sp.max_distance) < EPS;
            }
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

            Timing timing;

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
                bool dist_flag = branch_lenght >= bifurcation_min_dist;

                if(bifurcation_type == 0)
                {
                    if(prog_opt.verbose)
                        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold
                             << " branch_lenght = " << branch_lenght << ", bifurcation_min_dist = " << bifurcation_min_dist << endl;
                    return (a.at(2)/a.at(0) <= bifurcation_threshold) && dist_flag;
                }
                else if(bifurcation_type == 1)
                {
                    if(prog_opt.verbose)
                        cout << "a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold_2
                             << " branch_lenght = " << branch_lenght << ", bifurcation_min_dist = " << bifurcation_min_dist << endl;
                    return (a.at(0) >= bifurcation_threshold_2) && dist_flag;
                }
                else if(bifurcation_type == 2)
                {
                    if(prog_opt.verbose)
                        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold
                             << " a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold_2
                             << " branch_lenght = " << branch_lenght << ", bifurcation_min_dist = " << bifurcation_min_dist << endl;
                    return a.at(2)/a.at(0) <= bifurcation_threshold && a.at(0) >= bifurcation_threshold_2 && dist_flag;
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
                cout << "a1 = " << a.at(0) << ", growth threshold = " << growth_threshold << endl;
                return a.at(0) >= growth_threshold;
            }

            ///Evaluate next point of simualtion based on series parameters around tip.
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

            ///Checks if values of parameters are in normal ranges.
            void CheckParametersConsistency() const;

            ///Prints model structure and its subclasses
            friend ostream& operator <<(ostream& write, const Model & mdl);

            bool operator==(const Model& model) const
            {
                return 
                       abs(dx - model.dx) < EPS 
                    && abs(width - model.width) < EPS 
                    && abs(height - model.height) < EPS 
                    && abs(field_value - model.field_value) < EPS 
                    && abs(eta - model.eta) < EPS 
                    && abs(bifurcation_threshold - model.bifurcation_threshold) < EPS 
                    && abs(bifurcation_threshold_2 - model.bifurcation_threshold_2) < EPS 
                    && abs(bifurcation_min_dist - model.bifurcation_min_dist) < EPS 
                    && abs(bifurcation_angle - model.bifurcation_angle) < EPS 
                    && abs(growth_threshold - model.growth_threshold) < EPS 
                    && abs(growth_min_distance - model.growth_min_distance) < EPS 
                    && abs(ds - model.ds) < EPS 

                    && river_boundary_id  == model.river_boundary_id
                    && bifurcation_type == model.bifurcation_type
                    && growth_type == model.growth_type;
            }
    };
}
