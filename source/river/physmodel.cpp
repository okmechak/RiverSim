#include <iostream>

#include "physmodel.hpp"

using namespace std;

namespace River
{
    bool Model::q_biffurcate(double a1, double a2, double a3)
    {
        //TODO implement
        return false;
    }

    function<double(double, double)> Model::Gain(int index)
    {
        switch(index)
        {
            //FIXME: divide by Rmax - Rmin
            case 0:
                return [](double R, double phi)
                    {return cos(phi/2)/M_PI/sqrt(R);};
            case 1:
                return [](double R, double phi)
                    {return sin(phi)/M_PI/R;};
            case 2:
                return [](double R, double phi)
                    {return cos(3*phi/2)/M_PI/sqrt(R)/R;};
            default:
                throw std::invalid_argument( "index should be 0 or 1");
                break;
        }
    }

    GeomPolar Model::next_point(vector<double> series_params)
    {
        double phi = - atan(2*series_params[2]/series_params[1]*sqrt(ds));
        return {ds, phi};
    }

}