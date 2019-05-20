/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/** @file physmodel.hpp
 *   Physical model incapsulation.
 *
 *   Holds all specific information about physical model of process.
 */
#pragma once

#include <iostream>
#include <cmath>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <map>

#include "common.hpp"

using namespace std;

namespace River
{
    /**
     * Adaptive mesh area constraint function.
     */
    class MeshParams
    {
        public:
            //FIXME: implement this refinment function in better way
            ///Vector of tip points.
            vector<Point> tip_points;

            ///Radius of refinment.
            double refinment_radius = 0.02;

            ///Power.
            double exponant = 100.;

            ///Minimal area of mesh.
            double min_area = 1e-9;

            ///Maximal area of mesh element.
            double max_area = 10.;

            ///Minimal angle of mesh element.
            double min_angle = 30.;

            ///Width of branch.
            double eps = 1e-6;

            ///Sigma used in exponence(same as in Gauss formula)
            double sigma = 2.;

            inline double operator()(double x, double y) const
            {
                double result_area = 10000000/*some large area value*/;
                for(auto& tip: tip_points)
                {
                    auto r = (Point{x, y} - tip).norm();
                    auto exp_val = exp( -pow(r/refinment_radius, exponant)/2/sigma/sigma);
                    auto cur_area = min_area + (max_area - min_area)*(1 - exp_val)/(1 + exp_val);
                    if(result_area > cur_area)
                        result_area = cur_area;   
                }
                
                return result_area;
            }
    };

    class IntegrationParams
    {
        public:
            ///Circle radius with centrum in tip point.
            double weigth_func_radius = 0.01;

            ///Circle radius with centrum in tip point.
            double integration_radius = 3 * weigth_func_radius;

            ///Parameter is used in evaluation of weight function.
            double exponant = 2.;
            
            ///Weight function used in computation of series parameters.
            inline double WeightFunction(double r) const
            {
                return exp(-pow(r / weigth_func_radius, exponant));
            }
            
            ///Base Vector function used in computation of series parameters.
            inline double BaseVector(int nf, complex<double> zf) const
            {
                if( (nf % 2) == 0)
                    return -imag(pow(zf, nf/2.));
                else
                    return real(pow(zf, nf/2.));
                
            }
    };
    /**
     * Holds All parameters used in solver.
     */
    class SolverParams
    {
        public:
            ///Polynom degree of quadrature integration.
            int quadrature_degree = 2;
            
            ///Fraction of refined mesh elements.
            double refinment_fraction = 0.01;

            ///Number of refinment steps.
            int refinment_steps = 5;
    };

    /**
     * Physical model.
     * 
     * Holds parameters related to model. Region, numerical preocessing, parameters of growth etc.
     */
    class Model
    {   
        public: 
            //Geometrical parameters
            ///Initial x position of source.
            ///Valid only for rectangular area.
            double dx = 0.5;
            ///Width of region
            double width = 1.;
            ///Height of region
            double height = 1.;
            ///river boundary id and bottom line
            int river_boundary_id = 4;
            ///all boundaries ids in next order - right, top, left, bottom and river.
            vector<int> boundary_ids{1, 2, 3, river_boundary_id};

            //Model parameters
            ///Boundary conditions, 0 - Poisson, 1 - Laplacea
            int boundary_condition = 0;
            ///Field value used for Poisson conditions.
            double field_value = 0.0;
            ///Eta. Power of a1^eta
            double eta = 1.0;
            ///Biffurcation type. 0 - a3/a2, 1 - proportionallity to a1,2 - combines both types,  3 - no biffurcation.
            int biffurcation_type = 2;
            ///Biffurcation threshold.
            double biffurcation_threshold = -0.1;//Probably should be -0.1
            double biffurcation_threshold_2 = 0.001;//Probably should be -0.1
            ///Minimal distance between adjacent biffurcation points. Reduces numerical noise.
            double biffurcation_min_dist = 0.05;
            ///Biffurcation angle.
            double biffurcation_angle = M_PI/10;
            ///Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}
            int growth_type = 0;
            ///Growth of branch will be done only if a1 > growth-threshold.
            int growth_threshold = 0;
            ///Distance of constant tip growing after biffurcation point. Reduces numerical noise.
            double growth_min_distance = 0.01;
            
            //Numeriacal parameters
            ///Proportionality value to one step growth.
            double ds = 0.01;
            
            ///Mesh and mesh refinment parameters
            MeshParams mesh;

            ///Series parameteres integral parameters
            IntegrationParams integr;

            ///Solver parameters used by Deal.II
            SolverParams solver_params;

            ///Checks by evaluating series params for biffuraction condition.
            bool q_biffurcate(vector<double> a, double branch_lenght) const
            {
                bool dist_flag = branch_lenght > biffurcation_min_dist;

                if(biffurcation_type == 0)
                {
                    cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcation_threshold << endl;
                    return a.at(2)/a.at(0) < biffurcation_threshold && dist_flag;
                }
                else if(biffurcation_type == 1)
                {
                    cout << "a1 = " <<  a.at(0) << ", bif thr = " << biffurcation_threshold_2 << endl;
                    return a.at(0) > biffurcation_threshold_2 && dist_flag;
                }
                else if(biffurcation_type == 2)
                {
                    cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcation_threshold
                        << " a1 = " <<  a.at(0) << ", bif thr = " << biffurcation_threshold_2 << endl;
                    return a.at(2)/a.at(0) < biffurcation_threshold && a.at(0) > biffurcation_threshold_2 && dist_flag;
                }
                else if(biffurcation_type == 3)
                    return false;
                else 
                    throw invalid_argument("Wrong biffurcation_type value!");
            }

