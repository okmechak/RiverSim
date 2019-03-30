/*! \file physmodel.hpp
    Physical model incapsulation.

    Holds all specific information about physical model of process.
*/
#pragma once

#include <iostream>
#include <cmath>
#include <complex>
#include <math.h>
#include <algorithm>

#include "common.hpp"

using namespace std;

namespace River
{
    //FIXME:
    /**
     * Adaptive mesh area constraint function.
     */
    class AreaConstraint
    {
        public:
            //FIXME: implement this refinment function in better way
            ///Vector of tip points.
            vector<Point> tip_points;
            ///Radius of refinment.
            double r0 = 0.2;
            ///Power.
            double exponant = 4;
            ///Minimal area of mesh.
            double min_area = 1e-8;

            double operator()(double x, double y)
            {
                vector<double> area_constraint(tip_points.size(), 1/*some large area value*/);
                for(auto& tip: tip_points)
                    area_constraint.push_back(
                        1 + min_area - exp( - pow( (Point{x, y} - tip).norm()/r0, exponant))
                    );
                return *min_element(area_constraint.begin(), area_constraint.end());
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
            ///Width of branch.
            double eps = 1e-6;
            ///Enumeration of growth methods.
            enum class Method {Royal, Simple, Min, Integral};
            ///Proportionality value to one step growth.
            double ds = 0.01;
            ///Initial x position of source.
            ///Valid only for rectangular area.
            double dx = 0.5;
            ///Width of region
            double width = 1.;
            ///Height of region
            double height = 1.;
            ///Biffurcation threshold.
            double biffurcationThreshold = 0;//Probably should be -0.1
            ///Biffurcation angle.
            double biff_angle = M_PI/5;

            ///Checks by evaluating series params for biffuraction condition.
            bool q_biffurcate(vector<double> a)
            {
                cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcationThreshold << endl;
                return a.at(2)/a.at(0) < biffurcationThreshold;
            }

            ///Evaluate next point of simualtion based on series parameters around tip.
            Polar next_point(vector<double> series_params)
            {
                double phi = -atan(2 * series_params.at(1)/series_params.at(0) * sqrt(ds));
                return {ds, phi};
            }
            
        //private: FIXME: how to test private members???
            

            ///Circle radius with centrum in tip point.
            static constexpr double Rmax = 0.01;

            ///Parameter is used in evaluation of weight function.
            static constexpr int exponant = 2;
            
            ///Weight function used in computation of series parameters.
            static double WeightFunction(double r)
            {
                return exp(-pow(r/Rmax, exponant));
            }
            
            ///Base Vector function used in computation of series parameters.
            static double BaseVector(int nf, complex<double> zf)
            {
                if( (nf % 2) == 0)
                    return -imag(pow(zf, nf/2.));
                else
                    return real(pow(zf, nf/2.));
                
            }
    };
}