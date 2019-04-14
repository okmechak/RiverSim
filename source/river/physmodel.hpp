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
            double refinment_radius = 0.3;

            ///Power.
            double exponant = 4;

            ///Minimal area of mesh.
            double min_area = 5e-9;

            ///Maximal area of mesh element.
            double max_area = 10;

            ///Minimal angle of mesh element.
            double min_angle = 30;

            ///Width of branch.
            double eps = 1e-6;


            double operator()(double x, double y)
            {
                vector<double> area_constraint(tip_points.size(), 100/*some large area value*/);
                for(auto& tip: tip_points)
                    area_constraint.push_back(
                        1 + min_area - exp( - pow( (Point{x, y} - tip).norm()/refinment_radius, exponant))
                    );
                return *min_element(area_constraint.begin(), area_constraint.end());
            }
    };

    class IntegrationParams
    {
        public:
            ///Circle radius with centrum in tip point.
            double integration_radius = 0.01;

            ///Parameter is used in evaluation of weight function.
            double exponant = 2.;
            
            ///Weight function used in computation of series parameters.
            double WeightFunction(double r)
            {
                return exp(-pow(r/integration_radius, exponant));
            }
            
            ///Base Vector function used in computation of series parameters.
            double BaseVector(int nf, complex<double> zf)
            {
                if( (nf % 2) == 0)
                    return -imag(pow(zf, nf/2.));
                else
                    return real(pow(zf, nf/2.));
                
            }

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
            double eta = 0.0;
            ///Biffurcation threshold.
            double biffurcation_threshold = -0.1;//Probably should be -0.1
            ///Biffurcation type. 0 - a3/a2, 1 - proportionallity to a1, 2 - no biffurcation.
            int biffurcation_type = 0;
            ///Biffurcation angle.
            double biffurcation_angle = M_PI/5;
            ///Growth type. 0 - arctan(a2/a1), 1 - {dx, dy}
            int growth_type = 0;
            ///Growth of branch will be done only if a1 > growth-threshold.
            int growth_threshold = 0;
            
            //Numeriacal parameters
            ///Proportionality value to one step growth.
            double ds = 0.01;
            
            //Mesh and mesh refinment parameters
            MeshParams mesh;

            //Series parameteres integral parameters
            IntegrationParams integr;

            ///Checks by evaluating series params for biffuraction condition.
            bool q_biffurcate(vector<double> a)
            {
                if(biffurcation_type == 0)
                {
                    cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcation_threshold << endl;
                    return a.at(2)/a.at(0) < biffurcation_threshold;
                }
                else if(biffurcation_type == 1)
                {
                    cout << "a1 = " <<  a.at(0) << ", bif thr = " << biffurcation_threshold << endl;
                    return a.at(0) > biffurcation_threshold;
                }
                else if(biffurcation_type == 2)
                    return false;
                else 
                    throw invalid_argument("Wrong biffurcation_type value!");
            }

            ///Checks by evaluating series param for growth condition.
            bool q_growth(vector<double> a)
            {
                return a.at(0) > growth_threshold;
            }

            ///Evaluate next point of simualtion based on series parameters around tip.
            Polar next_point(vector<double> series_params)
            {
                auto beta = series_params.at(0)/series_params.at(1),
                    dl = ds * pow(series_params.at(0), eta);
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
            
            vector<int> GetZeroIndices()
            {
                if(boundary_condition == 0)
                    return boundary_ids;
                else if(boundary_condition == 1)
                    return {river_boundary_id};
                else
                    throw invalid_argument("Invalid value of boundary_condition");   
            }

            vector<int> GetNonZeroIndices()
            {
                if(boundary_condition == 0)
                    return {};
                else if(boundary_condition == 1)
                    return {boundary_ids.at(1)};
                else
                    throw invalid_argument("Invalid value of boundary_condition");   
            }

            //TODO implement in this class parameter checking
            void CheckParametersConsistency();
            

    };
}