            ///Checks by evaluating series param for growth condition.
            inline bool q_growth(vector<double> a) const
            {
                return a.at(0) > growth_threshold;
            }

            ///Evaluate next point of simualtion based on series parameters around tip.
            Polar next_point(vector<double> series_params, double branch_lenght) const
            {
                //handle situation near biffurcation point, to reduce killing one branch by another
                auto eta_local = eta;
                if(branch_lenght < growth_min_distance)
                    eta_local = 0;//constant growth of both branches.

                auto beta = series_params.at(0)/series_params.at(1),
                    dl = ds * pow(series_params.at(0), eta_local);
                if(growth_type == 0)
                {
                    double phi = -atan(2 / beta * sqrt(dl));
                    return {dl, phi};
                }
                else if(growth_type == 1)
                {
                    auto dy = beta*beta/9*( pow(27/2*dl/beta/beta + 1, 2./3.) - 1),
                        dx = 2*sqrt( pow(dy, 3)/pow(beta,2) + pow(dy, 4) / pow(beta, 3));

                    return Point{dx, dy}.getPolar();//TODO test this
                }
                else
                    throw invalid_argument("Invalid value of growth_type!");
            }
            
            vector<int> GetZeroIndices() const
            {
                if(boundary_condition == 0)
                    return {river_boundary_id};//boundary_ids;
                else if(boundary_condition == 1)
                    return {river_boundary_id};
                else
                    throw invalid_argument("Invalid value of boundary_condition");   
            }

            vector<int> GetNonZeroIndices() const
            {
                if(boundary_condition == 0)
                    return {};
                else if(boundary_condition == 1)
                    return {boundary_ids.at(1)};
                else
                    throw invalid_argument("Invalid value of boundary_condition");   
            }

            //TODO implement in this class parameter checking
            void CheckParametersConsistency() const;
    };




    /**
     * This structure holds comparsion data from Backward river simulation.
     */
    struct GeometryDifference
    {
        ///holds for each branch id all its series parameters: a1, a2, a3
        map<int, vector<vector<double>>> branches_series_params_and_geom_diff;
        ///series params info in biffurcation points
        map<int, vector<vector<double>>> branches_biffuraction_info;


        void StartBiffurcationRecord(int br_id, double biffurcation_difference)
        {
            if(bif_difference.count(br_id))
                throw invalid_argument("StartBiffurcationRecord. Such branch already recorded, id: " + to_string(br_id));
            else
            {
                bif_difference[br_id] = biffurcation_difference;
            }
        }

        void EndBiffurcationRecord(int br_id, vector<double> series_params)
        {
            if(bif_difference.count(br_id))
            {
                RecordBiffurcationPoint(br_id, bif_difference[br_id], series_params);
                bif_difference.erase(br_id);
            }
        }


        void RecordBranchSeriesParamsAndGeomDiff(int branch_id, double dalpha, double ds, const vector<double>& series_params)
        {
            if(!branches_series_params_and_geom_diff.count(branch_id))
            {
                branches_series_params_and_geom_diff[branch_id] = vector<vector<double>>{{0}, {0}, {0}, {0}, {0}};

                branches_series_params_and_geom_diff[branch_id].at(0/*biff difference index*/).at(0) = dalpha;
                branches_series_params_and_geom_diff[branch_id].at(1/*biff difference index*/).at(0) = ds;

                branches_series_params_and_geom_diff[branch_id].at(2/*a1 index*/).at(0) = series_params.at(0);
                branches_series_params_and_geom_diff[branch_id].at(3/*a2 index*/).at(0) = series_params.at(1);
                branches_series_params_and_geom_diff[branch_id].at(4/*a3 index*/).at(0) = series_params.at(2);
            }
            else
            {
                branches_series_params_and_geom_diff[branch_id].at(0/*biff difference index*/).push_back( dalpha);
                branches_series_params_and_geom_diff[branch_id].at(1/*biff difference index*/).push_back( ds);

                branches_series_params_and_geom_diff[branch_id].at(2/*a1 index*/).push_back(series_params.at(0));
                branches_series_params_and_geom_diff[branch_id].at(3/*a2 index*/).push_back(series_params.at(1));
                branches_series_params_and_geom_diff[branch_id].at(4/*a3 index*/).push_back(series_params.at(2));
            }
        }

        private: 

            map<int, double> bif_difference;

            void RecordBiffurcationPoint(int branch_id, double bif_difference, const vector<double>& bif_series_params)
            {
                if(!branches_biffuraction_info.count(branch_id))
                {
                    branches_biffuraction_info[branch_id] = vector<vector<double>>{{0}, {0}, {0}, {0}};
                    branches_biffuraction_info[branch_id].at(0/*biff difference index*/).at(0) = bif_difference;

                    branches_biffuraction_info[branch_id].at(1/*a1 index*/).at(0) = bif_series_params.at(0);
                    branches_biffuraction_info[branch_id].at(2/*a2 index*/).at(0) = bif_series_params.at(1);
                    branches_biffuraction_info[branch_id].at(3/*a3 index*/).at(0) = bif_series_params.at(2);
                }
                else
                {
                    branches_biffuraction_info[branch_id][0/*biff difference index*/].push_back(bif_difference);

                    branches_biffuraction_info[branch_id].at(1/*a1 index*/).push_back(bif_series_params.at(0));
                    branches_biffuraction_info[branch_id].at(2/*a2 index*/).push_back(bif_series_params.at(1));
                    branches_biffuraction_info[branch_id].at(3/*a3 index*/).push_back(bif_series_params.at(2));
                }
            }
    };
}