#include <iostream>

#include "physmodel.hpp"

using namespace std;

namespace River
{
    bool Model::q_biffurcate(vector<double> a)
    {
        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcationThreshold << endl;
        return a.at(2)/a.at(0) < biffurcationThreshold;
    }
}