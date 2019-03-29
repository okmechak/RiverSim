#pragma once

#include <iostream>
#include <cmath>
#include <functional>
#include <complex>
#include <math.h>
#include <algorithm>

#include "common.hpp"

using namespace std;

namespace River
{
    //FIXME:
    class AreaConstraint
    {
        public:
            //FIXME: implement this refinment function in better way
            vector<Point> tip_points;
            double r0 = 0.2;
            double exponant = 4;
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


    class Model
    {   
        public: 
            /*
                Different parameters that is used in river simulation
            */
            double eps = 1e-4;
            enum class Method {Royal, Simple, Min, Integral};
            double ac = 0.;
            static constexpr double ds = 0.002;
            double dx = 0.5;
            double width = 1.;
            double height = 1.;
            static constexpr double Rmin = 0.0000000001;
            static constexpr double Rmax = 0.01;
            double RMaxMesh = 0.01;
            double MinConstraintArea = 0.0000001;
            double MeshExponant = 1;
            double biffurcationThreshold = 0.1;
            static constexpr int exponant = 2;

            bool q_biffurcate(double a1, double a2, double a3);
            static Polar next_point(vector<double> series_params);
 
            static function<double(double, double)> Gain(int index);
            
        //private: FIXME: how to test private members???
            static double BaseVector(int nf, complex<double> zf)
            {
                if( (nf % 2) == 0)
                    return -imag(pow(zf, nf/2.));
                else
                    return real(pow(zf, nf/2.));
                
            }

            static double WeightFunction(double r)
            {
                return exp(-pow(r/Rmax, exponant));
            }
    };
}