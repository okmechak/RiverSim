#pragma once

#include <iostream>
#include <cmath>
#include <functional>

#include "physmodel.hpp"
#include "geometry.hpp"

using namespace std;

namespace River
{

    class Model
    {   
        public: 
            double eps = 0.01;
            /*
                                Different parameters that is used in river simulation
                            */
            enum class Method {Royal, Simple, Min, Integral};
            double ac = 0.;
            double ds = 0.1;
            double dx = 0.5;
            double width = 1.;
            double height = 1.;
            double Rmin = 0.001;
            double Rmax = 0.05;
            double biffurcationThreshold = 0.1;
            
            bool q_biffurcate(double a1, double a2, double a3);
            GeomPolar next_point(vector<double> series_params);

            function<double(double, double)> Gain(int index);
            
        private:



    };
}