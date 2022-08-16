//Link to Boost
//#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Solver"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "solver.hpp"

using namespace River;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE( open_empty_mesh, 
    *utf::tolerance(EPS))
{   
    SolverParams solver_params;
    Solver solver(solver_params);

    tethex::Mesh mesh;

    BOOST_CHECK_NO_THROW(solver.OpenMesh(mesh));
}

BOOST_AUTO_TEST_CASE( Trapezoidal_Integration, 
    *utf::tolerance(EPS))
{   
    auto f = [](double x){return 2. * sqrt(1. - x * x);};
    auto I = qtrap(f, -1, 1, EPS);
    BOOST_TEST(I == M_PI);
}

BOOST_AUTO_TEST_CASE( a1, 
    *utf::tolerance(EPS))
{   
    IntegrationParams integ;

    double rho = 0.01;

    auto a1_f = [&](double phi) { 
        auto weight_func_value = integ.WeightFunction(0.01);
        auto value = sqrt(0.01) * cos(phi/2.);
        auto base_vector_value = integ.BaseVectorFinal(1, 0, 0.01 * cos(phi), 0.01 * sin(phi));
        return value * weight_func_value * base_vector_value * rho;
    }; 
    auto I = qtrap(a1_f, 0, 2 * M_PI, 0.01);

    auto n_a1_f = [&](double phi) { 
        return integ.WeightFunction(rho) 
            * pow(integ.BaseVectorFinal(1, 0, rho * cos(phi), rho * sin(phi)), 2) 
            * rho;
    };
    auto I_n = 1.;//qtrap(n_a1_f, 0, 2 * M_PI, 0.01);

    cout << to_string(I) << endl;
    cout << to_string(I_n) << endl;

    //BOOST_TEST((I/I_n) == 1.);
}

