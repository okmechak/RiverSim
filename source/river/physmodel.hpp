#pragma once

#include <iostream>
#include <cmath>
#include <functional>

#include "common.hpp"

using namespace std;

namespace River
{

    class Model
    {   
        public: 
            double eps = 1e-4;
            /*
                                Different parameters that is used in river simulation
                            */
            enum class Method {Royal, Simple, Min, Integral};
            double ac = 0.;
            static constexpr double ds = 0.01;
            double dx = 0.5;
            double width = 1.;
            double height = 1.;
            static constexpr double Rmin = 0.0001;
            static constexpr double Rmax = 0.01;
            double biffurcationThreshold = 0.1;
            
            bool q_biffurcate(double a1, double a2, double a3);
            static Polar next_point(vector<double> series_params);

            static function<double(double, double)> Gain(int index);
            
        private:



    };
}